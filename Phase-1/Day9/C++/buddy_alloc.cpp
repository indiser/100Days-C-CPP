#include <iostream>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstring>

class BuddyAllocatorRaw {
private:
    static constexpr size_t MAX_ORDERS = 16;

    struct BlockHeader {
        BlockHeader* next;
        BlockHeader* prev;
        uint8_t order;
        bool is_free;
    };

    uint8_t* memory_pool;
    size_t pool_size;
    size_t min_block_size;
    uint8_t min_order;
    uint8_t max_order;
    BlockHeader* free_lists[MAX_ORDERS]{};

    static uint8_t log2_ceil(size_t size) {
        uint8_t order = 0;
        size_t val = 1;
        while (val < size) {
            val <<= 1;
            order++;
        }
        return order;
    }

    void list_add(BlockHeader** head, BlockHeader* node) {
        node->next = *head;
        node->prev = nullptr;
        if (*head) {
            (*head)->prev = node;
        }
        *head = node;
    }

    void list_remove(BlockHeader** head, BlockHeader* node) {
        if (node->prev) {
            node->prev->next = node->next;
        } else {
            *head = node->next;
        }
        if (node->next) {
            node->next->prev = node->prev;
        }
    }

public:
    BuddyAllocatorRaw(void* pool, size_t pool_size, size_t min_block_size)
        : memory_pool(static_cast<uint8_t*>(pool)),
          pool_size(pool_size),
          min_block_size(min_block_size) {
        min_order = log2_ceil(min_block_size);
        max_order = log2_ceil(pool_size);

        /* bounds check: array must fit num orders, else silent OOB write */
        assert(static_cast<size_t>(max_order - min_order + 1) <= MAX_ORDERS);

        uint8_t initial_idx = max_order - min_order;
        auto* root = reinterpret_cast<BlockHeader*>(memory_pool);
        root->order = max_order;
        root->is_free = true;
        root->next = nullptr;
        root->prev = nullptr;

        free_lists[initial_idx] = root;
    }

    void* allocate(size_t size) {
        size_t total_size = size + sizeof(BlockHeader);
        uint8_t req_order = log2_ceil(total_size);
        if (req_order < min_order) req_order = min_order;
        if (req_order > max_order) return nullptr;

        uint8_t target_idx = req_order - min_order;
        uint8_t current_idx = target_idx;

        while (current_idx <= (max_order - min_order) && free_lists[current_idx] == nullptr) {
            current_idx++;
        }

        if (current_idx > (max_order - min_order)) return nullptr;

        BlockHeader* block = free_lists[current_idx];
        list_remove(&free_lists[current_idx], block);

        while (current_idx > target_idx) {
            current_idx--;
            block->order--;

            size_t half_size = static_cast<size_t>(1) << block->order;
            auto* buddy = reinterpret_cast<BlockHeader*>(reinterpret_cast<uint8_t*>(block) + half_size);
            buddy->order = block->order;
            buddy->is_free = true;

            list_add(&free_lists[current_idx], buddy);
        }

        block->is_free = false;
        return static_cast<void*>(block + 1);
    }

    // size param unused: block's own order (in header) already tells us how
    // big it was, so we don't need caller-supplied size to free correctly.
    void deallocate(void* ptr) {
        if (!ptr) return;

        auto* block = reinterpret_cast<BlockHeader*>(ptr) - 1;
        block->is_free = true;

        uint8_t current_idx = block->order - min_order;

        while (block->order < max_order) {
            uintptr_t offset = reinterpret_cast<uintptr_t>(block) - reinterpret_cast<uintptr_t>(memory_pool);
            size_t block_size = static_cast<size_t>(1) << block->order;
            uintptr_t buddy_offset = offset ^ block_size;

            auto* buddy = reinterpret_cast<BlockHeader*>(memory_pool + buddy_offset);

            if (!buddy->is_free || buddy->order != block->order) break;

            list_remove(&free_lists[current_idx], buddy);

            if (reinterpret_cast<uintptr_t>(buddy) < reinterpret_cast<uintptr_t>(block)) {
                block = buddy;
            }

            block->order++;
            current_idx++;
        }

        list_add(&free_lists[current_idx], block);
    }

