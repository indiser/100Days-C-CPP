#include<iostream>
#include<vector>
#include<unordered_map>

template<typename T>
class MyUniquePtr {
    T *ptr;
public:
    explicit MyUniquePtr(T *p = nullptr) : ptr(p) {}
    ~MyUniquePtr() { delete ptr; }

    // no copy
    MyUniquePtr(const MyUniquePtr&) = delete;
    MyUniquePtr& operator=(const MyUniquePtr&) = delete;

    // move
    MyUniquePtr(MyUniquePtr&& other) noexcept {
        ptr = other.ptr;
        other.ptr = nullptr;
    }
    
    MyUniquePtr& operator=(MyUniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }
};



struct Foo {
    int val;
    Foo(int v) : val(v) { std::cout << "Foo " << val << " construct\n"; }
    ~Foo() { std::cout << "Foo " << val << " destruct\n"; }
    void speak() const { std::cout << "val=" << val << "\n"; }
};

int main()
{
    MyUniquePtr<Foo> p1(new Foo(10));
    p1->speak();

    MyUniquePtr<Foo> p2(std::move(p1));
    if(!p1.get()) std::cout<< "P1 is now null" <<std::endl;
    p2->speak();

    MyUniquePtr<Foo> p3(new Foo(20));
    p3 = std::move(p2);
    p3->speak();
    
    return 0;
}