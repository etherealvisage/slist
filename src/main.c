#include <stdio.h>
#include <stdlib.h>

#include "slist.h"

int main() {
    slist_linear_rng_context_t lrng;
    slist_default_linear_rng(&lrng);

    //slist_t test;
    SLIST_NAME(t) test;

    slist_init(&test, slist_ptrcmp, slist_linear_rng, &lrng, 50);

    for(long i = 0; i < 10000; i ++) {
        slist_insert(&test, (void *)i, (void *)i);
    }
    printf("previous: %p\n", slist_insert(&test, (void *)8, (void *)1));

    printf("[");
    for(long i = 0; i < 10000; i ++) {
        if(i != 0) printf(",");
        printf("%d", slist_depth(&test, (void *)i));
    }
    printf("]\n");

    printf("search result: %p\n", slist_search(&test, (void *)8));

    printf("removal result: %p\n", slist_remove(&test, (void *)8));

    printf("search result: %p\n", slist_search(&test, (void *)8));

    return 0;
}
