#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

#define KB(x) ((uint64_t)(x) * 1024)

typedef struct 
{
    uint8_t *buffer;
    uint64_t currentOffset, bufferSize;
} Arena;

void ArenaInit(Arena *arena, uint64_t bufferSize)
{
    void *ptr = NULL;
    #ifdef _WIN32
        ptr = VirtualAlloc(NULL, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!ptr) return;
    #else
        ptr = mmap(NULL, bufferSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) return;
    #endif
    *arena = (Arena) {
        .buffer = ptr,
        .bufferSize = bufferSize,
        .currentOffset = 0
    };
}

void *ArenaAlloc(Arena *arena, uint64_t numOfElem, uint64_t elemSize, uint64_t alignSize)
{
    if(alignSize == 0 || (alignSize & (alignSize - 1)) != 0) return NULL; // Pwer of 2 
    uintptr_t allocationSize = numOfElem * elemSize;
    if(allocationSize < elemSize) return NULL;

    uintptr_t totlaOffset = (uintptr_t)arena->currentOffset + (uintptr_t)arena->buffer;
    // uintptr_t padding = totlaOffset % alignSize;
    uintptr_t padding = (~totlaOffset) & (alignSize - 1);
    totlaOffset += padding;

    if(totlaOffset + allocationSize > (uintptr_t)arena->bufferSize + (uintptr_t)arena->buffer) return NULL;

    arena->currentOffset += (padding + allocationSize);
    memset((void*)totlaOffset, 0, allocationSize);
    return (void*)totlaOffset;
}

void ArenaResetPointer(Arena *arena)
{
    arena->currentOffset = 0;
    // memset(arena->buffer, 0, arena->bufferSize);
}

void ArenaDelete(Arena *arena)
{
    if (arena->buffer) {
#ifdef _WIN32
        VirtualFree(arena->buffer, 0, MEM_RELEASE);
#else
        munmap(arena->buffer, arena->bufferSize);
#endif
    }
    arena->buffer = NULL;
    arena->currentOffset = 0;
    arena->bufferSize = 0;
}

int main()
{
    Arena arena = {};
    ArenaInit(&arena, KB(10));

    int *a = ArenaAlloc(&arena, 1, sizeof(int), _Alignof(int));
    if (!a) { fprintf(stderr, "alloc failed\n"); return 1; }
    *a = 20;
    printf("%d\n", *a);

    ArenaDelete(&arena);
        
    return 0;
}