    // exposed for stress test proof only
    uint8_t top_index() const { return max_order - min_order; }
    BlockHeader* free_list_at(uint8_t idx) const { return free_lists[idx]; }
};

template <typename T>
class BuddyAllocator {
private:
    BuddyAllocatorRaw* raw_alloc;

public:
    using value_type = T;

    BuddyAllocator(BuddyAllocatorRaw* raw) noexcept : raw_alloc(raw) {}

    template <typename U>
    BuddyAllocator(const BuddyAllocator<U>& other) noexcept : raw_alloc(other.raw_alloc) {}

    T* allocate(std::size_t n) {
        if (auto* p = static_cast<T*>(raw_alloc->allocate(n * sizeof(T)))) return p;
        throw std::bad_alloc();
    }

    void deallocate(T* p, std::size_t) noexcept {
        raw_alloc->deallocate(p);
    }

    template <typename U>
    bool operator==(const BuddyAllocator<U>& other) const noexcept {
        return raw_alloc == other.raw_alloc;
    }

    template <typename U>
    bool operator!=(const BuddyAllocator<U>& other) const noexcept {
        return !(*this == other);
    }

    template <typename U> friend class BuddyAllocator;
};

// ---- stress test: random alloc/free, check no overlap, check full coalesce ----
static bool ranges_overlap(uint8_t* a_start, size_t a_len, uint8_t* b_start, size_t b_len) {
    uint8_t* a_end = a_start + a_len;
    uint8_t* b_end = b_start + b_len;
    return (a_start < b_end) && (b_start < a_end);
}

static void stress_test() {
    static uint8_t pool[1 << 16] alignas(8);
    BuddyAllocatorRaw raw_alloc(pool, sizeof(pool), 32);

    constexpr int NUM_PTRS = 64;
    void* ptrs[NUM_PTRS] = {};
    size_t sizes[NUM_PTRS] = {};

    std::srand(1234);

    for (int round = 0; round < 5000; round++) {
        int i = std::rand() % NUM_PTRS;
        if (ptrs[i]) {
            raw_alloc.deallocate(ptrs[i]);
            ptrs[i] = nullptr;
            sizes[i] = 0;
        } else {
            size_t sz = (std::rand() % 256) + 1;
            void* p = raw_alloc.allocate(sz);
            if (p) {
                for (int j = 0; j < NUM_PTRS; j++) {
                    if (j != i && ptrs[j]) {
                        assert(!ranges_overlap(static_cast<uint8_t*>(p), sz,
                                                static_cast<uint8_t*>(ptrs[j]), sizes[j]));
                    }
                }
                std::memset(p, 0xAB, sz);
                ptrs[i] = p;
                sizes[i] = sz;
            }
        }
    }

    for (int i = 0; i < NUM_PTRS; i++) {
        if (ptrs[i]) raw_alloc.deallocate(ptrs[i]);
    }

    uint8_t top_idx = raw_alloc.top_index();
    assert(raw_alloc.free_list_at(top_idx) != nullptr);
    for (uint8_t i = 0; i < top_idx; i++) {
        assert(raw_alloc.free_list_at(i) == nullptr);
    }

    std::cout << "Stress test passed: no overlaps, full coalesce verified.\n";
}

int main() {
    alignas(8) uint8_t pool[1024];
    BuddyAllocatorRaw raw_alloc(pool, 1024, 32);

    BuddyAllocator<int> alloc(&raw_alloc);
    std::vector<int, BuddyAllocator<int>> vec(alloc);

    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    for (int val : vec) {
        std::cout << val << " ";
    }
    std::cout << "\nVec size: " << vec.size() << "\n";

    stress_test();

    return 0;
}