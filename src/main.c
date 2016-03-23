#include <stdio.h>
#include <stdlib.h>

#include "slist.h"

int main() {
    slist_linear_rng_context_t lrng;
    slist_default_linear_rng(&lrng);

    //slist_t test;
    SLIST_NAME(t) test;

    slist_init(&test, slist_ptrcmp, slist_linear_rng, &lrng, 50);

    for(int i = 0; i < 1000; i ++) {
        slist_insert(&test, (void *)i, (void *)i);
    }
    printf("previous: %p\n", slist_insert(&test, (void *)8, (void *)1));

    printf("search result: %p\n", slist_search(&test, (void *)8));

    return 0;
}
