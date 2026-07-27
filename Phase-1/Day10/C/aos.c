#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<time.h>

#define N (1<<20)

struct particle
{
    float x, y, z;
    uint64_t color;
};

int main()
{
    struct particle *p = malloc(N * sizeof(struct particle));

    for (int i = 0; i < N; i++)
    {
        p[i].x = i * 1.0f;
        p[i].y = i * 2.0f;
        p[i].z = i * 3.0f;
        p[i].color = i;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    float sum = 0;
    for (int i = 0; i < N; i++)
    {
        sum += p[i].x + p[i].y + p[i].z;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    printf("sum: %.2f\n", sum);
    printf("time: %.2f ns (%.4f ns/particle)\n", ns, ns / N);

    free(p);
    return 0;
}