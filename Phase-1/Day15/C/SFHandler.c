#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<signal.h>
#include <unistd.h>
#include<execinfo.h>



void handler(int num) //CTRL + C handled
{
	write(STDOUT_FILENO, "I wont die\n", 11);
}

void segHandler(int num) //Segmentation Fault Handled
{
    void *buffer[32];
    int frames = backtrace(buffer, 32);
    write(STDERR_FILENO, "SEG FAULT\n", 10);
    backtrace_symbols_fd(buffer, frames, STDERR_FILENO);
    _exit(1);
}



int main()
{
    int *p = NULL;
    
    struct sigaction sa;
    sa.sa_handler = segHandler;

    sigaction(SIGSEGV, &sa, NULL);
    
    *p = 78;

    _exit(0);
}