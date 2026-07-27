#include <iostream>
#include <memory>
#include <cstdint>
#include <cstddef>
#include <vector>
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

    void reset() noexcept {
        offset_ = 0;
    }

    [[nodiscard]] std::size_t used() const noexcept { return offset_; }
    [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; }
};

template <typename T>
class ArenaAllocator {
public:
    using value_type = T;

    Arena* arena_{nullptr};

    ArenaAllocator() noexcept = default;
    explicit ArenaAllocator(Arena& arena) noexcept : arena_(&arena) {}

    template <typename U>
    constexpr ArenaAllocator(const ArenaAllocator<U>& other) noexcept : arena_(other.arena_) {}

    T* allocate(std::size_t n) {
        if (!arena_) throw std::bad_alloc();
        void* ptr = arena_->allocate(n * sizeof(T), alignof(T));
        if (!ptr) throw std::bad_alloc();
        return static_cast<T*>(ptr);
    }

    void deallocate(T* /*p*/, std::size_t /*n*/) noexcept {
        // Arena allocation: individual free is no-op.
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

int main() {
    Arena arena(1024 * 1024); // 1 MB arena

    std::vector<int, ArenaAllocator<int>> vec(ArenaAllocator<int>{arena});

    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
    }

    std::cout << "Vector size: " << vec.size() << "\n";
    std::cout << "Arena memory used: " << arena.used() << " bytes\n";

    arena.reset();
    std::cout << "Arena reset. Used: " << arena.used() << " bytes\n";

    return 0;
}