#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

#define FNV_PRIME 16777619
#define FNV_OFFSET 2166136261
#define INITIAL_CAP 64
#define TABLE_MAX_LOAD 0.75

static uint32_t hash_key(const char* key) {
    assert(key != NULL);

    uint32_t hash = FNV_OFFSET;

    for (const char* p = key; *p; p++) {
        hash ^= (uint8_t)(*p);
        hash *= FNV_PRIME;
    }

    return hash;
}

static size_t bucket_index(const size_t cap, const char* key) {
    assert(key != NULL);

    return hash_key(key) & (cap - 1);
}

static void free_item(ht_item* item) {
    if (!item)
        return;

    free(item->key);
    free(item);
}

static ht_item* malloc_item(const char* key, const size_t val) {
    assert(key != NULL);

    ht_item* item = malloc(sizeof(ht_item));
    if (!item) {
        goto error;
    }
    item->key = strdup(key);
    if (!item->key) {
        goto error;
    }
    item->val = val;
    item->next = NULL;

    return item;

error:
    free_item(item);
    return NULL;
}

static void free_bucket(ht_item* bucket) {
    while (bucket) {
        ht_item* item = bucket;
        bucket = bucket->next;
        free_item(item);
    }
}

static ht_item* find_item(ht_item* bucket, const char* key) {
    assert(key != NULL);

    while (bucket) {
        if (bucket->key && strcmp(bucket->key, key) == 0) {
            return bucket;
        }
        bucket = bucket->next;
    }

    return NULL;
}

void ht_free(ht_table* table) {
    if (!table)
        return;

    if (table->buckets) {
        for (size_t i = 0; i < table->cap; i++) {
            free_bucket(table->buckets[i]);
        }
        free(table->buckets);
    }

    free(table);
}

ht_table* ht_malloc() {
    ht_table* table = malloc(sizeof(ht_table));
    if (!table) {
        goto error;
    }

    table->size = 0;
    table->cap = INITIAL_CAP;

    table->buckets = calloc(table->cap, sizeof(ht_item*));
    if (!table->buckets) {
        goto error;
    }

    return table;

error:
    ht_free(table);
    return NULL;
}

static int ht_realloc(ht_table* table) {
    assert(table != NULL);

    size_t cap = table->cap * 2;
    ht_item** buckets = calloc(cap, sizeof(ht_item*));
    if (!table->buckets) {
        goto error;
    }

    for (size_t i = 0; i < table->cap; i++) {
        ht_item* item = table->buckets[i];
        while (item) {
            if (!item->key) {
                ht_item* tmp = item;
                item = item->next;
                free_item(tmp);
                continue;
            }

            size_t b_idx = bucket_index(cap, item->key);
            ht_item* bucket = malloc_item(item->key, item->val);
            if (!bucket) {
                goto error;
            }

            bucket->next = buckets[b_idx];
            buckets[b_idx] = bucket;

            ht_item* tmp = item;
            item = item->next;
            free_item(tmp);
        }
    }
    free(table->buckets);

    table->cap = cap;
    table->buckets = buckets;

    return 0;

error:
    if (buckets) {
        for (size_t i = 0; i < cap; i++) {
            free_bucket(buckets[i]);
        }
        free(buckets);
    }
    return -1;
}

ht_item* ht_set(ht_table* table, const char* key, const size_t val) {
    assert(table != NULL);
    assert(key != NULL);

    if (table->size / (float)table->cap > TABLE_MAX_LOAD) {
        ht_realloc(table);
    }

    size_t b_idx = bucket_index(table->cap, key);
    ht_item* item = find_item(table->buckets[b_idx], key);

    if (!item) {
        ht_item* bucket = malloc_item(key, val);
        if (!bucket) {
            return NULL;
        }

        table->size++;
        bucket->next = table->buckets[b_idx];
        return table->buckets[b_idx] = bucket;
    }

    item->val = val;

    return item;
}

ht_item* ht_get(const ht_table* table, const char* key) {
    assert(table != NULL);
    assert(key != NULL);

    size_t b_idx = bucket_index(table->cap, key);

    return find_item(table->buckets[b_idx], key);
}

size_t ht_size(const ht_table* table) { return table->size; }

void ht_del(const ht_table* table, const char* key) {
    assert(table != NULL);
    assert(key != NULL);

    size_t b_idx = bucket_index(table->cap, key);
    ht_item* item = find_item(table->buckets[b_idx], key);
    if (item) {
        free(item->key);
        item->key = NULL;
    }
}

void ht_each(const ht_table* table, ht_each_fn* fn) {
    assert(table != NULL);
    assert(fn != NULL);

    for (size_t i = 0; i < table->cap; i++) {
        ht_item* bucket = table->buckets[i];
        while (bucket) {
            if (!bucket->key) {
                continue;
            }

            fn(bucket);

            bucket = bucket->next;
        }
    }
}
