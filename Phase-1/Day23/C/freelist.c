#include <stdio.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdbool.h>

#define POOL_SIZE 1024

typedef struct FreeBlock {
    size_t size;
    struct FreeBlock *next;
} FreeBlock;

static alignas(max_align_t) unsigned char memory_pool[POOL_SIZE];

static FreeBlock *free_list;

/*
 * Round size upward to alignment.
 *
 * Example:
 *
 * size = 1
 * alignment = 16
 *
 * result = 16
 */
static size_t align_up(size_t size)
{
    size_t alignment = alignof(max_align_t);

    return (size + alignment - 1) &
           ~(alignment - 1);
}

/*
 * Check whether pointer lies inside memory pool.
 */
static bool ptr_in_pool(void *ptr)
{
    uintptr_t start = (uintptr_t)memory_pool;
    uintptr_t end = start + POOL_SIZE;
    uintptr_t p = (uintptr_t)ptr;

    return p >= start && p < end;
}

/*
 * Initialize allocator.
 */
void free_list_init(void)
{
    free_list = (FreeBlock *)memory_pool;

    free_list->size = POOL_SIZE - sizeof(FreeBlock);
    free_list->next = NULL;
}

/*
 * Allocate memory.
 */
void *free_list_alloc(size_t requested_size)
{
    if (requested_size == 0)
        return NULL;

    size_t size = align_up(requested_size);

    FreeBlock *current = free_list;
    FreeBlock *previous = NULL;

    while (current != NULL) {

        if (current->size >= size) {

            size_t remaining =
                current->size - size;

            /*
             * Enough room to split block.
             */
            if (remaining >= sizeof(FreeBlock) +
                             alignof(max_align_t)) {

                FreeBlock *new_block =
                    (FreeBlock *)(
                        (unsigned char *)current +
                        sizeof(FreeBlock) +
                        size
                    );

                new_block->size =
                    remaining - sizeof(FreeBlock);

                new_block->next =
                    current->next;

                if (previous != NULL)
                    previous->next = new_block;
                else
                    free_list = new_block;

                current->size = size;
            }
            else {

                /*
                 * Consume entire block.
                 */
                if (previous != NULL)
                    previous->next = current->next;
                else
                    free_list = current->next;
            }

            return (unsigned char *)current +
                   sizeof(FreeBlock);
        }

        previous = current;
        current = current->next;
    }

    return NULL;
}

/*
 * Insert block into free list in address order.
 *
 * Address ordering makes coalescing easy.
 */
static void insert_free_block(FreeBlock *block)
{
    FreeBlock *current = free_list;
    FreeBlock *previous = NULL;

    while (current != NULL &&
           current < block) {

        previous = current;
        current = current->next;
    }

    block->next = current;

    if (previous != NULL)
        previous->next = block;
    else
        free_list = block;
}

/*
 * Merge adjacent blocks.
 */
static void coalesce(void)
{
    FreeBlock *current = free_list;

    while (current != NULL &&
           current->next != NULL) {

        unsigned char *current_end =
            (unsigned char *)current +
            sizeof(FreeBlock) +
            current->size;

        if (current_end ==
            (unsigned char *)current->next) {

            /*
             * Blocks physically touch.
             *
             * Merge:
             *
             * [current][next]
             *
             * into:
             *
             * [current................]
             */
            current->size +=
                sizeof(FreeBlock) +
                current->next->size;

            current->next =
                current->next->next;
        }
        else {
            current = current->next;
        }
    }
}

/*
 * Check whether block already exists in free list.
 *
 * Used for double-free detection.
 */
static bool is_already_free(FreeBlock *block)
{
    FreeBlock *current = free_list;

    while (current != NULL) {

        if (current == block)
            return true;

        current = current->next;
    }

    return false;
}

/*
 * Free memory.
 */
bool free_list_free(void *ptr)
{
    if (ptr == NULL)
        return true;

    /*
     * Pointer must point inside pool.
     */
    if (!ptr_in_pool(ptr)) {
        fprintf(stderr,
                "free_list_free: pointer outside pool\n");
        return false;
    }

    /*
     * Payload must be after a FreeBlock header.
     */
    uintptr_t pool_start =
        (uintptr_t)memory_pool;

    uintptr_t p =
        (uintptr_t)ptr;

    if (p < pool_start + sizeof(FreeBlock)) {
        fprintf(stderr,
                "free_list_free: invalid pointer\n");
        return false;
    }

    FreeBlock *block =
        (FreeBlock *)(
            (unsigned char *)ptr -
            sizeof(FreeBlock)
        );

    /*
     * Header itself must lie inside pool.
     */
    if (!ptr_in_pool(block)) {
        fprintf(stderr,
                "free_list_free: invalid block\n");
        return false;
    }

    /*
     * Detect double free.
     */
    if (is_already_free(block)) {
        fprintf(stderr,
                "free_list_free: double free\n");
        return false;
    }

    insert_free_block(block);

    coalesce();

    return true;
}

/*
 * Print free list.
 */
void free_list_dump(void)
{
    FreeBlock *current = free_list;

    printf("Free list:\n");

    while (current != NULL) {

        printf("  address=%p size=%zu\n",
               (void *)current,
               current->size);

        current = current->next;
    }
}

int main(void)
{
    free_list_init();

    printf("Initial:\n");
    free_list_dump();

    int *a = free_list_alloc(sizeof(int));
    int *b = free_list_alloc(sizeof(int));
    char *buffer = free_list_alloc(101);

    if (a == NULL ||
        b == NULL ||
        buffer == NULL) {

        printf("Allocation failed\n");
        return 1;
    }

    *a = 42;
    *b = 100;

    printf("\na = %d\n", *a);
    printf("b = %d\n", *b);

    printf("\nAfter allocations:\n");
    free_list_dump();

    free_list_free(a);
    free_list_free(b);
    free_list_free(buffer);

    printf("\nAfter freeing + coalescing:\n");
    free_list_dump();

    /*
     * Double-free test.
     */
    printf("\nDouble-free test:\n");

    if (!free_list_free(a))
        printf("Double free rejected\n");

    return 0;
}