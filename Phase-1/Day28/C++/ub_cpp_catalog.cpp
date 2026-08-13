#include <iostream>
#include <cstring>
#include <bit>
#include <cstdlib>
#include <cstdint>

// ==========================================
// 1. STRICT ALIASING & PUNNING DIVERGENCE
// ==========================================

__attribute__((noinline))
static uint32_t cpp_aliasing_test(uint32_t *i, float *f) {
    *i = 100;
    *f = 0.0f; // Bit pattern 0. UB via reinterpret_cast punning.
    return *i; // -O3 reuses cached 100. Optimizer assumes *i and *f cannot alias.
}

void trigger_reinterpret_aliasing() {
    uint32_t val;
    uint32_t res = cpp_aliasing_test(&val, reinterpret_cast<float*>(&val));
    std::cout << "[UB] reinterpret_cast aliasing result (Expected 0, Got " << res << ")\n";
}

union IllegalPunning {
    uint32_t i;
    float f;
};

void trigger_union_punning_ub() {
    IllegalPunning u;
    u.i = 100;
    u.f = 0.0f; // C++ rule: writing inactive member changes active member.
    // Reading non-active member 'i' = UB in C++ (Unlike C99 extension).
    std::cout << "[UB] Union inactive read result: " << u.i << "\n";
}

void trigger_defined_punning() {
    uint32_t val = 100;
    float f_val = 0.0f;
    // Defined Method 1: std::memcpy
    std::memcpy(&val, &f_val, sizeof(val));
    
    // Defined Method 2: C++20 std::bit_cast
    uint32_t val2 = std::bit_cast<uint32_t>(f_val);
    
    std::cout << "[Defined] memcpy result: " << val << " | bit_cast result: " << val2 << "\n";
}

// ==========================================
// 2. C++ OBJECT LIFETIME / VTABLE UB (NO C EQUIVALENT)
// ==========================================

class Base {
public:
    Base() {
        // Calling virtual function from ctor dispatches to Base::pv_func, NOT Derived.
        // Pure virtual call inside ctor = UB / abort (vtable partially constructed).
        call_pv();
    }
    
    void call_pv() {
        pv_func(); // Indirect call hides pure virtual dispatch from compiler warning.
    }

    virtual void pv_func() = 0;
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    void pv_func() override {
        std::cout << "Derived pv_func executed\n";
    }
};

void trigger_vtable_ctor_ub() {
    std::cout << "[UB] Constructing object with pure virtual call in ctor...\n";
    Derived d; // Crashes at runtime: pure virtual method called
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <1-4>\n";
        return 1;
    }

    int choice = std::atoi(argv[1]);
    switch (choice) {
        case 1: trigger_reinterpret_aliasing(); break;
        case 2: trigger_union_punning_ub(); break;
        case 3: trigger_defined_punning(); break;
        case 4: trigger_vtable_ctor_ub(); break;
        default: std::cout << "Invalid option\n"; break;
    }

    return 0;
}