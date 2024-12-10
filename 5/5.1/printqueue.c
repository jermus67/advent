#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 10

// page ordering rule
struct page_ordering_rule {
    int before_page_number;
    int after_page_number;
};

struct page_ordering_rule* page_ordering_rule_create(int before_page_number, int after_page_number) {
    struct page_ordering_rule* rule = malloc(sizeof(struct page_ordering_rule));
    rule->before_page_number = before_page_number;
    rule->after_page_number = after_page_number;
    return rule;
}

void page_ordering_rule_destroy(struct page_ordering_rule* rule) {
    free(rule);
}

// page ordering rule sequence
struct page_ordering_rule_sequence {
    struct page_ordering_rule** rules;
    size_t size;
    size_t capacity;
};

struct page_ordering_rule_sequence* page_ordering_rule_sequence_create() {
    struct page_ordering_rule_sequence* sequence = malloc(sizeof(struct page_ordering_rule_sequence));
    sequence->rules = NULL;
    sequence->size = 0;
    sequence->capacity = 0;
    return sequence;
}

void page_ordering_rule_sequence_destroy(struct page_ordering_rule_sequence* sequence) {
    for (size_t i = 0; i < sequence->size; i++) {
        page_ordering_rule_destroy(sequence->rules[i]);
    }
    free(sequence->rules);
    free(sequence);
}

void page_ordering_rule_sequence_add(struct page_ordering_rule_sequence* sequence, struct page_ordering_rule* rule) {
    if (sequence->size == sequence->capacity) {
        sequence->capacity = sequence->capacity == 0 ? INITIAL_CAPACITY : sequence->capacity * 2;
        sequence->rules = realloc(sequence->rules, sequence->capacity * sizeof(struct page_ordering_rule));
    }
    sequence->rules[sequence->size++] = rule;
}

// page number sequence
struct page_number_sequence {
    int * page_numbers;
    size_t size;
    size_t capacity;
};

struct page_number_sequence* page_number_sequence_create() {
    struct page_number_sequence* sequence = malloc(sizeof(struct page_number_sequence));
    sequence->page_numbers = NULL;
    sequence->size = 0;
    sequence->capacity = 0;
    return sequence;
}

void page_number_sequence_destroy(struct page_number_sequence* sequence) {
    free(sequence->page_numbers);
    free(sequence);
}

void page_number_sequence_add(struct page_number_sequence* sequence, int page_number) {
    if (sequence->size == sequence->capacity) {
        sequence->capacity = sequence->capacity == 0 ? INITIAL_CAPACITY : sequence->capacity * 2;
        sequence->page_numbers = realloc(sequence->page_numbers, sequence->capacity * sizeof(int));
    }
    sequence->page_numbers[sequence->size++] = page_number;
}

// update
struct update {
    struct page_number_sequence* page_numbers;
    int valid;
    int middle_page;
};

struct update* update_create() {
    struct update* update = malloc(sizeof(struct update));
    update->page_numbers = page_number_sequence_create();
    update->valid = 1;
    update->middle_page = 0;
    return update;
}

void update_destroy(struct update* update) {
    page_number_sequence_destroy(update->page_numbers);
    free(update);
}

// update sequence
struct update_sequence {
    struct update** updates;
    size_t size;
    size_t capacity;
};

struct update_sequence* update_sequence_create() {
    struct update_sequence* sequence = malloc(sizeof(struct update_sequence));
    sequence->updates = NULL;
    sequence->size = 0;
    sequence->capacity = 0;
    return sequence;
}

void update_sequence_destroy(struct update_sequence* sequence) {
    for (size_t i = 0; i < sequence->size; i++) {
        update_destroy(sequence->updates[i]);
    }
    free(sequence->updates);
    free(sequence);
}

void update_sequence_add(struct update_sequence* sequence, struct update* update) {
    if (sequence->size == sequence->capacity) {
        sequence->capacity = sequence->capacity == 0 ? INITIAL_CAPACITY : sequence->capacity * 2;
        sequence->updates = realloc(sequence->updates, sequence->capacity * sizeof(struct update));
    }
    sequence->updates[sequence->size++] = update;
}

// print queue
struct print_queue {
    struct page_ordering_rule_sequence* rules;
    struct update_sequence* updates;
};

struct print_queue* print_queue_create() {
    struct print_queue* queue = malloc(sizeof(struct print_queue));
    queue->rules = page_ordering_rule_sequence_create();
    queue->updates = update_sequence_create();
    return queue;
}

void print_queue_destroy(struct print_queue* queue) {
    page_ordering_rule_sequence_destroy(queue->rules);
    update_sequence_destroy(queue->updates);
    free(queue);
}

void read_input(struct print_queue* print_queue) {
    char buffer[BUFSIZ];
    while (fgets(buffer, BUFSIZ, stdin) != NULL && buffer[0] != '\n') {
        int before_page_number, after_page_number;
        if (sscanf(buffer, "%d|%d", &before_page_number, &after_page_number) == 2) {
            struct page_ordering_rule* rule = page_ordering_rule_create(before_page_number, after_page_number);
            page_ordering_rule_sequence_add(print_queue->rules, rule);
        }
    }
    
    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        struct update* update = update_create();
        char* token = strtok(buffer, ",");
        while (token != NULL) {
            int page_number;
            if (sscanf(token, "%d", &page_number) == 1) {
                page_number_sequence_add(update->page_numbers, page_number);
            }
            token = strtok(NULL, ",");
        }
        update_sequence_add(print_queue->updates, update);
    }
}

void validate_rule(struct update* update, struct page_ordering_rule* rule) {
    int before_page_number = rule->before_page_number;
    int after_page_number = rule->after_page_number;
    int before_page_number_index = -1;
    int after_page_number_index = -1;
    for (size_t i = 0; i < update->page_numbers->size; i++) {
        if (update->page_numbers->page_numbers[i] == before_page_number) {
            before_page_number_index = i;
        }
        if (update->page_numbers->page_numbers[i] == after_page_number) {
            after_page_number_index = i;
        }
    }
    if (before_page_number_index != -1 && after_page_number_index != -1) {
        if (before_page_number_index > after_page_number_index) {
            update->valid = 0;
        }
    }
}

void validate_rules(struct update* update, struct page_ordering_rule_sequence* rules) {
    for (size_t i = 0; i < rules->size; i++) {
        validate_rule(update, rules->rules[i]);
        if (!update->valid) {
            break;
        }   
    }
}

void validate_updates(struct print_queue* print_queue) {
    for (size_t i = 0; i < print_queue->updates->size; i++) {
        struct update* update = print_queue->updates->updates[i];
        validate_rules(update, print_queue->rules);
        update->middle_page = update->page_numbers->page_numbers[update->page_numbers->size / 2];
    }
}
int main() {
    int total_middle_pages = 0;
    int total_reordered_middle_pages = 0;

    struct print_queue* print_queue = print_queue_create();

    read_input(print_queue);

    validate_updates(print_queue);

    for (size_t i = 0; i < print_queue->updates->size; i++) {
        struct update* update = print_queue->updates->updates[i];
        if (update->valid) {
            for (size_t j = 0; j < update->page_numbers->size; j++) {
                printf("%d", update->page_numbers->page_numbers[j]);
                if (j < update->page_numbers->size - 1) {
                    printf(",");
                }
            }
            printf(" -> %d\n", update->middle_page);
            total_middle_pages += update->middle_page;
        }
    }

    printf("Total middle pages: %d\n", total_middle_pages);

    print_queue_destroy(print_queue);
}