#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>

// Declare a function pointer to hold the original malloc address
static void* (*real_malloc)(size_t) = NULL;

void* malloc(size_t size) {
    // 1. Check if the pointer to the real malloc is already initialized
    if (!real_malloc) {
        // Use dlsym with RTLD_NEXT to find the genuine libc malloc
        real_malloc = dlsym(RTLD_NEXT, "malloc");
        if (!real_malloc) {
            // Standard error handling using internal unistd write to avoid recursion
            write(2, "Error binding malloc\n", 21);
            return NULL;
        }
    }

    // 2. Perform custom custom instrumentation/logging
    // Note: Do not use printf here! Printf calls malloc internally, causing infinite recursion.
    write(1, "Intercepted malloc request!\n", 28);

    // 3. Forward the call to the authentic malloc function
    return real_malloc(size);
}
