#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct stone {
    char *label;
    size_t number;
};

struct stone *stone_create(const char *label, size_t number) {
    struct stone *stone = malloc(sizeof(struct stone));
    stone->label = strdup(label);
    stone->number = number;
    snprintf(stone->label, strlen(stone->label) + 1, "%zu", number);
    return stone;
}

void stone_destroy(struct stone *stone) {
    free(stone->label);
    free(stone);
}

struct stones {
    struct stone **stones;
    size_t size;
};

struct stones *stones_create() {
    struct stones *stones = malloc(sizeof(struct stones));
    stones->stones = NULL;
    stones->size = 0;
    return stones;
}

void stones_destroy(struct stones* stones) {
    free(stones->stones);
    free(stones);
}

void stones_add_stone(struct stones *stones, struct stone* stone) {
    stones->stones = (struct stone **) realloc(stones->stones, (stones->size + 1) * sizeof(struct stone *));
    stones->stones[stones->size] = stone;
    stones->size++;
}

void stones_insert_stone(struct stones *stones, struct stone* stone, size_t index) {
    stones->stones = (struct stone **) realloc(stones->stones, (stones->size + 1) * sizeof(struct stone *));
    for (size_t i = stones->size; i > index; i--) {
        stones->stones[i] = stones->stones[i - 1];
    }
    stones->stones[index] = stone;
    stones->size++;
}

void stones_print(struct stones *stones) {
    for (size_t i = 0; i < stones->size; i++) {
        printf("%s", stones->stones[i]->label);
        if (i < stones->size - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

void blink_zero_numbered_stone(struct stone *stone) {
    stone->number = 1;
    stone->label[0] = '1';
}

void blink_even_length_numbered_stone(struct stone *stone, struct stones *stones, size_t index) {
    size_t len = strlen(stone->label);
    struct stone *new_stone = stone_create(stone->label + (len / 2), atol(stone->label + (len / 2)));
    stones_insert_stone(stones, new_stone, index + 1);
    stone->label[len / 2] = '\0';
    stone->number = atol(stone->label);
}

void blink_odd_length_numbered_stone(struct stone *stone) {
    char buffer[BUFSIZ];
    stone->number = stone->number * 2024;
    snprintf(buffer, BUFSIZ, "%zu", stone->number);
    free(stone->label);
    stone->label = strdup(buffer);
}

void blink(struct stones *stones) {
    for (size_t i = 0; i < stones->size; i++) {
        if (stones->stones[i]->number == 0) {
            blink_zero_numbered_stone(stones->stones[i]);
        } else if (strlen(stones->stones[i]->label) % 2 == 0) {
            blink_even_length_numbered_stone(stones->stones[i], stones, i);
            i++;
        } else {
            blink_odd_length_numbered_stone(stones->stones[i]);
        }
    }
}

int main() {
    char buffer[BUFSIZ];
    struct stone *stone;
    struct stones *stones = stones_create();

    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        char *token = strtok(buffer, " \t\n");
        while (token != NULL) {
            stone = stone_create(token, atol(token));
            stones_add_stone(stones, stone);
            token = strtok(NULL, " \t\n");
        }
    }

    for (int i = 0; i < 25; i++) {
        printf("Blink %d\n", i + 1);
        blink(stones);
        //stones_print(stones);
    }

    printf("Number of stones: %zu\n", stones->size);
    stones_destroy(stones);
    return 0;
}