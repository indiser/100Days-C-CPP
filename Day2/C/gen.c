#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

// Function Overloading in c
int additioni(int a, int b) {return a + b;}
float additionf(float a, float b) {return a + b;}

#define addition(a, b) _Generic((a), \
        int: additioni, \
        float: additionf \
)(a, b)

int main()
{
    // printf("Integres: %d\n", additioni(10, 20));
    // printf("Floats: %.2f\n", additionf(10.1, 20.2));

    printf("Generic int: %d\n", addition(10, 20));
    printf("Generic float: %.2f\n", addition(10.1f, 20.2f));

    return 0;
}