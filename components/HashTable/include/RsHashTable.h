#ifndef RS_HASH_TABLE_H
#define RS_HASH_TABLE_H

/*
 * A thread safe hash table asbtract implementation.
 */

struct node;

struct table_ops
{
    struct table *(*init)(int size);

    void **(*table_fini)(struct table *);

    int *(*retrieve_keys)(struct table *);

    void **(*retrieve_values)(struct table *t);

    void (*print)(struct table *t);

    void (*insert)(struct table *t, int key, void *value);

    void *(*lookup)(struct table *t, int key);

    void *(*delete)(struct table *t, int key);
};

struct table
{
    int size;
    int elems;
    struct node **list;
    // struct table_ops *ops;
};

/*
 * This function initializes a hash table. It creates an array of size elements
 * and set them to NULL (i.e., empty). Then, every time a new element is added
 * to the hash table, if there is a collision, it will be added as the first
 * element of the linked list associated to the aforementioned hash (i.e., the
 * new element will point to the latest added element. In pseudocode:
 * newElement->next = list[hash(key)].
 */
struct table *ht_init(int size);

int ht_get_num_elems(struct table *t);

#endif // RS_HASH_TABLE_H