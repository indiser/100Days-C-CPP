#include <iostream>
#include <concepts>
#include <vector>

// Define concept using requires expression
template <typename T>
concept Numeric = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
    { a - b } -> std::same_as<T>;
    { a * b } -> std::same_as<T>;
};

// Define concept checking member function
template <typename T>
concept Printable = requires(std::ostream& os, const T& obj) {
    { os << obj } -> std::same_as<std::ostream&>;
};

// Constrain template function with concept
template <Numeric T>
T multiply_add(T a, T b, T c) {
    return (a * b) + c;
}

// Constrain via requires clause
template <typename Container>
requires requires(Container c) {
    { c.size() } -> std::convertible_to<std::size_t>;
    typename Container::value_type;
}
void print_info(const Container& c) {
    std::cout << "Size: " << c.size() << "\n";
}

// Function overloaded by concept constraints
void process(Numeric auto val) {
    std::cout << "Numeric process: " << val << "\n";
}

int main() {
    // Valid: int fits Numeric
    std::cout << "Math: " << multiply_add(2, 3, 4) << "\n";

    // Valid: std::vector fits Container requirements
    std::vector<int> v = {1, 2, 3};
    print_info(v);

    // Concept auto dispatch
    process(42);
    process(3.14);

    // Next line fails at compile time if uncommented:
    // multiply_add("foo", "bar", "baz"); // Error: std::string not Numeric

    return 0;
}