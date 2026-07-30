#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void handler(int num)
{
	write(STDOUT_FILENO, "I wont die\n", 13);
}

void segHandler(int num)
{
	write(STDOUT_FILENO, "Seg Fault\n", 10);
}

int main()
{
	// int *p = NULL;
	// *p = 7;
	// signal(SIGINT, handler);
	// signal(SIGTERM, handler);
	// signal(SIGKILL, handler); //Order not a request
	// signal(SIGSEGV, segHandler);

	struct sigaction sa;
	sa.sa_handler = handler;

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	while(1)
	{
		printf("Wasting Your Cycles. %d\n", getpid());
		sleep(1);
	}

	_exit(1);
}

