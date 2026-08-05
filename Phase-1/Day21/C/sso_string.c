#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include<assert.h>

#define SSO_STACK_BUFFER_CAP 14

static size_t malloc_count = 0;
static size_t realloc_count = 0;
static size_t free_count = 0;

typedef struct {
    bool __is_long;
    union {
        struct {
            char *__ptr;
            size_t __size;
            size_t __capacity;
        } heap;
        struct {
            char __data[SSO_STACK_BUFFER_CAP + 1];
            uint8_t __size;
        } stack_buffer;
    } storage;
} sso_string;

void *tracked_malloc(size_t size) { //malloc wrapper
    malloc_count++;
    return malloc(size);
}

void *tracked_realloc(void *ptr, size_t size) { //realloc wrapper
    realloc_count++;
    return realloc(ptr, size);
}

void tracked_free(void *ptr) { //free wraper
    if (ptr) free_count++;
    free(ptr);
}

void reset_counters(void) {
    malloc_count = 0;
    realloc_count = 0;
    free_count = 0;
}

void sso_free(sso_string *ss)
{
    if(!ss) return;
    
    if(ss->__is_long)
    {
        tracked_free(ss->storage.heap.__ptr);
        ss->storage.heap.__ptr = NULL;
        ss->storage.heap.__size = 0;
        ss->storage.heap.__capacity = 0;
    }
    else
    {
        ss->storage.stack_buffer.__size = 0; 
        ss->storage.stack_buffer.__data[0] = '\0';
    }
    ss->__is_long = false;
}

sso_string *sso_init(sso_string *ss, const char *str)
{
    if(!ss) return NULL;
    if(!str) str = "";

    
    if(ss->__is_long) sso_free(ss);
    memset(ss, 0, sizeof(sso_string));

    size_t len = strlen(str);

    if(len <= SSO_STACK_BUFFER_CAP)
    {
        ss->__is_long = false;
        ss->storage.stack_buffer.__size = (uint8_t)len;
        memcpy(ss->storage.stack_buffer.__data, str, len + 1);
    }
    else
    {
        ss->__is_long = true;
        ss->storage.heap.__ptr = tracked_malloc(len + 1);
        if(!ss->storage.heap.__ptr) return NULL;
        ss->storage.heap.__size = len;
        ss->storage.heap.__capacity = len;
        memcpy(ss->storage.heap.__ptr, str, len + 1);
    }

    return ss;
}


sso_string *sso_append(sso_string *ss, const char *str)
{
    if(!str || !ss) return NULL;

    size_t append_len = strlen(str);

    size_t currLen = ss->__is_long ? ss->storage.heap.__size : ss->storage.stack_buffer.__size;
    if (SIZE_MAX - currLen < append_len) return NULL;

    size_t newLen =  currLen + append_len;

    if(!ss->__is_long)
    {

        if(newLen <= SSO_STACK_BUFFER_CAP)
        {
            memcpy(ss->storage.stack_buffer.__data + currLen, str, append_len + 1);
            ss->storage.stack_buffer.__size = (uint8_t)newLen;
        }
        else
        {
            size_t newCap = newLen * 2;
            char *new_ptr = tracked_malloc(newCap + 1);
            if(!new_ptr) return NULL;

            memcpy(new_ptr, ss->storage.stack_buffer.__data, currLen);
            memcpy(new_ptr + currLen, str, append_len + 1);

            ss->__is_long = true;
            ss->storage.heap.__ptr = new_ptr;
            ss->storage.heap.__capacity = newCap;
            ss->storage.heap.__size = newLen;
        }
    }
    else
    {
        if(newLen > ss->storage.heap.__capacity)
        {
            size_t newCap = ss->storage.heap.__capacity * 2;
            if(newCap < newLen) newCap = newLen;

            char *new_ptr = tracked_realloc(ss->storage.heap.__ptr, newCap + 1);
            if(!new_ptr) return NULL;

            ss->storage.heap.__capacity = newCap;
            ss->storage.heap.__ptr = new_ptr;
        }
        memcpy(ss->storage.heap.__ptr + currLen, str, append_len + 1);
        ss->storage.heap.__size = newLen;
    }

    return ss;
}

sso_string *sso_copy(sso_string *dist, sso_string *src)
{
    if(!dist || !src) return NULL;

    if(dist == src) return dist;

    sso_free(dist);

    dist->__is_long = src->__is_long;

    if(!src->__is_long)
    {
        dist->storage.stack_buffer = src->storage.stack_buffer;
    }
    else
    {
        size_t len = src->storage.heap.__size;
        dist->storage.heap.__ptr = tracked_malloc(len + 1);
        if(!dist->storage.heap.__ptr) return NULL;
        dist->storage.heap.__size = len;
        dist->storage.heap.__capacity = len;
        memcpy(dist->storage.heap.__ptr, src->storage.heap.__ptr, len + 1);
    }

    return dist;
}

