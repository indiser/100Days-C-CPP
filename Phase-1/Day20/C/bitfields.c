#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>

// typedef struct __attribute__((packed))
// {
//     unsigned int a: 2;
//     unsigned int b: 4;
//     unsigned int c: 6;
// }bitfield;
uint64_t array_of_bits = 0;
#define SET_BIT(BF, N) BF |= ((uint64_t)0x00000001) << N
#define CLR_BIT(BF, N) BF &= ~((uint64_t)0x00000001) << N
#define IS_BIT_SET(BF, N) ((BF >> N) & 0x1)

#define NUM 50
int main()
{
    // bitfield bf;

    // printf("Sizeof: %lu\n", sizeof(bf));

    SET_BIT(array_of_bits, 0);
    SET_BIT(array_of_bits, 2);
    SET_BIT(array_of_bits, 0);
    SET_BIT(array_of_bits, 9);
    SET_BIT(array_of_bits, 4);
    SET_BIT(array_of_bits, 34);
    SET_BIT(array_of_bits, 5);
    SET_BIT(array_of_bits, 29);
    SET_BIT(array_of_bits, 22);



    CLR_BIT(array_of_bits, 2);

    for (int i = 0; i < 64; i++)
    {
        if(IS_BIT_SET(array_of_bits, i)) printf("+");
        else printf(".");
    }
    printf("/n");
    return 0;
}