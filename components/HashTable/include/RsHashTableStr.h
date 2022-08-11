#ifndef _HASH_TABLE_STR_H
#define _HASH_TABLE_STR_H

/*
 * Methods for dealing with objects that have a STRING as key
 */

struct table *ht_str_init(int size);

void **ht_str_fini(struct table *table);

void *ht_str_lookup(struct table *table, char *key);

void ht_str_insert(struct table *table, char *key, void *value);

void *ht_str_delete(struct table *table, char *key);

void ht_str_print_table(struct table *table);

void **ht_str_retrieve_values(struct table *t);

#endif /* _HASH_TABLE_STR_H */