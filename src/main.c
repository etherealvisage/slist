#include <stdio.h>
#include <stdlib.h>

#include "slist.h"

int main() {
    slist_linear_rng_context_t lrng;
    slist_default_linear_rng(&lrng);

    slist_t test;

    long n[1000];

    slist_init(&test, slist_ptrcmp, slist_linear_rng, &lrng, 50);
    for(long i = 0; i < 1000; i ++) {
        long v = rand() % 100000;
        n[i] = v;
        slist_insert(&test, (void *)v, (void *)i);
    }

    printf("[");
    for(long i = 0; i < 1000; i ++) {
        if(i != 0) printf(",");
        printf("%d", slist_depth(&test, (void *)n[i]));
    }
    printf("]\n");
    return 0;
}
