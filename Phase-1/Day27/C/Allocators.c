#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdalign.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

// --- Generic VTable Allocator Interface ---
typedef struct {
    void* (*allocate)(void* ctx, size_t size, size_t align);
    void (*deallocate)(void* ctx, void* ptr, size_t size);
    void* ctx;
} Allocator;

// --- Stats Structure ---
typedef struct {
    size_t alloc_calls;
    size_t bytes_allocated;
} AllocStats;

// --- Backend 1: Arena Allocator ---
typedef struct {
    uint8_t* buffer;
    size_t capacity;
    size_t offset;
    AllocStats stats;
} ArenaBackend;

ArenaBackend* arena_create(size_t capacity) {
    ArenaBackend* arena = (ArenaBackend*)malloc(sizeof(ArenaBackend));
    if (!arena) return NULL;
    arena->capacity = capacity;
    arena->offset = 0;
    arena->stats.alloc_calls = 0;
    arena->stats.bytes_allocated = 0;

#ifdef _WIN32
    arena->buffer = (uint8_t*)VirtualAlloc(NULL, capacity, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    void* ptr = mmap(NULL, capacity, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    arena->buffer = (ptr == MAP_FAILED) ? NULL : (uint8_t*)ptr;
#endif

    if (!arena->buffer) {
        free(arena);
        return NULL;
    }
    return arena;
}

void arena_destroy(ArenaBackend* arena) {
    if (!arena) return;
    if (arena->buffer) {
#ifdef _WIN32
        VirtualFree(arena->buffer, 0, MEM_RELEASE);
#else
        munmap(arena->buffer, arena->capacity);
#endif
    }
    free(arena);
}

void* arena_alloc(void* ctx, size_t size, size_t align) {
    ArenaBackend* arena = (ArenaBackend*)ctx;
    if (!arena || align == 0 || (align & (align - 1)) != 0) return NULL;

    uintptr_t current = (uintptr_t)(arena->buffer + arena->offset);
    uintptr_t aligned = (current + align - 1) & ~(align - 1);
    uintptr_t padding = aligned - current;

    if (arena->offset + padding + size > arena->capacity) return NULL;

    arena->offset += (padding + size);
    arena->stats.alloc_calls++;
    arena->stats.bytes_allocated += size;
    return (void*)aligned;
}

void arena_free(void* ctx, void* ptr, size_t size) {
    (void)ctx;
    (void)ptr;
    (void)size;
    // Bump allocator: no-op, memory leaked until arena reset/destroy
}

// --- Backend 2: Malloc/Free Allocator ---
typedef struct {
    AllocStats stats;
} MallocBackend;

void* malloc_alloc(void* ctx, size_t size, size_t align) {
    MallocBackend* mb = (MallocBackend*)ctx;
    if (align < sizeof(void*)) align = sizeof(void*);

    void* ptr = NULL;
#ifdef _WIN32
    ptr = _aligned_malloc(size, align);
#else
    if (posix_memalign(&ptr, align, size) != 0) ptr = NULL;
#endif

    if (ptr && mb) {
        mb->stats.alloc_calls++;
        mb->stats.bytes_allocated += size;
    }
    return ptr;
}

void malloc_free(void* ctx, void* ptr, size_t size) {
    (void)ctx;
    (void)size;
    if (!ptr) return;
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

// --- Custom Vector ---
typedef struct {
    int* data;
    size_t size;
    size_t capacity;
    Allocator alloc;
} CustomVector;

CustomVector vec_create(Allocator alloc) {
    CustomVector vec;
    vec.data = NULL;
    vec.size = 0;
    vec.capacity = 0;
    vec.alloc = alloc;
    return vec;
}

bool vec_push_back(CustomVector* vec, int val) {
    if (vec->size >= vec->capacity) {
        size_t new_cap = (vec->capacity == 0) ? 4 : vec->capacity * 2;
        size_t new_bytes = new_cap * sizeof(int);
        size_t old_bytes = vec->capacity * sizeof(int);

        int* new_data = (int*)vec->alloc.allocate(vec->alloc.ctx, new_bytes, _Alignof(int));
        if (!new_data) return false; // Graceful failure, vector state preserved

        for (size_t i = 0; i < vec->size; ++i) {
            new_data[i] = vec->data[i];
        }

        if (vec->data) {
            vec->alloc.deallocate(vec->alloc.ctx, vec->data, old_bytes);
        }

        vec->data = new_data;
        vec->capacity = new_cap;
    }
    vec->data[vec->size++] = val;
    return true;
}

void vec_destroy(CustomVector* vec) {
    if (vec->data) {
        vec->alloc.deallocate(vec->alloc.ctx, vec->data, vec->capacity * sizeof(int));
        vec->data = NULL;
    }
    vec->size = 0;
    vec->capacity = 0;
}

void section(const char* name) {
    printf("\n=== %s ===\n", name);
}

int main() {
    // 1. SWAP DEMO & WASTE MEASUREMENT (Arena Backend)
    section("TEST 1: Arena Backend + Waste Measurement");
    ArenaBackend* arena = arena_create(1024 * 1024);
    Allocator arena_vtable = { .allocate = arena_alloc, .deallocate = arena_free, .ctx = arena };

    CustomVector vec_arena = vec_create(arena_vtable);
    for (int i = 0; i < 100; ++i) vec_push_back(&vec_arena, i * 10);

    size_t live_bytes = vec_arena.capacity * sizeof(int);
    printf("Vector size=%zu, cap=%zu\n", vec_arena.size, vec_arena.capacity);
    printf("Alloc calls=%zu, Total requested=%zu bytes\n", arena->stats.alloc_calls, arena->stats.bytes_allocated);
    printf("Arena offset (used)=%zu bytes, Live vector memory=%zu bytes\n", arena->offset, live_bytes);
    printf("Wasted/Abandoned Arena Memory=%zu bytes\n", arena->offset - live_bytes);
    vec_destroy(&vec_arena);

    // 2. RUNTIME SWAP (Malloc Backend, Identical Interface)
    section("TEST 2: Malloc Backend (Runtime Swap)");
    MallocBackend malloc_b = { .stats = {0, 0} };
    Allocator malloc_vtable = { .allocate = malloc_alloc, .deallocate = malloc_free, .ctx = &malloc_b };

    CustomVector vec_malloc = vec_create(malloc_vtable);
    for (int i = 0; i < 100; ++i) vec_push_back(&vec_malloc, i * 10);

    printf("Vector size=%zu, cap=%zu\n", vec_malloc.size, vec_malloc.capacity);
    printf("Alloc calls=%zu, Total allocated bytes=%zu\n", malloc_b.stats.alloc_calls, malloc_b.stats.bytes_allocated);
    if (vec_malloc.size > 0) {
        printf("First item=%d, Last item=%d\n", vec_malloc.data[0], vec_malloc.data[vec_malloc.size - 1]);
    }
    vec_destroy(&vec_malloc);

    // 3. OVERFLOW & SMALL ARENA FAIL SAFE
    section("TEST 3: Forced Overflow (Tiny 500-byte Arena)");
    ArenaBackend* tiny_arena = arena_create(500); // Tiny arena
    Allocator tiny_vtable = { .allocate = arena_alloc, .deallocate = arena_free, .ctx = tiny_arena };

    CustomVector vec_tiny = vec_create(tiny_vtable);
    int pushed = 0;
    for (int i = 0; i < 1000; ++i) {
        if (!vec_push_back(&vec_tiny, i)) {
            printf("ALLOCATION FAILED at i=%d\n", i);
            break;
        }
        pushed++;
    }

    printf("Pushed items before fail=%d\n", pushed);
    printf("Vector size=%zu, cap=%zu (Uncorrupted state)\n", vec_tiny.size, vec_tiny.capacity);
    printf("Arena used=%zu / %zu bytes\n", tiny_arena->offset, tiny_arena->capacity);

    // Safe access check
    if (vec_tiny.size > 0) {
        printf("First item=%d, Last valid item=%d\n", vec_tiny.data[0], vec_tiny.data[vec_tiny.size - 1]);
    } else {
        printf("Vector empty, skipping read.\n");
    }

    vec_destroy(&vec_tiny);
    arena_destroy(tiny_arena);
    arena_destroy(arena);

    return 0;
}