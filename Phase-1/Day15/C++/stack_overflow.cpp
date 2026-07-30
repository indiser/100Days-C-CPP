#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <pthread.h>
#include <execinfo.h>

class StackOverflowHandler {
public:
    StackOverflowHandler() {
        pthread_t self = pthread_self();
        pthread_attr_t attr;
        pthread_getattr_np(self, &attr);
        pthread_attr_getstack(&attr, &stack_base, &stack_size);
        pthread_attr_destroy(&attr);

        size_t stack_sz = sysconf(_SC_SIGSTKSZ);
        if (stack_sz < 16384) stack_sz = 16384;
        sig_stack = std::malloc(stack_sz);

        stack_t ss{};
        ss.ss_sp = sig_stack;
        ss.ss_size = stack_sz;
        ss.ss_flags = 0;
        sigaltstack(&ss, nullptr);

        struct sigaction sa{};
        sa.sa_sigaction = handle_overflow;
        sa.sa_flags = SA_ONSTACK | SA_SIGINFO;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGSEGV, &sa, nullptr);
    }

    ~StackOverflowHandler() {
        std::free(sig_stack);
    }

private:
    inline static void* sig_stack = nullptr;
    inline static void* stack_base = nullptr;
    inline static size_t stack_size = 0;

    static void handle_overflow(int sig, siginfo_t* info, void* ctx) {
        void* fault_addr = info->si_addr;
        bool is_overflow = (fault_addr >= static_cast<char*>(stack_base) - 4096) && 
                           (fault_addr <= static_cast<char*>(stack_base) + stack_size);

        if (is_overflow) {
            void* buffer[32];
            int frames = backtrace(buffer, 32);
            write(STDERR_FILENO, "CPP STACK OVERFLOW DETECTED\n", 28);
            backtrace_symbols_fd(buffer, frames, STDERR_FILENO);
            std::_Exit(1);
        }

        std::signal(SIGSEGV, SIG_DFL);
        std::raise(SIGSEGV);
    }
};

void cause_overflow() {
    volatile char buf[1024];
    cause_overflow();
}

int main() {
    StackOverflowHandler handler;

    cause_overflow();

    return 0;
}