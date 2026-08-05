#include <iostream>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <utility>
#include <cstdio>

// Trackers for SSOString allocations only
static size_t malloc_count = 0;
static size_t free_count = 0;

void reset_counters() {
    malloc_count = 0;
    free_count = 0;
}

class SSOString {
public:
    static constexpr size_t STACK_CAP = 15;

    // Scoped allocator wrappers replacing global operator new/delete
    static void* alloc(size_t size) {
        malloc_count++;
        void* p = std::malloc(size);
        if (!p) throw std::bad_alloc();
        return p;
    }

    static void dealloc(void* p) noexcept {
        if (p) {
            free_count++;
            std::free(p);
        }
    }

private:
    bool is_long;
    union Storage {
        struct Heap {
            char* ptr;
            size_t size;
            size_t capacity;
        } heap;
        struct Stack {
            char data[STACK_CAP + 1];
            uint8_t size;
        } stack;

        Storage() {}
        ~Storage() {}
    } storage;

public:
    // Default constructor
    SSOString() : is_long(false) {
        storage.stack.size = 0;
        storage.stack.data[0] = '\0';
    }

    // C-string constructor
    SSOString(const char* str) {
        if (!str) str = "";
        size_t len = std::strlen(str);

        if (len <= STACK_CAP) {
            is_long = false;
            storage.stack.size = static_cast<uint8_t>(len);
            std::memcpy(storage.stack.data, str, len + 1);
        } else {
            is_long = true;
            storage.heap.ptr = static_cast<char*>(alloc(len + 1));
            storage.heap.size = len;
            storage.heap.capacity = len;
            std::memcpy(storage.heap.ptr, str, len + 1);
        }
    }

    // Destructor (RAII)
    ~SSOString() {
        if (is_long) {
            dealloc(storage.heap.ptr);
        }
    }

    // Copy Constructor
    SSOString(const SSOString& other) : is_long(other.is_long) {
        if (!is_long) {
            storage.stack = other.storage.stack;
        } else {
            size_t len = other.storage.heap.size;
            size_t cap = other.storage.heap.capacity; // Preserve source capacity
            
            storage.heap.ptr = static_cast<char*>(alloc(cap + 1));
            storage.heap.size = len;
            storage.heap.capacity = cap;
            std::memcpy(storage.heap.ptr, other.storage.heap.ptr, len + 1);
        }
    }

    // Move Constructor
    SSOString(SSOString&& other) noexcept : is_long(other.is_long) {
        if (!is_long) {
            storage.stack = other.storage.stack;
        } else {
            storage.heap = other.storage.heap;
            other.is_long = false;
            other.storage.stack.size = 0;
            other.storage.stack.data[0] = '\0';
        }
    }

    // Copy Assignment
    SSOString& operator=(const SSOString& other) {
        if (this != &other) {
            SSOString temp(other);
            *this = std::move(temp);
        }
        return *this;
    }

    // Move Assignment
    SSOString& operator=(SSOString&& other) noexcept {
        if (this != &other) {
            if (is_long) {
                dealloc(storage.heap.ptr);
            }
            is_long = other.is_long;
            if (!is_long) {
                storage.stack = other.storage.stack;
            } else {
                storage.heap = other.storage.heap;
                other.is_long = false;
                other.storage.stack.size = 0;
                other.storage.stack.data[0] = '\0';
            }
        }
        return *this;
    }

    // Append
    SSOString& operator+=(const char* str) {
        if (!str) return *this;
        size_t append_len = std::strlen(str);

        if (!is_long) {
            size_t cur_len = storage.stack.size;
            if (SIZE_MAX - cur_len < append_len) return *this; // Overflow guard
            size_t new_len = cur_len + append_len;

            if (new_len <= STACK_CAP) {
                std::memcpy(storage.stack.data + cur_len, str, append_len + 1);
                storage.stack.size = static_cast<uint8_t>(new_len);
            } else {
                // 2x growth on stack-to-heap transition
                size_t new_cap = new_len * 2;
                char* new_ptr = static_cast<char*>(alloc(new_cap + 1));
                std::memcpy(new_ptr, storage.stack.data, cur_len);
                std::memcpy(new_ptr + cur_len, str, append_len + 1);

                is_long = true;
                storage.heap.ptr = new_ptr;
                storage.heap.size = new_len;
                storage.heap.capacity = new_cap;
            }
        } else {
            size_t cur_len = storage.heap.size;
            if (SIZE_MAX - cur_len < append_len) return *this; // Overflow guard
            size_t new_len = cur_len + append_len;

            if (new_len > storage.heap.capacity) {
                size_t new_cap = storage.heap.capacity * 2;
                if (new_cap < new_len) new_cap = new_len;
                char* new_ptr = static_cast<char*>(alloc(new_cap + 1));
                std::memcpy(new_ptr, storage.heap.ptr, cur_len);
                dealloc(storage.heap.ptr);
                storage.heap.ptr = new_ptr;
                storage.heap.capacity = new_cap;
            }
            std::memcpy(storage.heap.ptr + cur_len, str, append_len + 1);
            storage.heap.size = new_len;
        }
        return *this;
    }

    // Accessors
    const char* c_str() const noexcept {
        return is_long ? storage.heap.ptr : storage.stack.data;
    }

