#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<time.h>

#define N (1<<20)

int main()
{
    float *x = malloc(N * sizeof(float));
    float *y = malloc(N * sizeof(float));
    float *z = malloc(N * sizeof(float));
    uint64_t *color = malloc(N * sizeof(uint64_t));

    for (int i = 0; i < N; i++)
    {
        x[i] = i * 1.0f;
        y[i] = i * 2.0f;
        z[i] = i * 3.0f;
        color[i] = i;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    float sum = 0;
    for (int i = 0; i < N; i++)
    {
        sum += x[i] + y[i] + z[i];
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    printf("sum: %.2f\n", sum);
    printf("time: %.2f ns (%.4f ns/particle)\n", ns, ns / N);

    free(x); free(y); free(z); free(color);
    return 0;
}