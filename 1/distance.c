#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIST_INCREMENT (5);

struct list {
    int *data;
    size_t size;
    size_t capacity;
};

struct list *create_list() {
    struct list *list = malloc(sizeof(struct list));
    list->size = 0;
    list->capacity = LIST_INCREMENT;
    list->data = malloc(sizeof(int) * list->capacity);
    return list;
}

void add_to_list(struct list *list, int value) {
    if (list->size == list->capacity) {
        list->capacity += LIST_INCREMENT;
        list->data = realloc(list->data, sizeof(int) * list->capacity);
    }
    list->data[list->size++] = value;
}

void free_list(struct list *list) {
    free(list->data);
    free(list);
}

struct tokens {
    char **tokens;
    size_t size;
    size_t capacity;
};

struct tokens *create_tokens() {
    struct tokens *tokens = malloc(sizeof(struct tokens));
    tokens->size = 0;
    tokens->capacity = LIST_INCREMENT;
    tokens->tokens = malloc(sizeof(char*) * tokens->capacity);
    return tokens;
}

void add_to_tokens(struct tokens *tokens, char *value) {
    if (tokens->size == tokens->capacity) {
        tokens->capacity += LIST_INCREMENT;
        tokens->tokens = realloc(tokens->tokens, sizeof(char*) * tokens->capacity);
    }
    tokens->tokens[tokens->size++] = value;
}

void free_tokens(struct tokens *tokens) {
    for (int i = 0; i < tokens->size; i++) {
        free(tokens->tokens[i]);
    }
    free(tokens->tokens);
    free(tokens);
}

void tokenize(struct tokens *tokens, char *buffer) {
    char *token = strtok(buffer, " \t\n");
    while (token != NULL) {
        add_to_tokens(tokens, strdup(token));
        token = strtok(NULL, " \t\n");
    }
}

static int compare(const void *a, const void *b) {
    return (*(const int*)a) - (*(const int*)b);
}

int get_distance(struct list* a, struct list* b) {
    int distance = 0;
    for (int i = 0; i < a->size; i++) {
        if (a->data[i] > b->data[i]) {
            distance += a->data[i] - b->data[i];
        } else {
            distance += b->data[i] - a->data[i];
        }
    }
    return distance;
}

int main() {
    int total_distance = 0;

    struct list* a = create_list();
    struct list* b = create_list();

    char buffer[BUFSIZ];

    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        struct tokens *tokens = create_tokens();
        tokenize(tokens, buffer);
        add_to_list(a, atoi(tokens->tokens[0]));
        add_to_list(b, atoi(tokens->tokens[1]));
        free_tokens(tokens);
    }

    qsort(a->data, a->size, sizeof(int), compare);
    qsort(b->data, b->size, sizeof(int), compare);

    total_distance = get_distance(a, b);

    printf("Total distance: %d\n", total_distance);

    free_list(b);
    free_list(a);

    return 0;
}