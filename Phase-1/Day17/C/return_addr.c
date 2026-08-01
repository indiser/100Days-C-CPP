#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

void* malloc(size_t size) {
    // Find the real malloc function pointer
    static void* (*real_malloc)(size_t) = NULL;
    if (!real_malloc) {
        real_malloc = dlsym(RTLD_NEXT, "malloc");
    }

    // Inspect the call stack to see who called malloc
    void* caller = __builtin_return_address(0);

    // Print the raw instruction pointer address
    // (Note: Using a raw write/format here to mimic logging)
    printf("malloc(%zu) invoked by code at address: %p\n", size, caller);

    return real_malloc(size);
}
