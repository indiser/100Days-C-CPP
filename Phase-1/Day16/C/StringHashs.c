#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>

uint32_t djb2(const char *str)
{
    uint32_t hash = 5381;
    while (*str)
    {
        hash = ((hash << 5) + hash) + (uint8_t)*str;
        str++;
    }
    return hash;
}

uint32_t fnv1a(const char *str) {
    uint32_t hash = 0x811C9DC5; // FNV-1a 32-bit offset basis
    const uint32_t prime = 0x01000193; // FNV-1a 32-bit prime

    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= prime;
    }

    return hash;
}

uint32_t murmur3_32(const char *key, uint32_t seed) {
    uint32_t len = (uint32_t)strlen(key);
    const uint8_t *data = (const uint8_t *)key;
    const int nblocks = len / 4;

    uint32_t h1 = seed;

    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    // Body
    for (int i = 0; i < nblocks; i++) {
        uint32_t k1;
        memcpy(&k1, data + i * 4, sizeof(uint32_t)); // Safe aliasing

        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> (32 - 15));
        k1 *= c2;

        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> (32 - 13));
        h1 = h1 * 5 + 0xe6546b64;
    }

    // Tail
    const uint8_t *tail = data + nblocks * 4;
    uint32_t k1 = 0;

    switch (len & 3) {
        case 3: k1 ^= (uint32_t)tail[2] << 16; [[fallthrough]];
        case 2: k1 ^= (uint32_t)tail[1] << 8;  [[fallthrough]];
        case 1: k1 ^= (uint32_t)tail[0];
                k1 *= c1; 
                k1 = (k1 << 15) | (k1 >> (32 - 15)); 
                k1 *= c2; 
                h1 ^= k1;
    }

    // Finalization
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}

int main()
{
    const char *str = "God is good";

    printf("String hash in Djb2: %u\n", djb2(str));
    printf("String hash in FNV-1a: %u\n", fnv1a(str));
    printf("String hash using MurMurHash: %u\n", murmur3_32(str, 42));
    return 0;
}