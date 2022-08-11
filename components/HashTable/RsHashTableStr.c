#include <stdio.h>
#include <stdlib.h>

#include "RsHashTable.h"
#include "RsHashTableInt.h"
#include "RsHashTableStr.h"

static struct table_ops table_ops = {.init = ht_init,
                                     .insert = ht_int_insert,
                                     .lookup = ht_int_lookup,
                                     .delete = ht_int_delete,
                                     .table_fini = ht_int_fini,
                                     .print = ht_int_print_table,
                                     .retrieve_values = ht_int_retrieve_values};

static unsigned long hashCode_str(unsigned char *str);

// Using djb2 akgorithm that seems to be a good job
unsigned long
hashCode_str(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

struct table *
ht_str_init(int size)
{
    return table_ops.init(size);
}

void **
ht_str_fini(struct table *t)
{
    return table_ops.table_fini(t);
}

void *
ht_str_lookup(struct table *t, char *key)
{
    return table_ops.lookup(t, hashCode_str((unsigned char *)key));
}

void ht_str_insert(struct table *t, char *key, void *value)
{
    return table_ops.insert(t, hashCode_str((unsigned char *)key), value);
}

void *
ht_str_delete(struct table *t, char *key)
{
    return table_ops.delete(t, hashCode_str((unsigned char *)key));
}

void **
ht_str_retrieve_values(struct table *t)
{
    return table_ops.retrieve_values(t);
}

void ht_str_print_table(struct table *t)
{
    table_ops.print(t);
}