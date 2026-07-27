#include<iostream>
#include<vector>
#include<unordered_map>

template<typename T>
struct ControlBlock {
    T *ptr;
    int refcount;
};

template<typename T>
class MySharedPtr {
    ControlBlock<T> *ctrl;
public:
    explicit MySharedPtr(T *p = nullptr) {
        ctrl = new ControlBlock<T>{p, 1};
    }

    MySharedPtr(const MySharedPtr &other) {
        ctrl = other.ctrl;
        ctrl->refcount++;
    }

    MySharedPtr& operator=(const MySharedPtr &other) {
        if (this != &other) {
            release();
            ctrl = other.ctrl;
            ctrl->refcount++;
        }
        return *this;
    }

    ~MySharedPtr() { release(); }

    T& operator*() const { return *ctrl->ptr; }
    T* operator->() const { return ctrl->ptr; }

private:
    void release() {
        if(!ctrl) return;
        ctrl->refcount--;
        if (ctrl->refcount == 0) {
            delete ctrl->ptr;
            delete ctrl;
        }
    }
};


struct Foo {
    int val;
    Foo(int v) : val(v) { std::cout << "Foo " << val << " construct\n"; }
    ~Foo() { std::cout << "Foo " << val << " destruct\n"; }
    void speak() const { std::cout << "val=" << val << "\n"; }
};

int main()
{
    MySharedPtr<Foo> sp1(new Foo(20));
    sp1->speak();

    {
        MySharedPtr<Foo> sp2 = sp1;
        sp2->speak();

        MySharedPtr<Foo> sp3(new Foo(50));
        sp3 = sp2;
        sp3->speak();
    }

    std::cout<<"Back in main loop"<< std::endl;
    sp1->speak();
    return 0;
}