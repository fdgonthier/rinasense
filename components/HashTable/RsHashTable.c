#include <stdio.h>
#include <stdlib.h>

#include "RsHashTable.h"

/*
 * Functions agnostic to the key data type
 */
struct table *ht_init(int size)
{
    struct table *t = (struct table *)malloc(sizeof(struct table));
    t->elems = 0;
    t->size = size;

    t->list = (struct node **)malloc(sizeof(struct node *) * size);

    // Table is empty
    int i;
    for (i = 0; i < size; i++)
        t->list[i] = NULL;

    return t;
}

int ht_get_num_elems(struct table *t)
{
    int num_elems = 0;

    num_elems = t->elems;

    return num_elems;
}