#include <new>
#include <cstdlib>
#include <cstdio>

// Technique-1
// A thread-local flag to detect reentrancy
static thread_local bool inside_tracker = false;

void* operator new(std::size_t size) {
    // 1. Fast-path check: If this thread is already tracking, bypass the instrumentation
    if (inside_tracker) {
        void* ptr = std::malloc(size);
        if (!ptr) throw std::bad_alloc();
        return ptr;
    }

    // 2. Set the guard flag
    inside_tracker = true;

    // 3. Execute your tracking payload (Safe to use complex allocations now)
    // E.g., std::printf or updating an internal std::unordered_map
    std::printf("Intercepted allocation of %zu bytes\n", size);

    // 4. Perform the actual allocation
    void* ptr = std::malloc(size);

    // 5. Clear the guard flag before returning
    inside_tracker = false;

    if (!ptr) throw std::bad_alloc();
    return ptr;
}

// Technique - 2
struct TrackerGuard {
    TrackerGuard() { inside_tracker = true; }
    ~TrackerGuard() { inside_tracker = false; }
    
    // Prevent copying/moving to keep the guard strictly local
    TrackerGuard(const TrackerGuard&) = delete;
    TrackerGuard& operator=(const TrackerGuard&) = delete;
};

void* operator new(std::size_t size) {
    if (inside_tracker) {
        void* ptr = std::malloc(size);
        if (!ptr) throw std::bad_alloc();
        return ptr;
    }

    {
        // Instantiating the guard sets the thread-local flag to true
        TrackerGuard guard;

        // Complex operations like formatting strings or locking mutextes go here
        std::printf("Allocated: %zu\n", size);
    } 
    // The guard automatically goes out of scope here and resets the flag to false

    void* ptr = std::malloc(size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

// Technique - 3

#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>

#define BOOTSTRAP_BUFFER_SIZE 4096
static char bootstrap_buffer[BOOTSTRAP_BUFFER_SIZE];
static size_t buffer_index = 0;

static void* (*real_malloc)(size_t) = NULL;
static int initializing = 0;

void* malloc(size_t size) {
    // 1. If real_malloc is ready, use it directly
    if (real_malloc) {
        return real_malloc(size);
    }

    // 2. If we are currently inside dlsym initializing real_malloc, use the bootstrap pool
    if (initializing) {
        if (buffer_index + size > BOOTSTRAP_BUFFER_SIZE) {
            write(2, "Bootstrap buffer exhausted!\n", 28);
            return NULL;
        }
        void* ptr = &bootstrap_buffer[buffer_index];
        buffer_index += size; // Hand out static memory slice
        return ptr;
    }

    // 3. First-time initialization trigger
    initializing = 1;
    real_malloc = dlsym(RTLD_NEXT, "malloc");
    initializing = 0;

    return real_malloc(size);
}
