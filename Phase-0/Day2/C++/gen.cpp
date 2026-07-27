#include<iostream>
#define NDEBUG // Turns of assert only at compile time
#include<cassert>
using namespace std;

int main()
{
    // assert - run time fail check
    // static_assert - compile time fail check

    assert(sizeof(int) >= 5);
    static_assert(sizeof(int) >= 4, "Less or More than 4 bytes");
    cout<< "Assertion works" << endl;
    return 0;
}