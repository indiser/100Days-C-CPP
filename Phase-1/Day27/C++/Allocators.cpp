#include <iostream>
#include <memory>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <list>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

class Arena {
private:
    uint8_t* buffer_{nullptr};
    std::size_t capacity_{0};
    std::size_t offset_{0};

public:
    explicit Arena(std::size_t bytes) : capacity_(bytes) {
#ifdef _WIN32
        buffer_ = static_cast<uint8_t*>(VirtualAlloc(nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
#else
        void* ptr = mmap(nullptr, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        buffer_ = (ptr == MAP_FAILED) ? nullptr : static_cast<uint8_t*>(ptr);
#endif
        if (!buffer_) throw std::bad_alloc();
    }

    ~Arena() noexcept {
        if (buffer_) {
#ifdef _WIN32
            VirtualFree(buffer_, 0, MEM_RELEASE);
#else
            munmap(buffer_, capacity_);
#endif
        }
    }

    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    void* allocate(std::size_t bytes, std::size_t align = alignof(std::max_align_t)) {
        if (align == 0 || (align & (align - 1)) != 0) return nullptr;

        uintptr_t current = reinterpret_cast<uintptr_t>(buffer_ + offset_);
        uintptr_t aligned = (current + align - 1) & ~(align - 1);
        uintptr_t padding = aligned - current;

        if (offset_ + padding + bytes > capacity_) return nullptr;

        offset_ += (padding + bytes);
        return reinterpret_cast<void*>(aligned);
    }

    void reset() noexcept { offset_ = 0; }
    [[nodiscard]] std::size_t used() const noexcept { return offset_; }
    [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; }
};

struct AllocStats {
    std::size_t alloc_calls{0};
    std::size_t bytes_allocated{0};
};


template <typename T>
class ArenaAllocator {
public:
    using value_type = T;

    // propagate flags — explicit, not left to default
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_copy_assignment  = std::false_type;
    using propagate_on_container_swap             = std::true_type;

    Arena* arena_{nullptr};
    AllocStats* stats_{nullptr};

    ArenaAllocator() noexcept = default;
    ArenaAllocator(Arena& arena, AllocStats& stats) noexcept
        : arena_(&arena), stats_(&stats) {}

    template <typename U>
    constexpr ArenaAllocator(const ArenaAllocator<U>& other) noexcept
        : arena_(other.arena_),
          stats_(other.stats_) {}

    T* allocate(std::size_t n) {
        if (!arena_) throw std::bad_alloc();
        std::size_t bytes = n * sizeof(T);
        void* ptr = arena_->allocate(bytes, alignof(T));
        if (!ptr) throw std::bad_alloc();
        if (stats_) { ++stats_->alloc_calls; stats_->bytes_allocated += bytes; }
        return static_cast<T*>(ptr);
    }

    void deallocate(T* , std::size_t n) noexcept {
        std::cout << "  [deallocate] n=" << n << " sizeof(T)=" << sizeof(T)
                   << " (no-op, arena owns memory)\n";
    }

    template <typename U>
    bool operator==(const ArenaAllocator<U>& other) const noexcept {
        return arena_ == other.arena_;
    }
    template <typename U>
    bool operator!=(const ArenaAllocator<U>& other) const noexcept {
        return arena_ != other.arena_;
    }
};

void section(const char* name) {
    std::cout << "\n=== " << name << " ===\n";
}

int main() {
    // ---- TEST 1: basic vector, prove stats ----
    section("TEST 1: vector<int> basic + stats");
    Arena arena1(1024 * 1024);
    AllocStats stats1{};
    ArenaAllocator<int> alloc1(arena1, stats1);
    std::vector<int, ArenaAllocator<int>> vec(alloc1);
    for (int i = 0; i < 100; ++i) vec.push_back(i);
    std::cout << "size=" << vec.size()
              << " arena_used=" << arena1.used()
              << " alloc_calls=" << vec.get_allocator().stats_->alloc_calls
              << " bytes_allocated=" << vec.get_allocator().stats_->bytes_allocated << "\n";

    // ---- TEST 2: force overflow on purpose ----
    section("TEST 2: forced overflow, small arena");
    Arena tinyArena(200); // tiny on purpose
    AllocStats tinyStats{};
    ArenaAllocator<int> tinyAlloc(tinyArena, tinyStats);
    std::vector<int, ArenaAllocator<int>> tinyVec(tinyAlloc);
    try {
        for (int i = 0; i < 1000; ++i) tinyVec.push_back(i);
    } catch (const std::bad_alloc& e) {
        std::cout << "Caught bad_alloc at size=" << tinyVec.size()
                  << ", arena used=" << tinyArena.used()
                  << "/" << tinyArena.capacity() << "\n";
    }

    // ---- TEST 3: move-assignment across two different arenas ----
    section("TEST 3: move-assign, propagate_on_container_move_assignment=true");
    Arena arenaA(1024);
    Arena arenaB(1024);
    AllocStats statsA{}, statsB{};
    std::vector<int, ArenaAllocator<int>> vecA(ArenaAllocator<int>{arenaA, statsA});
    std::vector<int, ArenaAllocator<int>> vecB(ArenaAllocator<int>{arenaB, statsB});
    vecA.push_back(1);
    vecA.push_back(2);
    std::cout << "before move: vecA.allocator arena=" << vecA.get_allocator().arena_
              << " vecB.allocator arena=" << vecB.get_allocator().arena_ << "\n";
    vecB = std::move(vecA); // with propagate=true, allocator pointer steals over
    std::cout << "after move: vecB.allocator arena=" << vecB.get_allocator().arena_
              << " (should now equal arenaA=" << &arenaA << ")\n";

    // ---- TEST 4: rebind proof via std::list ----
    section("TEST 4: std::list forces rebind, different sizeof(T)");
    Arena arenaL(1024 * 1024);
    AllocStats statsL{};
    ArenaAllocator<int> allocL(arenaL, statsL);
    std::list<int, ArenaAllocator<int>> lst(allocL);
    for (int i = 0; i < 10; ++i) lst.push_back(i);
    std::cout << "list size=" << lst.size()
              << " arena used=" << arenaL.used()
              << " alloc_calls=" << statsL.alloc_calls
              << " bytes_allocated=" << statsL.bytes_allocated << "\n";

    return 0;
}