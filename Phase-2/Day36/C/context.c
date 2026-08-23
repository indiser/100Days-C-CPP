#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<ucontext.h>

ucontext_t ctx_main, ctx_coro;

void coro_func()
{
    printf("Coro: step1\n");
    swapcontext(&ctx_coro, &ctx_main);

    printf("Coro: step2\n");
    swapcontext(&ctx_coro, &ctx_main);

    printf("Done\n");
}


int main()
{
    char stack[10000];

    getcontext(&ctx_coro);
    ctx_coro.uc_stack.ss_sp = stack;
    ctx_coro.uc_stack.ss_size = sizeof(stack);
    ctx_coro.uc_link = &ctx_main;

    makecontext(&ctx_coro, coro_func, 0);

    printf("Main: before first switch\n");
    swapcontext(&ctx_main, &ctx_coro);

    printf("Main: back after step 1\n");
    swapcontext(&ctx_main, &ctx_coro);

    printf("Main: back after step 2\n");
    swapcontext(&ctx_main, &ctx_coro);

    printf("Main: all done\n");
    return 0;}