#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ORDERS 16

typedef struct BlockHeader {
    struct BlockHeader *next;
    struct BlockHeader *prev;
    uint8_t order;
    bool is_free;
} BlockHeader;

typedef struct {
    uint8_t *memory_pool;
    size_t pool_size;
    size_t min_block_size;
    uint8_t min_order;
    uint8_t max_order;
    BlockHeader *free_lists[MAX_ORDERS];
} BuddyAllocator;

static inline uint8_t log2_ceil(size_t size) {
    uint8_t order = 0;
    size_t val = 1;
    while (val < size) {
        val <<= 1;
        order++;
    }
    return order;
}

static void list_add(BlockHeader **head, BlockHeader *node) {
    node->next = *head;
    node->prev = NULL;
    if (*head) {
        (*head)->prev = node;
    }
    *head = node;
}

static void list_remove(BlockHeader **head, BlockHeader *node) {
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        *head = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }
}

void buddy_init(BuddyAllocator *alloc, void *pool, size_t pool_size, size_t min_block_size) {
    alloc->memory_pool = (uint8_t *)pool;
    alloc->pool_size = pool_size;
    alloc->min_block_size = min_block_size;

    alloc->min_order = log2_ceil(min_block_size);
    alloc->max_order = log2_ceil(pool_size);

    /* bounds check: array must fit num orders, else silent OOB write */
    assert((size_t)(alloc->max_order - alloc->min_order + 1) <= MAX_ORDERS);

    for (int i = 0; i < MAX_ORDERS; i++) {
        alloc->free_lists[i] = NULL;
    }

    uint8_t initial_idx = alloc->max_order - alloc->min_order;
    BlockHeader *root = (BlockHeader *)alloc->memory_pool;
    root->order = alloc->max_order;
    root->is_free = true;
    root->next = NULL;
    root->prev = NULL;

    alloc->free_lists[initial_idx] = root;
}

void *buddy_alloc(BuddyAllocator *alloc, size_t size) {
    size_t total_size = size + sizeof(BlockHeader);
    uint8_t req_order = log2_ceil(total_size);
    if (req_order < alloc->min_order) {
        req_order = alloc->min_order;
    }

    if (req_order > alloc->max_order) {
        return NULL;
    }

    uint8_t target_idx = req_order - alloc->min_order;
    uint8_t current_idx = target_idx;

    while (current_idx < (alloc->max_order - alloc->min_order + 1) && alloc->free_lists[current_idx] == NULL) {
        current_idx++;
    }

    if (current_idx > (alloc->max_order - alloc->min_order)) {
        return NULL;
    }

    BlockHeader *block = alloc->free_lists[current_idx];
    list_remove(&alloc->free_lists[current_idx], block);

    while (current_idx > target_idx) {
        current_idx--;
        block->order--;

        size_t half_size = (size_t)1 << block->order;
        BlockHeader *buddy = (BlockHeader *)((uint8_t *)block + half_size);
        buddy->order = block->order;
        buddy->is_free = true;

        list_add(&alloc->free_lists[current_idx], buddy);
    }

    block->is_free = false;
    return (void *)(block + 1);
}

void buddy_free(BuddyAllocator *alloc, void *ptr) {
    if (!ptr) return;

    BlockHeader *block = (BlockHeader *)ptr - 1;
    block->is_free = true;

    uint8_t current_idx = block->order - alloc->min_order;

    while (block->order < alloc->max_order) {
        uintptr_t offset = (uintptr_t)block - (uintptr_t)alloc->memory_pool;
        size_t block_size = (size_t)1 << block->order;
        uintptr_t buddy_offset = offset ^ block_size;

        BlockHeader *buddy = (BlockHeader *)(alloc->memory_pool + buddy_offset);

        if (!buddy->is_free || buddy->order != block->order) {
            break;
        }

        list_remove(&alloc->free_lists[current_idx], buddy);

        if ((uintptr_t)buddy < (uintptr_t)block) {
            block = buddy;
        }

        block->order++;
        current_idx++;
    }

    list_add(&alloc->free_lists[current_idx], block);
}

/* ---- stress test: random alloc/free, check no overlap, check full coalesce ---- */
#define NUM_PTRS 64

static bool ranges_overlap(uint8_t *a_start, size_t a_len, uint8_t *b_start, size_t b_len) {
    uint8_t *a_end = a_start + a_len;
    uint8_t *b_end = b_start + b_len;
    return (a_start < b_end) && (b_start < a_end);
}

static void stress_test(void) {
    static uint8_t pool[1 << 16] __attribute__((aligned(8)));
    BuddyAllocator alloc;
    buddy_init(&alloc, pool, sizeof(pool), 32);

    void *ptrs[NUM_PTRS] = {0};
    size_t sizes[NUM_PTRS] = {0};

    srand(1234);

    for (int round = 0; round < 5000; round++) {
        int i = rand() % NUM_PTRS;
        if (ptrs[i]) {
            buddy_free(&alloc, ptrs[i]);
            ptrs[i] = NULL;
            sizes[i] = 0;
        } else {
            size_t sz = (rand() % 256) + 1;
            void *p = buddy_alloc(&alloc, sz);
            if (p) {
                /* overlap check against all live allocations */
                for (int j = 0; j < NUM_PTRS; j++) {
                    if (j != i && ptrs[j]) {
                        assert(!ranges_overlap((uint8_t *)p, sz, (uint8_t *)ptrs[j], sizes[j]));
                    }
                }
                memset(p, 0xAB, sz); /* touch memory, catch OOB via asan */
                ptrs[i] = p;
                sizes[i] = sz;
            }
        }
    }

    for (int i = 0; i < NUM_PTRS; i++) {
        if (ptrs[i]) buddy_free(&alloc, ptrs[i]);
    }

    /* proof of full coalesce: single root block in top free-list */
    uint8_t top_idx = alloc.max_order - alloc.min_order;
    assert(alloc.free_lists[top_idx] != NULL);
    assert(alloc.free_lists[top_idx]->next == NULL);
    for (int i = 0; i < top_idx; i++) {
        assert(alloc.free_lists[i] == NULL);
    }

    printf("Stress test passed: no overlaps, full coalesce verified.\n");
}

int main() {
    uint8_t pool[1024] __attribute__((aligned(8)));
    BuddyAllocator alloc;

    buddy_init(&alloc, pool, 1024, 32);

    void *p1 = buddy_alloc(&alloc, 100);
    void *p2 = buddy_alloc(&alloc, 200);

    printf("Allocated p1: %p, p2: %p\n", p1, p2);

    buddy_free(&alloc, p1);
    buddy_free(&alloc, p2);

    printf("Freed successfully.\n");

    assert(alloc.free_lists[alloc.max_order - alloc.min_order] != NULL);

    stress_test();

    return 0;
}