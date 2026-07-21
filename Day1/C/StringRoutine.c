#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>

void reverse(char *s, int n) {
    for (int i = 0; i < n/2; i++) {
        char t = s[i];
        s[i] = s[n-1-i];
        s[n-1-i] = t;
    }
}
int main() {
    char buf[1000000];
    clock_t t0 = clock();
    for (int i=0;i<1000000;i++) buf[i]='a'+(i%26);
    for (int i=0;i<1000;i++) reverse(buf, 1000000);
    clock_t t1 = clock();
    printf("%f ms\n", (double)(t1-t0)*1000.0/CLOCKS_PER_SEC);
    return 0;
}