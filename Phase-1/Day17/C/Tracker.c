#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>

#define MAX_ALLOCS 1024

typedef struct {
    void *ptr;
    size_t size;
    const char *file;
    int line;
} alloc_info_t;

static alloc_info_t alloc_table[MAX_ALLOCS];
static size_t alloc_count = 0;
static _Thread_local bool inside_tracker = false;

// Real function pointers
static void *(*real_malloc)(size_t) = NULL;
static void (*real_free)(void *) = NULL;

static void init_real_functions(void) {
    if (!real_malloc) real_malloc = dlsym(RTLD_NEXT, "malloc");
    if (!real_free) real_free = dlsym(RTLD_NEXT, "free");
}

void *tracked_malloc(size_t sz, const char *file, int line) {
    init_real_functions();

    if (inside_tracker) {
        return real_malloc(sz);
    }

    inside_tracker = true;
    void *ptr = real_malloc(sz);
    
    if (ptr && alloc_count < MAX_ALLOCS) {
        alloc_table[alloc_count++] = (alloc_info_t){
            .ptr = ptr, 
            .size = sz, 
            .file = file, 
            .line = line
        };
    }
    
    inside_tracker = false;
    return ptr;
}

void tracked_free(void *ptr, const char *file, int line) {
    init_real_functions();

    if (!ptr) return;

    if (inside_tracker) {
        real_free(ptr);
        return;
    }

    inside_tracker = true;
    
    bool found = false;
    for (size_t i = 0; i < alloc_count; i++) {
        if (alloc_table[i].ptr == ptr) {
            alloc_table[i] = alloc_table[--alloc_count]; // Swap back
            found = true;
            break;
        }
    }

    if (!found) {
        printf("BAD FREE: %p at %s:%d\n", ptr, file, line);
    }

    real_free(ptr);
    inside_tracker = false;
}

void dump_leaks(void) {
    if (alloc_count == 0) {
        printf("NO LEAKS\n");
        return;
    }
    printf("LEAKS DETECTED:\n");
    for (size_t i = 0; i < alloc_count; i++) {
        printf("Leak %zu bytes at %p (%s:%d)\n", 
            alloc_table[i].size, alloc_table[i].ptr, 
            alloc_table[i].file, alloc_table[i].line);
    }
}

#define malloc(sz) tracked_malloc(sz, __FILE__, __LINE__)
#define free(ptr) tracked_free(ptr, __FILE__, __LINE__)

int main(void) {
    atexit(dump_leaks);

    int *p1 = malloc(sizeof(int) * 10);
    int *p2 = malloc(sizeof(int) * 20);
    (void)p2;

    free(p1); // p2 leaks!

    return 0;
}