#ifndef _HASH_TABLE_INT_H
#define _HASH_TABLE_INT_H

/*
 * Methods for dealing with objects that have an INT as key
 */
void *ht_int_lookup(struct table *t, int key);

void ht_int_insert(struct table *t, int key, void *value);

void *ht_int_delete(struct table *t, int key);

void ht_int_print_table(struct table *t);

int *ht_int_retrieve_keys(struct table *t);

void **ht_int_fini(struct table *t);

void **ht_int_retrieve_values(struct table *t);

#endif /* _HASH_TABLE_INT_H */