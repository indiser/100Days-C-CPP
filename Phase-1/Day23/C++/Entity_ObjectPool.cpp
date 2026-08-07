#include <iostream>
#include <cstdint>
#include <cassert>
#include <cstddef>
#include <utility>
#include <memory>

struct Entity {
    uint64_t x{0};
    uint64_t y{0};
    uint64_t z{0};
    unsigned int velocity{0};
    unsigned int health{0};
    bool flag{false};

    Entity(uint64_t x, uint64_t y, uint64_t z, unsigned int hp) 
        : x(x), y(y), z(z), health(hp) {}
    ~Entity() = default;
};

template <typename T, std::size_t Capacity>
class ObjectPool {
private:
    union Node {
        alignas(alignof(T)) std::byte storage[sizeof(T)];
        Node* next;

        Node() {}
        ~Node() {}
    };

    Node pool_[Capacity];
    Node* freelist_{nullptr};
    bool in_use_[Capacity]{false};

public:
    ObjectPool() {
        for (std::size_t i = 0; i < Capacity - 1; ++i) {
            pool_[i].next = &pool_[i + 1];
        }
        pool_[Capacity - 1].next = nullptr;
        freelist_ = &pool_[0];
    }

    ~ObjectPool() {
        reset();
    }

    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

    template <typename... Args>
    T* construct(Args&&... args) {
        if (!freelist_) return nullptr;

        Node* node = freelist_;
        freelist_ = freelist_->next;

        std::size_t index = static_cast<std::size_t>(node - pool_);
        in_use_[index] = true;

        // Placement new in allocated storage
        T* obj = ::new (static_cast<void*>(node->storage)) T(std::forward<Args>(args)...);
        return obj;
    }

    bool destroy(T* ptr) {
        if (!ptr) return false;

        auto node_ptr = reinterpret_cast<Node*>(ptr);
        std::uintptr_t base = reinterpret_cast<std::uintptr_t>(pool_);
        std::uintptr_t p = reinterpret_cast<std::uintptr_t>(node_ptr);

        // Bounds check
        if (p < base || p >= base + sizeof(pool_)) {
            std::cerr << "destroy: pointer outside pool\n";
            return false;
        }

        std::size_t index = static_cast<std::size_t>(node_ptr - pool_);

        // Double-free guard
        if (!in_use_[index]) {
            std::cerr << "destroy: double free detected\n";
            return false;
        }

        // Call explicit destructor
        ptr->~T();

        in_use_[index] = false;
        node_ptr->next = freelist_;
        freelist_ = node_ptr;
        return true;
    }

    // RAII Smart Pointer acquire
    struct Deleter {
        ObjectPool* pool;
        void operator()(T* ptr) const {
            if (pool) pool->destroy(ptr);
        }
    };

    using UniqueHandle = std::unique_ptr<T, Deleter>;

    template <typename... Args>
    UniqueHandle acquire(Args&&... args) {
        T* ptr = construct(std::forward<Args>(args)...);
        return UniqueHandle(ptr, Deleter{this});
    }

    void reset() {
        for (std::size_t i = 0; i < Capacity; ++i) {
            if (in_use_[i]) {
                reinterpret_cast<T*>(pool_[i].storage)->~T();
                in_use_[i] = false;
            }
        }
        for (std::size_t i = 0; i < Capacity - 1; ++i) {
            pool_[i].next = &pool_[i + 1];
        }
        pool_[Capacity - 1].next = nullptr;
        freelist_ = &pool_[0];
    }
};

int main() {
    constexpr std::size_t PoolSize = 1028;
    ObjectPool<Entity, PoolSize> pool;

    // Direct construct/destroy
    Entity* e1 = pool.construct(10ULL, 20ULL, 30ULL, 100u);
    assert(e1 != nullptr);
    assert(e1->x == 10);

    bool freed = pool.destroy(e1);
    assert(freed == true);

    // Double free guard check
    assert(pool.destroy(e1) == false);

    // RAII UniqueHandle test
    {
        auto handle = pool.acquire(1ULL, 2ULL, 3ULL, 50u);
        assert(handle != nullptr);
        assert(handle->health == 50);
    } // Auto-destroyed here

    // Pool exhaustion test
    Entity* arr[PoolSize];
    for (std::size_t i = 0; i < PoolSize; ++i) {
        arr[i] = pool.construct(0ULL, 0ULL, 0ULL, 0u);
    }
    assert(pool.construct(0ULL, 0ULL, 0ULL, 0u) == nullptr); // Exhausted

    for (std::size_t i = 0; i < PoolSize; ++i) {
        pool.destroy(arr[i]);
    }

    return 0;
}