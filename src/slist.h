/*
    Skip-list implementation, header file.

    This implementation was written by Kent "ethereal" Williams-King and is
    hereby released into the public domain. Do what you wish with it.

    No guarantees as to the correctness of the implementation are provided.
*/

#ifndef SLIST_H
#define SLIST_H

#include <stddef.h>
/* for  malloc(), free() */
#include <stdlib.h>

#define SLIST_NAME(name) slist_ ## name
#define SLIST_UNAME(name) SLIST_ ## name
#define SLIST_ALLOC(variable, type) variable = (type *)malloc(sizeof(type))
#define SLIST_ALLOC_ARRAY(variable, type, count) \
    variable = (type *)malloc(sizeof(type)*count)
#define SLIST_FREE(variable) free(variable)

typedef unsigned int SLIST_NAME(offset_t);

#ifdef SLIST_SOURCE
const SLIST_NAME(offset_t) SLIST_NAME(offset_sentinel) = -1;
#else
extern const SLIST_NAME(offset_t) SLIST_NAME(offset_sentinel);
#endif

typedef int (*SLIST_NAME(rng))(void *);
typedef int (*SLIST_NAME(comparator_t))(void *, void *);
typedef void (*SLIST_NAME(key_destructor_t))(void *);

typedef struct SLIST_NAME(node_t) SLIST_NAME(node_t);
typedef struct {
    SLIST_NAME(comparator_t) comparator;
    SLIST_NAME(key_destructor_t) destructor;
    
    SLIST_NAME(rng) rng;
    void *rng_context;

    int threshold;

    SLIST_NAME(node_t) **links;
    int link_count;
} SLIST_NAME(t);

void SLIST_NAME(init)(SLIST_NAME(t) *list, SLIST_NAME(comparator_t) cmp,
    SLIST_NAME(rng) rng, void *rng_context, int threshold);

void *SLIST_NAME(search)(SLIST_NAME(t) *list, void *key);
void *SLIST_NAME(insert)(SLIST_NAME(t) *list, void *key, void *data);
void *SLIST_NAME(remove)(SLIST_NAME(t) *list, void *key);
int SLIST_NAME(depth)(SLIST_NAME(t) *list, void *key);

typedef struct {
    int a, b;
    int n, m;
    int x;
} SLIST_NAME(linear_rng_context_t);

int SLIST_NAME(linear_rng)(void *context);
void SLIST_NAME(default_linear_rng)(SLIST_NAME(linear_rng_context_t) *context);

int SLIST_NAME(ptrcmp)(void *key1, void *key2);
int SLIST_NAME(intcmp)(void *key1, void *key2);
int SLIST_NAME(ulongcmp)(void *key1, void *key2);

#endif
