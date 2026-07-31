#include "pool.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include<assert.h>

uint32_t fnv1a(const char *str) {
    uint32_t hash = 0x811C9DC5; // FNV-1a 32-bit offset basis
    const uint32_t prime = 0x01000193; // FNV-1a 32-bit prime

    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= prime;
    }

    return hash;
}

StringPool *pool_create(size_t capacity)
{
    StringPool *sp = malloc(sizeof(StringPool));
    if(!sp) return NULL;

    sp->buckets = calloc(capacity, sizeof(char *));
    if(!sp->buckets)
    {
        free(sp);
        return NULL;
    }

    sp->capacity = capacity;
    sp->count = 0;

    return sp;
}

const char *pool_intern(StringPool *pool, const char *str)
{
    if (!pool || !str || pool->count >= pool->capacity) return NULL;

    uint32_t hash = fnv1a(str);
    size_t index = hash % pool->capacity;

    while(pool->buckets[index] != NULL)
    {
        if(strcmp(pool->buckets[index], str) == 0) return pool->buckets[index];
        index = (index + 1) % pool->capacity;
    }

    char *copy = strdup(str);
    if(!copy) return NULL;

    pool->buckets[index] = copy;
    pool->count++;

    return copy;
}

void pool_destroy(StringPool *pool)
{
    if (!pool) return;

    for (size_t i = 0; i < pool->capacity; i++)
    {
        free(pool->buckets[i]);
    }

    free(pool->buckets);
    free(pool);
}

int main()
{
    StringPool *pool = pool_create(16);

    const char *s1 = pool_intern(pool, "hello");
    const char *s2 = pool_intern(pool, "world");
    const char *s3 = pool_intern(pool, "hello");

    assert(s1 == s3);

    printf("s1 ptr: %p, str: %s\n", (void*)s1, s1);
    printf("s3 ptr: %p, str: %s\n", (void*)s3, s3);
    printf("s2 ptr: %p, str: %s\n", (void*)s2, s2);
    printf("Pointer match check: %s\n", (s1 == s3) ? "PASS (Same pointer)" : "FAIL");


    pool_destroy(pool);
    return 0;
}