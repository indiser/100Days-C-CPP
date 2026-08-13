#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

void trigger_signed_overflow(void) {
    int max = INT_MAX;
    int val = max + 1;
    printf("Signed overflow result: %d\n", val);
}

__attribute__((noinline))
static uint32_t aliasing_test(uint32_t *i, float *f) {
    *i = 100;
    *f = 0.0f; 
    return *i; 
}

void trigger_strict_aliasing(void) {
    uint32_t val;
    uint32_t res = aliasing_test(&val, (float *)&val);
    printf("Strict aliasing breach (Expected 0, Got %u)\n", res);
}

void trigger_shift_out_of_bounds(void) {
    uint32_t val = 1;
    uint32_t shift = 35;
    uint32_t res = val << shift;
    printf("Shift out of bounds result: %u\n", res);
}

void trigger_null_deref(void) {
    int *ptr = NULL;
    printf("Null deref result: %d\n", *ptr);
}

void trigger_unaligned_access(void) {
    char buf[8] = {0};
    // Force unaligned pointer (32-bit int at odd byte boundary)
    int *unaligned_ptr = (int *)(buf + 1);
    *unaligned_ptr = 42;
    printf("Unaligned access result: %d\n", *unaligned_ptr);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <1-5>\n", argv[0]);
        return 1;
    }

    int choice = atoi(argv[1]);
    switch (choice) {
        case 1: trigger_signed_overflow(); break;
        case 2: trigger_strict_aliasing(); break;
        case 3: trigger_shift_out_of_bounds(); break;
        case 4: trigger_null_deref(); break;
        case 5: trigger_unaligned_access(); break;
        default: printf("Invalid option\n"); break;
    }

    return 0;
}