#include "math_functions.hpp"

int addition(int a, int b) {return a + b;}
int subStraction(int a, int b)
{
    if(a > b) return a - b;
    return b - a;
}
int multiplication(int a, int b) {return a * b;}
int division(int a, int b)
{
    if(b == 0) return 0;
    if(b == 1) return a;
    return a / b;
}

int remender(int a, int b)
{
    if(b == 0) return 0;
    return a % b;
}