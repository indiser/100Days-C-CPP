#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdint.h>

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
uint64_t count = 0;
bool turn = false; // state variable

void *increment(void *arg)
{
    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&mut);
        while (turn != false) // check predicate while holding lock
        {
            pthread_cond_wait(&cv, &mut); // releases mut, sleeps, re-acquires mut
        }
        count++;
        printf("incrementing....\n");
        turn = true;
        pthread_cond_signal(&cv);
        pthread_mutex_unlock(&mut);
    }
    return NULL;
}

void *decrement(void *arg)
{
    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&mut);
        while (turn != true) // check predicate while holding lock
        {
            pthread_cond_wait(&cv, &mut);
        }
        count--;
        printf("decrementing....\n");
        turn = false;
        pthread_cond_signal(&cv);
        pthread_mutex_unlock(&mut);
    }
    return NULL;
}

int main()
{
    pthread_t p1, p2;

    pthread_create(&p1, NULL, increment, NULL);
    pthread_create(&p2, NULL, decrement, NULL);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&cv);
    printf("%lu\n", count);
    return 0;
}