const char *sso_get_cstr(sso_string *ss)
{
    if(!ss) return NULL;

    return ss->__is_long ? ss->storage.heap.__ptr : ss->storage.stack_buffer.__data;
}



int main(void) {
    sso_string ss1 = {0};
    sso_string ss2 = {0};

    // Test 1: Stack allocation <= 14 bytes
    reset_counters();
    sso_init(&ss1, "Hello");
    assert(!ss1.__is_long);
    assert(strcmp(sso_get_cstr(&ss1), "Hello") == 0);
    assert(malloc_count == 0); // Must be zero mallocs!
    sso_free(&ss1);
    assert(free_count == 0);
    printf("[PASS] Stack init\n");

    // Test 2: Stack append staying <= 14 bytes
    reset_counters();
    sso_init(&ss1, "Hello");
    sso_append(&ss1, " World"); // Length 11
    assert(!ss1.__is_long);
    assert(strcmp(sso_get_cstr(&ss1), "Hello World") == 0);
    assert(malloc_count == 0); // Still zero mallocs!
    sso_free(&ss1);
    assert(free_count == 0);
    printf("[PASS] Stack append\n");

    // Test 3: Stack to Heap transition (> 14 bytes)
    reset_counters();
    sso_init(&ss1, "Hello World"); // Length 11 (stack)
    sso_append(&ss1, "!!!");        // Length 14 (stack)
    assert(!ss1.__is_long);
    assert(malloc_count == 0);

    sso_append(&ss1, " SSO Mode Active"); // Crosses limit -> heap transition
    assert(ss1.__is_long);
    assert(strcmp(sso_get_cstr(&ss1), "Hello World!!! SSO Mode Active") == 0);
    assert(malloc_count == 1); // Exact 1 malloc on transition
    sso_free(&ss1);
    assert(free_count == 1);   // Exact 1 free
    printf("[PASS] Stack to Heap transition\n");

    // Test 4: Direct Heap init (> 14 bytes)
    reset_counters();
    sso_init(&ss1, "This string is way longer than 14 bytes");
    assert(ss1.__is_long);
    assert(malloc_count == 1);
    
    // Append on Heap triggering realloc
    sso_append(&ss1, " appending more heap data");
    assert(realloc_count == 1);
    sso_free(&ss1);
    assert(free_count == 1);
    printf("[PASS] Direct Heap init & realloc\n");

    // Test 5: sso_copy on stack-mode string, mutate copy, verify original untouched
    reset_counters();
    sso_init(&ss1, "StackOrig");
    assert(!ss1.__is_long);
    sso_copy(&ss2, &ss1);
    assert(!ss2.__is_long);
    assert(malloc_count == 0);
    sso_append(&ss2, "Mut");
    assert(strcmp(sso_get_cstr(&ss1), "StackOrig") == 0);
    assert(strcmp(sso_get_cstr(&ss2), "StackOrigMut") == 0);
    sso_free(&ss1);
    sso_free(&ss2);
    assert(free_count == 0);
    printf("[PASS] Test 5: Stack copy & mutate isolation\n");

    // Test 6: sso_copy on heap-mode string — deep copy, ptr check, mutate, clean free
    reset_counters();
    sso_init(&ss1, "Heap original string payload that forces heap mode");
    assert(ss1.__is_long);
    assert(malloc_count == 1);

    sso_copy(&ss2, &ss1);
    assert(ss2.__is_long);
    assert(malloc_count == 2); // Distinct allocation for destination
    assert(ss1.storage.heap.__ptr != ss2.storage.heap.__ptr); // Pointers MUST NOT match

    sso_append(&ss2, " [MUTATED COPY]");
    assert(strcmp(sso_get_cstr(&ss1), "Heap original string payload that forces heap mode") == 0);
    assert(strcmp(sso_get_cstr(&ss2), "Heap original string payload that forces heap mode [MUTATED COPY]") == 0);

    sso_free(&ss1);
    sso_free(&ss2);
    assert(free_count == 2); // Both heap buffers freed clean
    printf("[PASS] Test 6: Heap deep copy, pointer isolation & double free safety\n");

    // Test 7: sso_init called twice on same already-heap-mode ss — prove no leak
    reset_counters();
    sso_init(&ss1, "First long heap string payload to force allocation");
    assert(ss1.__is_long);
    assert(malloc_count == 1);

    sso_init(&ss1, "Second long heap string payload forcing re-init overwrite");
    assert(ss1.__is_long);
    assert(malloc_count == 2);
    assert(free_count == 1); // First allocation MUST be freed before second init

    sso_free(&ss1);
    assert(free_count == 2); // Final free balances total allocations
    printf("[PASS] Test 7: Double heap init leak prevention\n");

    printf("\nALL SSO TESTS PASSED PERFECTLY!\n");
    return 0;
}