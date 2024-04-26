#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hash_table.h"

char* slurp_file(const char* name) {
    FILE* f = fopen(name, "r");
    char* str = NULL;
    if (!f) {
        perror("fopen");
        goto error;
    }

    if (fseek(f, 0, SEEK_END) < 0) {
        perror("fseek");
        goto error;
    }

    int64_t s = ftell(f);
    if (s < 0) {
        perror("ftell");
        goto error;
    }
    rewind(f);

    if (!(str = malloc(s + 1))) {
        perror("malloc");
        goto error;
    }
    if (fread(str, s, s, f) <= 0) {
        perror("fread");
        goto error;
    }

    if (fclose(f) < 0) {
        perror("fclose");
        goto error;
    }

    return str;

error:
    if (f && fclose(f) < 0) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
    free(str);
    return NULL;
}

typedef struct {
    char* src;
    size_t curr;
    size_t start;
    size_t line;
} scanner;

scanner* new_scanner(char* src) {
    assert(src != NULL);

    scanner* s = malloc(sizeof(scanner));
    if (!s) {
        return NULL;
    }
    s->src = src;
    s->curr = 0;
    s->start = 0;
    s->line = 1;

    return s;
}

void free_scanner(scanner* s) {
    free(s);
}

char peek(scanner* s) {
    assert(s != NULL);

    return s->src[s->curr];
}

bool is_at_end(scanner* s) {
    assert(s != NULL);

    return peek(s) == '\0';
}

char advance(scanner* s) {
    assert(s != NULL);

    if (is_at_end(s)) {
        return '\0';
    }

    return s->src[s->curr++];
}

void my_each(ht_item* item) {
    printf("key: \"%s\", val: %zu\n", item->key, item->val);
}

bool repl(ht_table* table) {
    for (;;) {
        char word[256];
        printf(">>> ");
        if (scanf("%255s", word) < 0) {
            return false;
        }
        if (strcmp("::exit", word) == 0) {
            break;
        }

        ht_item* item = ht_get(table, word);
        printf("%zu\n", item ? item->val : 0);
    }
    
    return true;
}

int main(int argc, char** argv) {
    char* str; ht_table* table; scanner* s;
    str = slurp_file("tinyshakespeare.txt");
    if (!str) {
        fprintf(stderr, "slurp_file\n");
        goto error;
    }

    table = ht_malloc();
    if (!table) {
        fprintf(stderr, "ht_malloc\n");
        goto error;
    }
    
    s = new_scanner(str);
    if (!s) {
        fprintf(stderr, "new_scanner\n");
        goto error;
    }

    while (!is_at_end(s)) {
        while (!is_at_end(s) && isspace(peek(s))) {
            if (advance(s) == '\n') s->line++;
        }

        s->start = s->curr;

        while (!is_at_end(s) && !isspace(peek(s))) {
            if (advance(s) == '\n') s->line++;
        }

        if (s->start == s->curr) continue;

        size_t n = s->curr - s->start;
        char key[n + 1]; key[n] = '\0';

        strncpy(key, s->src + s->start, n);
        // printf("curr: %zu, start: %zu, n: %zu, line: %zu, val: %s\n", s->curr, s->start,n, s->line, key);

        ht_item* item = ht_get(table, key);
        ht_set(table, key, item ? item->val + 1 : 1);
    }

    if (argc == 1) {
        bool ok = repl(table);
        if (!ok) {
            goto error;
        }
    } else {
        char* word = argv[1];
        ht_item* item = ht_get(table, word);
        printf("%zu\n", item ? item->val : 0);
    }

    free_scanner(s);
    ht_free(table);
    free(str);
    
    return EXIT_SUCCESS;

error:
    free_scanner(s);
    ht_free(table);
    free(str);
    return EXIT_FAILURE;
}
