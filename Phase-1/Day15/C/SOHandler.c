#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <execinfo.h>

static void *sig_stack;
static void *stack_base;
static size_t stack_size;

void stackOverflowHandler(int sig, siginfo_t *info, void *ctx)
{
    void *fault_addr = info->si_addr;
    int is_stack_overflow = (fault_addr >= stack_base - 4096) && 
                            (fault_addr <= stack_base + stack_size);

    if (is_stack_overflow) {
        write(STDERR_FILENO, "STACK OVERFLOW DETECTED\n", 24);
        void *buffer[32];
        int frames = backtrace(buffer, 32);
        backtrace_symbols_fd(buffer, frames, STDERR_FILENO);
        _exit(1);
    }

    signal(SIGSEGV, SIG_DFL);
    raise(SIGSEGV);
}

void cause_overflow(void)
{
    char buf[1024];
    cause_overflow();
}

int main(void)
{
    pthread_t self = pthread_self();
    pthread_attr_t attr;
    pthread_getattr_np(self, &attr);
    pthread_attr_getstack(&attr, &stack_base, &stack_size);
    pthread_attr_destroy(&attr);

    size_t stack_sz = sysconf(_SC_SIGSTKSZ);
    if (stack_sz < 16384) stack_sz = 16384;
    sig_stack = malloc(stack_sz);

    stack_t ss = {
        .ss_sp = sig_stack,
        .ss_size = stack_sz,
        .ss_flags = 0
    };
    sigaltstack(&ss, NULL);

    struct sigaction sa = {0};
    sa.sa_sigaction = stackOverflowHandler;
    sa.sa_flags = SA_ONSTACK | SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGSEGV, &sa, NULL);

    cause_overflow();

    _exit(0);
}