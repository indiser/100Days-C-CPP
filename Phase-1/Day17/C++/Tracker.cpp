#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <mutex>

struct AllocEntry {
    void* ptr;
    std::size_t size;
    bool is_array;
};

constexpr std::size_t MAX_RECORDS = 1024;
static AllocEntry g_alloc_map[MAX_RECORDS];
static std::size_t g_alloc_count = 0;
static std::mutex g_tracker_mutex;

static thread_local bool g_inside_tracker = false;

static void add_record(void* ptr, std::size_t size, bool is_array) {
    if (!ptr || g_inside_tracker) return;
    g_inside_tracker = true;
    
    std::lock_guard<std::mutex> lock(g_tracker_mutex);
    if (g_alloc_count < MAX_RECORDS) {
        g_alloc_map[g_alloc_count++] = {ptr, size, is_array};
    }
    
    g_inside_tracker = false;
}

static void remove_record(void* ptr, bool is_array) {
    if (!ptr || g_inside_tracker) return;
    g_inside_tracker = true;

    std::lock_guard<std::mutex> lock(g_tracker_mutex);
    for (std::size_t i = 0; i < g_alloc_count; ++i) {
        if (g_alloc_map[i].ptr == ptr) {
            if (g_alloc_map[i].is_array != is_array) {
                std::cout << "[ERROR] Mismatched delete for " << ptr << "\n";
            }
            g_alloc_map[i] = g_alloc_map[--g_alloc_count];
            g_inside_tracker = false;
            return;
        }
    }
    
    std::cout << "[ERROR] Invalid or double delete for " << ptr << "\n";
    g_inside_tracker = false;
}

// 1. New overloads
void* operator new(std::size_t size) {
    void* ptr = std::malloc(size);
    if (!ptr) throw std::bad_alloc();
    add_record(ptr, size, false);
    return ptr;
}

void* operator new[](std::size_t size) {
    void* ptr = std::malloc(size);
    if (!ptr) throw std::bad_alloc();
    add_record(ptr, size, true);
    return ptr;
}

// 2. Unsized Delete overloads
void operator delete(void* ptr) noexcept {
    remove_record(ptr, false);
    std::free(ptr);
}

void operator delete[](void* ptr) noexcept {
    remove_record(ptr, true);
    std::free(ptr);
}

// 3. Sized Delete overloads (C++14)
void operator delete(void* ptr, std::size_t size) noexcept {
    (void)size;
    operator delete(ptr);
}

void operator delete[](void* ptr, std::size_t size) noexcept {
    (void)size;
    operator delete[](ptr);
}

struct LeakChecker {
    ~LeakChecker() {
        if (g_alloc_count == 0) {
            std::cout << "NO CPP LEAKS\n";
            return;
        }
        std::cout << "CPP LEAKS DETECTED:\n";
        for (std::size_t i = 0; i < g_alloc_count; ++i) {
            std::cout << "Leak " << g_alloc_map[i].size << " bytes at " 
                      << g_alloc_map[i].ptr 
                      << " [Array: " << (g_alloc_map[i].is_array ? "yes" : "no") << "]\n";
        }
    }
};

static LeakChecker g_checker;

int main() {
    int* a = new int(10);
    int* arr = new int[50];
    (void)arr;

    delete a;
    // delete[] arr; // Leaked test

    return 0;
}