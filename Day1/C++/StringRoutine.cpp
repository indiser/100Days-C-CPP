// StringRoutine.cpp
#include <iostream>
#include <chrono>
#include <cstring>

// non-templated, same as C version
void reverse(char *s, int n) {
    for (int i = 0; i < n/2; i++) {
        char t = s[i];
        s[i] = s[n-1-i];
        s[n-1-i] = t;
    }
}

// templated version
template<typename T>
void reverseT(T *s, int n) {
    for (int i = 0; i < n/2; i++) {
        T t = s[i];
        s[i] = s[n-1-i];
        s[n-1-i] = t;
    }
}

int main() {
    static char buf[1000000];
    for (int i = 0; i < 1000000; i++) buf[i] = 'a' + (i % 26);

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < 1000; i++) reverse(buf, 1000000);
    auto t1 = std::chrono::steady_clock::now();

    for (int i = 0; i < 1000000; i++) buf[i] = 'a' + (i % 26);

    auto t2 = std::chrono::steady_clock::now();
    for (int i = 0; i < 1000; i++) reverseT(buf, 1000000);
    auto t3 = std::chrono::steady_clock::now();

    double ms1 = std::chrono::duration<double, std::milli>(t1 - t0).count();
    double ms2 = std::chrono::duration<double, std::milli>(t3 - t2).count();

    std::cout << "non-template: " << ms1 << " ms\n";
    std::cout << "template:     " << ms2 << " ms\n";
    return 0;
}