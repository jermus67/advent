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

void remove_index_from_list(struct list *list, int index) {
    for (int i = index; i < list->size - 1; i++) {
        list->data[i] = list->data[i + 1];
    }
    list->size--;
}

struct list* duplicate_list(struct list *list) {
    struct list *new_list = create_list();
    for (int i = 0; i < list->size; i++) {
        add_to_list(new_list, list->data[i]);
    }
    return new_list;
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

void add_tokens_to_list(struct list *list, struct tokens *tokens) {
    for (int i = 0; i < tokens->size; i++) {
        add_to_list(list, atoi(tokens->tokens[i]));
    }
}

int is_level_safe_increase(int level_a, int level_b) {
    int diff = level_b - level_a;
    return (level_a < level_b) &&  (diff >= 1) && (diff <= 3);
}

int is_level_safe_decrease(int level_a, int level_b) {
    int diff = level_a - level_b;
    return (level_a > level_b) && (diff >= 1) && (diff <= 3);
}

int is_report_safe(struct list* report) {
    int report_safe = 1;

    int increasing = report->data[0] < report->data[1];

    for (int i = 0; i < report->size - 1; i++) {
        if (increasing) {
            report_safe = report_safe && is_level_safe_increase(report->data[i], report->data[i + 1]);
        } else {
            report_safe = report_safe && is_level_safe_decrease(report->data[i], report->data[i + 1]);
        }
    }

    return report_safe;
}

int is_dampened_report_safe(struct list* report) {
    int report_safe = is_report_safe(report);

    for (int i = 0; !report_safe && i < report->size; i++) {
        struct list * dampened_report = duplicate_list(report);
        remove_index_from_list(dampened_report, i);
        report_safe = is_report_safe(dampened_report);
    }

    return report_safe;
}

int main() {
    int total_safe_reports = 0;

    char buffer[BUFSIZ];

    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        struct tokens *tokens = create_tokens();
        struct list* report = create_list();
        tokenize(tokens, buffer);
        add_tokens_to_list(report, tokens);
        if (is_dampened_report_safe(report)) {
            total_safe_reports++;
        }
        free_list(report);
        free_tokens(tokens);
    }

    printf("Total safe reports: %d\n", total_safe_reports);

    return 0;
}