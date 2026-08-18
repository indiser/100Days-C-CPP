#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

void *myTurn(void *args)
{
    while(1)
    {
        sleep(1);
        printf("My Turn\n");
    }
    return NULL;
}

void yourTurn()
{
    while(1)
    {
        sleep(2);
        printf("your Turn\n");
    }
}

int main()
{
    pthread_t newThread;

    pthread_create(&newThread, NULL, myTurn, NULL);
    yourTurn();

    pthread_join(newThread, NULL);

    pthread_detach(newThread);
    return 0;
}