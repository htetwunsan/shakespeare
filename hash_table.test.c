#include <stdio.h>
#include <stdlib.h>

#include "hash_table.h"

void print_table(const ht_table* table) {
    printf("size: %zu, cap: %zu\n", table->size, table->cap);
    for (size_t i = 0; i < table->cap; i++) {
        ht_item* item = table->buckets[i];
        while (item) {
            printf("bucket: %zu, key: %s, val: %zu\n", i, item->key, item->val);
            item = item->next;
        }
    }
}

void test_ht_set(ht_table* table, const char* key, const size_t val) {
    printf("\n");
    printf("test_ht_set\n");
    printf("---before---\n");
    print_table(table); 
    printf("---before---\n");

    ht_set(table, key, val);

    printf("---after---\n");
    print_table(table);
    printf("---after---\n");
    printf("\n");
}

void test_ht_get(const ht_table* table, const char* key) {
    printf("\n");
    printf("test_ht_get\n");
    print_table(table);
    ht_item* item = ht_get(table, key);
    if (item) {
        printf("key: %s, val: %zu\n", key, item->val);
    } else {
        printf("key: %s not found\n", key);
    }
    printf("\n");
}

void test_ht_del(const ht_table* table, const char* key) {
    printf("\n");
    printf("test_ht_del\n");
    printf("---before---\n");
    print_table(table); 
    printf("---before---\n");

    ht_del(table, key);

    printf("---after---\n");
    print_table(table);
    printf("---after---\n");
    printf("\n");

    printf("\n");
}

int main() {
    ht_table* table = ht_malloc();

    test_ht_get(table, "one");
    test_ht_del(table, "one");
    test_ht_set(table, "one", 1);
    test_ht_set(table, "one", 11);
    test_ht_get(table, "one");
    test_ht_del(table, "one");

    ht_set(table, "one", 1);
    ht_set(table, "two", 2);
    ht_set(table, "three", 3);
    ht_set(table, "four", 4);
    ht_set(table, "five", 5);
    ht_set(table, "six", 6);
    ht_set(table, "seven", 7);
    ht_set(table, "eight", 8);
    ht_set(table, "nine", 9);
    ht_set(table, "ten", 10);

    test_ht_get(table, "one");
    test_ht_get(table, "two");
    test_ht_get(table, "three");
    test_ht_get(table, "four");
    test_ht_get(table, "five");
    test_ht_get(table, "six");
    test_ht_get(table, "seven");
    test_ht_get(table, "eight");
    test_ht_get(table, "nine");
    test_ht_get(table, "ten");

    ht_free(table);

    return 0;
}
