#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#define MIN(A, B) ({ \
            typeof(A) _a = (A); \
            typeof(B) _b = (B); \
            _a < _b ? _a : _b; \
        })

// int randomNUm()
// {
//     int num = rand() % 100;
//     printf("next --> %d\n", num);
//     return randomNUm();
// }

int main()
{
    // int n1, n2, n3;
    // n3 = MIN(n1 = randomNUm(), n2 = randomNUm());
    printf("%d\n", MIN(2, 9));
    printf("%.2f\n", MIN(4.65, 8.96));
    return 0;
}