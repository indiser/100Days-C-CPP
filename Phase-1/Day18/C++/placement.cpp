#include <iostream>
#include <new>
#include <utility>
#include <cstddef>
#include <cstdint>

template <typename T, std::size_t Capacity>
class FixedObjectPool {
private:
    alignas(alignof(T)) std::byte storage_[Capacity * sizeof(T)];
    bool slot_used_[Capacity]{false};

public:
    FixedObjectPool() = default;

    template <typename... Args>
    T* allocate(Args&&... args) {
        for (std::size_t i = 0; i < Capacity; ++i) {
            if (!slot_used_[i]) {
                slot_used_[i] = true;
                void* address = &storage_[i * sizeof(T)];
                // Placement new
                return ::new (address) T(std::forward<Args>(args)...);
            }
        }
        return nullptr; // Pool full
    }

    void deallocate(T* ptr) {
        if (!ptr) return;

        std::size_t index = (reinterpret_cast<std::byte*>(ptr) - storage_) / sizeof(T);
        if (index < Capacity && slot_used_[index]) {
            // Explicit destructor call
            ptr->~T();
            slot_used_[index] = false;
        }
    }
};

struct Particle {
    int id;
    float velocity;

    Particle(int id, float v) : id(id), velocity(v) {
        std::cout << "Particle " << id << " created\n";
    }

    ~Particle() {
        std::cout << "Particle " << id << " destroyed\n";
    }
};

int main() {
    FixedObjectPool<Particle, 2> pool;

    Particle* p1 = pool.allocate(1, 15.5f);
    Particle* p2 = pool.allocate(2, 30.0f);

    std::cout << "P1 ID: " << p1->id << ", Vel: " << p1->velocity << "\n";

    pool.deallocate(p1); // Invokes ~Particle(), frees slot

    Particle* p3 = pool.allocate(3, 45.0f); // Reuses p1 slot

    pool.deallocate(p2);
    pool.deallocate(p3);

    return 0;
}