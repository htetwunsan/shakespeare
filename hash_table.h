#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>

typedef struct item {
    char* key;
    size_t val;

    struct item* next;
} ht_item;

typedef struct {
    size_t size;
    size_t cap;
    ht_item** buckets;
} ht_table;

typedef void ht_each_fn(ht_item* item);

ht_table* ht_malloc();
void ht_free(ht_table* table);

ht_item* ht_set(ht_table* table, const char* key, const size_t val);
ht_item* ht_get(const ht_table* table, const char* key);
size_t ht_size(const ht_table* table);
void ht_del(const ht_table* table, const char* key);
void ht_each(const ht_table* table, ht_each_fn* fn);

#endif
