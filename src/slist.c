#include <stdio.h> // debugging
/*
    Skip-list implementation, source file.

    This implementation was written by Kent "ethereal" Williams-King and is
    hereby released into the public domain. Do what you wish with it.

    No guarantees as to the correctness of the implementation are provided.
*/

#include "slist.h"

/* required definitions */
#ifndef NULL
    #define NULL ((void *)0)
#endif

struct SLIST_NAME(node_t) {
    void *key;
    void *data;

    SLIST_NAME(node_t) **links;
    int link_count;
};

static SLIST_NAME(node_t) *SLIST_NAME(search_helper)(SLIST_NAME(t) *list,
    SLIST_NAME(node_t) **links, int link_count, void *key);
static SLIST_NAME(node_t) *SLIST_NAME(insert_helper)(SLIST_NAME(t) *list,
    SLIST_NAME(node_t) ***links, int level, void *key,
    int *created);

static SLIST_NAME(node_t) *SLIST_NAME(make_node)(SLIST_NAME(t) *list);

void SLIST_NAME(init)(SLIST_NAME(t) *list, SLIST_NAME(comparator_t) cmp,
    SLIST_NAME(rng) rng, void *rng_context, int threshold) {

    list->comparator = cmp;
    list->destructor = 0; // TODO

    list->rng = rng;
    list->rng_context = rng_context;

    list->threshold = threshold;

    list->links = NULL;
    list->link_count = 0;
}

static SLIST_NAME(node_t) *SLIST_NAME(search_helper)(SLIST_NAME(t) *list,
    SLIST_NAME(node_t) **links, int link_count, void *key) {

    for(int i = link_count-1; i >= 0; i --) {
        if(!links[i]) continue;

        int cmp = list->comparator(key, links[i]->key);
        // have we found the target?
        if(cmp == 0) return links[i];
        // have we overshot the target?
        else if(cmp < 0) continue;
        // have we undershot the target?
        else if(cmp > 0) {
            return SLIST_NAME(search_helper)(list, links[i]->links,
                links[i]->link_count, key);
        }
    }

    // if reached, then there is no such node.
    return NULL;
}

void *SLIST_NAME(search)(SLIST_NAME(t) *list, void *key) {
    SLIST_NAME(node_t) *node = SLIST_NAME(search_helper)(list, list->links,
        list->link_count, key);

    if(node) return node->data;
    else return NULL;
}

static SLIST_NAME(node_t) *SLIST_NAME(make_node)(SLIST_NAME(t) *list) {
    int height = 1;
    while(1) {
        if(list->rng(list->rng_context) > list->threshold) break;
        height ++;
    }

    SLIST_NAME(node_t) *node;
    SLIST_ALLOC(node, SLIST_NAME(node_t));

    node->link_count = height;
    SLIST_ALLOC_ARRAY(node->links, SLIST_NAME(node_t) *, height);

    for(int i = 0; i < height; i ++) {
        node->links[i] = NULL;
    }

    // handle insertions at slist root
    if(height > list->link_count) {
        SLIST_NAME(node_t) **new_list_links;
        SLIST_ALLOC_ARRAY(new_list_links, SLIST_NAME(node_t) *, height);
        for(int i = 0; i < list->link_count; i ++) {
            new_list_links[i] = list->links[i];
        }
        for(int i = list->link_count; i < height; i ++) {
            new_list_links[i] = node;
        }

        SLIST_FREE(list->links);
        list->links = new_list_links;
        list->link_count = height;
    }

    return node;
}

static SLIST_NAME(node_t) *SLIST_NAME(insert_helper)(SLIST_NAME(t) *list,
    SLIST_NAME(node_t) ***links, int level, void *key,
    int *created) {

    SLIST_NAME(node_t) *node = NULL;
    int entry_level = level;

    //printf("insert_helper invoked, entry_level = %d\n", entry_level);

    for(; level >= 0; level --) {
        if(!(*links)[level]) continue;

        //printf("links[%d] = %p\n", level, (*links)[level]);

        int cmp = list->comparator(key, (*links)[level]->key);
        //printf("comparison: %d\n", cmp);
        // have we found the target?
        if(cmp == 0) {
            *created = 0;
            return (*links)[level];
        }
        // have we overshot the target?
        else if(cmp < 0) continue;
        // have we undershot the target?
        else if(cmp > 0) {
            node = SLIST_NAME(insert_helper)(list,
                &(*links)[level]->links, level, key,
                created);

            if(!*created) {
                return node;
            }
            break;
        }
    }

    //printf("finished loop\n");

    // do we create the node, or has the node been created by a sub-invocation?
    if(node == NULL) {
        // if reached, then there is no such node, so a new one is needed.
        node = SLIST_NAME(make_node)(list);
        node->key = key;
        level = 0;
        *created = 1;
    }

    for(level ++; level <= entry_level && level < node->link_count; level ++) {
        node->links[level] = (*links)[level];
        (*links)[level] = node;
    }

    // the recursive parents will handle inserting at higher levels.
    return node;
}

void *SLIST_NAME(insert)(SLIST_NAME(t) *list, void *key, void *data) {
    // is the slist empty?
    if(list->links == 0) {
        SLIST_NAME(node_t) *node = SLIST_NAME(make_node)(list);
        node->key = key;
        node->data = data;
        return NULL;
    }

    int created = 0;
    SLIST_NAME(node_t) *node = SLIST_NAME(insert_helper)(list, &list->links,
        list->link_count-1, key, &created);

    if(node) {
        void *old = node->data;
        node->data = data;
        return old;
    }
    else {
        // should never be reached!
        // if so, insert_helper failed to allocate memory.
        return 0;
    }
}

int SLIST_NAME(linear_rng)(void *context) {
    SLIST_NAME(linear_rng_context_t) *lcontext = context;

    int nx = lcontext->a * lcontext->x + lcontext->b;

    lcontext->x = nx % lcontext->n;

    return nx % lcontext->m;
}

void SLIST_NAME(default_linear_rng)(
    SLIST_NAME(linear_rng_context_t) *context) {

    context->m = 100;
    context->n = 32771;
    context->a = 169;
    context->b = 3705;
    context->x = 1;
}

int SLIST_NAME(ptrcmp)(void *key1, void *key2) {
    if(key1 < key2) return -1;
    else if(key1 > key2) return 1;
    else return 0;
}

int SLIST_NAME(intcmp)(void *key1, void *key2) {
    int val1 = *(int *)key1;
    int val2 = *(int *)key2;
    if(val1 < val2) return -1;
    else if(val1 > val2) return 1;
    else return 0;
}

int SLIST_NAME(ulongcmp)(void *key1, void *key2) {
    unsigned long val1 = *(int *)key1;
    unsigned long val2 = *(int *)key2;
    if(val1 < val2) return -1;
    else if(val1 > val2) return 1;
    else return 0;
}

void dump_slist(SLIST_NAME(t) *list) {
    printf("slist:\n");
    printf("root links:\n");
    for(int i = 0; i < list->link_count; i ++) {
        printf("\t[%d]: %p\n", i, list->links[i]);
    }
    if(list->link_count == 0) return;
    SLIST_NAME(node_t) *c = list->links[0];
    while(c) {
        printf("link %p (key: %p)\n", c, c->key);
        for(int i = 0; i < c->link_count; i ++) {
            printf("\t[%d]: %p\n", i, c->links[i]);
        }
        c = c->links[0];
    }
}
