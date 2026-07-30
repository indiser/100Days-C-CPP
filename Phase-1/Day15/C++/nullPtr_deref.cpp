#include <iostream>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <execinfo.h>

class CrashHandler {
public:
    CrashHandler() {
        struct sigaction sa{};
        sa.sa_handler = handle_segv;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGSEGV, &sa, nullptr);
    }

private:
    static void handle_segv(int sig) {
        void* buffer[32];
        int frames = backtrace(buffer, 32);
        write(STDERR_FILENO, "CPP SEG FAULT DETECTED\n", 23);
        backtrace_symbols_fd(buffer, frames, STDERR_FILENO);
        std::_Exit(1);
    }
};

int main() {
    CrashHandler handler;

    int* p = nullptr;
    *p = 42;

    return 0;
}