    size_t length() const noexcept {
        return is_long ? storage.heap.size : storage.stack.size;
    }

    size_t capacity() const noexcept {
        return is_long ? storage.heap.capacity : STACK_CAP;
    }

    bool empty() const noexcept {
        return length() == 0;
    }

    char operator[](size_t index) const {
        return c_str()[index];
    }
};

int main() {
    // 1. Measure Size
    std::printf("sizeof(SSOString): %zu bytes\n", sizeof(SSOString));

    // Test 1: Stack allocation <= 15 bytes
    reset_counters();
    {
        SSOString s("Hello World");
        assert(s.length() == 11);
        assert(std::strcmp(s.c_str(), "Hello World") == 0);
        assert(malloc_count == 0);
    }
    assert(free_count == 0);
    std::cout << "[PASS] Stack Init & RAII\n";

    // Test 2: Stack append
    reset_counters();
    {
        SSOString s("Hello");
        s += " World!";
        assert(s.length() == 12);
        assert(std::strcmp(s.c_str(), "Hello World!") == 0);
        assert(malloc_count == 0);
    }
    assert(free_count == 0);
    std::cout << "[PASS] Stack Append\n";

    // Test 3: Transition stack to heap
    reset_counters();
    {
        SSOString s("1234567890");
        s += "1234567";
        assert(s.length() == 17);
        assert(std::strcmp(s.c_str(), "12345678901234567") == 0);
        assert(malloc_count == 1);
    }
    assert(free_count == 1);
    std::cout << "[PASS] Stack to Heap Transition\n";

    // Test 4: Move semantics
    reset_counters();
    {
        SSOString s1("This string is long enough to force heap allocation!");
        size_t pre_move_allocs = malloc_count;
        
        SSOString s2 = std::move(s1);
        assert(malloc_count == pre_move_allocs);
        assert(std::strcmp(s2.c_str(), "This string is long enough to force heap allocation!") == 0);
        assert(s1.empty());
    }
    std::cout << "[PASS] Move Semantics\n";

    // Test 5: Stack copy & mutate isolation
    reset_counters();
    {
        SSOString s1("StackOrig");
        SSOString s2 = s1;
        assert(malloc_count == 0);
        s2 += "Mut";
        assert(std::strcmp(s1.c_str(), "StackOrig") == 0);
        assert(std::strcmp(s2.c_str(), "StackOrigMut") == 0);
    }
    assert(free_count == 0);
    std::cout << "[PASS] Test 5: Stack copy & mutate isolation\n";

    // Test 6: Heap deep copy & pointer isolation (Capacity Preserved)
    reset_counters();
    {
        SSOString s1("Heap string"); // Initial alloc
        s1 += " payload force growth!"; // Force capacity expansion (2x capacity growth)
        size_t initial_allocs = malloc_count;
        
        SSOString s2 = s1; // Copy preserves s1's expanded capacity headroom
        assert(malloc_count == initial_allocs + 1); // Exact 1 new copy allocation
        assert(s1.c_str() != s2.c_str()); // Distinct heap pointers
        
        s2 += " [MUTATED]"; // Fits inside preserved capacity -> NO realloc triggered!
        assert(malloc_count == initial_allocs + 1); // Zero new allocations on append
        
        assert(std::strcmp(s1.c_str(), "Heap string payload force growth!") == 0);
        assert(std::strcmp(s2.c_str(), "Heap string payload force growth! [MUTATED]") == 0);
    }
    // Scope exit: s1 and s2 destructed clean
    assert(free_count == malloc_count); // All allocations perfectly balanced
    std::cout << "[PASS] Test 6: Heap deep copy & capacity headroom preservation\n";

    // Test 7: Double heap assignment leak prevention
    reset_counters();
    {
        SSOString s1("First long heap string payload to force allocation");
        assert(malloc_count == 1);

        s1 = SSOString("Second long heap string payload forcing overwrite");
        assert(malloc_count == 2);
        assert(free_count == 1);
    }
    assert(malloc_count == 2);
    assert(free_count == 2);
    std::cout << "[PASS] Test 7: Double heap assignment leak prevention\n";

    // Test 8: Boundary Exact Test (15 stack max vs 16 heap trigger)
    reset_counters();
    {
        SSOString s("123456789012345"); // Exactly 15 bytes
        assert(s.length() == 15);
        assert(malloc_count == 0);

        s += "6"; // Exactly 16 bytes -> stack to heap
        assert(s.length() == 16);
        assert(malloc_count == 1);
        assert(std::strcmp(s.c_str(), "1234567890123456") == 0);
    }
    assert(free_count == 1);
    std::cout << "[PASS] Test 8: Boundary Exact Limit (15 Stack -> 16 Heap)\n";

    // Test 9: Self-move assignment safety
    reset_counters();
    {
        SSOString s1("Self move heap payload test");
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wself-move"
        s1 = std::move(s1);
        #pragma clang diagnostic pop
        assert(std::strcmp(s1.c_str(), "Self move heap payload test") == 0);
    }
    std::cout << "[PASS] Test 9: Self-move guard verification\n";

    std::cout << "\nALL C++ SSO TESTS PASSED PERFECTLY!\n";
    return 0;
}