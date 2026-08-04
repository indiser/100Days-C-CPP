#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>


int main()
{
    // unsigned int a = 2147483648;

    // for(int i = 0; i < 32; i++)
    // {
    //     printf("Right Shifted: %d %08x %u\n", i, a >> i, a >> i);
    // }

    int a = ~5;
    // 0101
    // 1010
    printf("%02x",a);


    return 0;
}