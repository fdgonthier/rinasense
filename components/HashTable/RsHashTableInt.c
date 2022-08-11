#include <stdio.h>
#include <stdlib.h>

#include "RsHashTable.h"
#include "RsHashTableInt.h"

struct node
{
    int key;
    void *value;
    struct node *next;
};

static int hashCode_int(struct table *t, int key);

void **ht_int_fini(struct table *t)
{
    if (t == NULL)
    {
        return NULL;
    }

    int *keys = ht_int_retrieve_keys(t);
    int num_elems = ht_get_num_elems(t);
    void **values = calloc(num_elems, sizeof(void *));

    for (int i = 0; i < num_elems; i++)
    {
        if (ht_int_lookup(t, keys[i]) != NULL)
        {
            values[i] = ht_int_delete(t, keys[i]);

            if (values[i] == NULL)
            {
                printf("Couldn't remove element with key %d", keys[i]);
                return NULL;
            }
        }
    }

    free(t->list);
    free(keys);
    free(t);

    return values;
}

int *ht_int_retrieve_keys(struct table *t)
{

    int *keys = (int *)malloc(sizeof(int) * t->elems);
    int pos = 0;
    struct node *temp;

    for (int i = 0; i < t->size; i++)
    {
        temp = t->list[i];
        while (temp != NULL)
        {
            keys[pos] = temp->key;
            pos++;
            temp = temp->next;
        }
    }

    return keys;
}

void *ht_int_lookup(struct table *t, int key)
{
    void *value = NULL;

    int pos = hashCode_int(t, key);
    struct node *list = t->list[pos];
    struct node *temp = list;

    while (temp)
    {
        if (temp->key == key)
        {
            value = temp->value;
            break;
        }
        temp = temp->next;
    }

    return value;
}

void ht_int_insert(struct table *t, int key, void *value)
{
    int pos = hashCode_int(t, key);
    struct node *newNode = (struct node *)malloc(sizeof(struct node));

    struct node *list = t->list[pos];
    struct node *temp = list;

    // Go through list associated to the hash keys in order to check if key
    // already exists.
    while (temp)
    {
        // If key exists, update value and leave.
        if (temp->key == key)
        {
            temp->value = value;
            goto UNLOCK;
        }
        temp = temp->next;
    }

    newNode->key = key;
    newNode->value = value;
    newNode->next = list;
    t->list[pos] = newNode;
    t->elems++;

UNLOCK:
    return;
}

void *ht_int_delete(struct table *t, int key)
{
    void *ret = NULL;

    int index = hashCode_int(t, key);

    if (t->list[index] != NULL)
    {
        struct node *tmp = t->list[index];
        t->elems--;

        // Mark the next available node as the first node in the list.
        t->list[index] = tmp->next;

        ret = tmp->value;
        free(tmp);
    }

    return ret;
}

void **ht_int_retrieve_values(struct table *t)
{

    void **values = (void **)malloc(t->elems * sizeof(void *));
    int pos = 0;
    struct node *temp;

    for (int i = 0; i < t->size; i++)
    {
        temp = t->list[i];
        while (temp != NULL)
        {
            values[pos] = temp->value;
            pos++;
            temp = temp->next;
        }
    }

    return values;
}

// Using silly hash function
int hashCode_int(struct table *t, int key)
{
    // Need to cast to unsigned int, otherwise the hash function for strings
    // won't work
    return (unsigned int)key % t->size;
}

void ht_int_print_table(struct table *t)
{

    for (int i = 0; i < t->size; i++)
    {
        if (t->list[i] == NULL)
        {
            printf("\t%d\t---\n", i);
        }
        else
        {
            printf("\t%d\t ", i);

            struct node *tmp = t->list[i];

            while (tmp != NULL)
            {
                printf("%u - ", tmp->key);
                tmp = tmp->next;
            }
            printf("\n");
        }
    }
    printf("\n\tTotal elements in table: %d\n\n", t->elems);
}