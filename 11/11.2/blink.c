#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct blink_key_value {
    unsigned long long stone;
    int times_to_blink;
    unsigned long long number_of_stones;
    struct blink_key_value *next;
}; 

struct hash_table {
    struct blink_key_value **table;
    size_t size;
    size_t capacity;
};

struct hash_table *create_hash_table(size_t capacity) {
    struct hash_table *hash_table = malloc(sizeof(struct hash_table));
    hash_table->table = malloc(sizeof(struct blink_key_value *) * capacity);
    hash_table->size = 0;
    hash_table->capacity = capacity;

    for (size_t i = 0; i < capacity; i++) {
        hash_table->table[i] = NULL;
    }

    return hash_table;
}

size_t hash(struct blink_key_value* key, size_t capacity) {
    unsigned long long hash = 7;

    hash = (hash * 31) + key->stone;
    hash = (hash * 31) + key->times_to_blink;

    return hash % capacity;
}

void hash_table_insert(struct hash_table *hash_table, unsigned long long stone, int times_to_blink, unsigned long long number_of_stones) {
    struct blink_key_value *key_value = malloc(sizeof(struct blink_key_value));
    key_value->stone = stone;
    key_value->times_to_blink = times_to_blink;
    key_value->number_of_stones = number_of_stones;

    size_t index = hash(key_value, hash_table->capacity);

    key_value->next = hash_table->table[index];
    hash_table->table[index] = key_value;
    hash_table->size++;
}

struct blink_key_value *hash_table_search(struct hash_table *hash_table, unsigned long long stone, int times_to_blink) {
    struct blink_key_value key = {stone, times_to_blink, 0};
    size_t index = hash(&key, hash_table->capacity);
    struct blink_key_value *key_value = hash_table->table[index];

    while (key_value != NULL) {
        if (key_value->stone == stone && key_value->times_to_blink == times_to_blink) {
            return key_value;
        }
        key_value = key_value->next;
    }

    return NULL;
}

struct hash_table *hash_table_delete(struct hash_table *hash_table, unsigned long long stone, int times_to_blink) {
    struct blink_key_value key = {stone, times_to_blink, 0};
    size_t index = hash(&key, hash_table->capacity);
    struct blink_key_value *key_value = hash_table->table[index];
    struct blink_key_value *prev = NULL;

    while (key_value != NULL) {
        if (key_value->stone == stone && key_value->times_to_blink == times_to_blink) {
            if (prev == NULL) {
                hash_table->table[index] = key_value->next;
            } else {
                prev->next = key_value->next;
            }

            free((char *) key_value->stone);
            free(key_value);
            hash_table->size--;
            return hash_table;
        }

        prev = key_value;
        key_value = key_value->next;
    }

    return hash_table;
}

unsigned long long blink(unsigned long long stone, int times_to_blink, struct hash_table *hash_table) {
    unsigned long long number_of_stones = 0;

    printf("Blinking %llu %d times\n", stone, times_to_blink);

    struct blink_key_value *key_value = hash_table_search(hash_table, stone, times_to_blink);
    if (key_value != NULL) {
        printf("Found %llu %d in hash table (%llu)\n", key_value->stone, key_value->times_to_blink, key_value->number_of_stones);
        number_of_stones = key_value->number_of_stones;
    } else if (times_to_blink == 0) {
        number_of_stones = 1;
        hash_table_insert(hash_table, stone, times_to_blink, 1);
    } else {
        char number[BUFSIZ];
        snprintf(number, BUFSIZ, "%llu", stone);
        size_t len = strlen(number);
        if (len % 2 == 0) {
            unsigned long long stone_right = strtoull(number + len / 2, NULL, 10);
            number[len / 2] = '\0';
            unsigned long long stone_left = strtoull(number, NULL, 10);

            unsigned long long stones_left = blink(stone_left, times_to_blink - 1, hash_table);
            unsigned long long stones_right = blink(stone_right, times_to_blink - 1, hash_table);

            number_of_stones = stones_left + stones_right;
            hash_table_insert(hash_table, stone, times_to_blink, number_of_stones);
        } else if (stone == 0) {
            number_of_stones = blink(1, times_to_blink - 1, hash_table);
            hash_table_insert(hash_table, stone, times_to_blink, number_of_stones);
        } else {
            number_of_stones = blink(stone * 2024, times_to_blink - 1, hash_table);
            hash_table_insert(hash_table, stone, times_to_blink, number_of_stones);
        }
    }
    
    return number_of_stones;
}

int main() {
    char buffer[BUFSIZ];
    unsigned long long number_of_stones = 0;
    struct hash_table *hash_table = create_hash_table(1000000);

    int times_to_blink = 75;

    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        char *token = strtok(buffer, " \t\n");
        while (token != NULL) {
            number_of_stones += blink(strtoull(token,NULL, 10), times_to_blink, hash_table);
            token = strtok(NULL, " \t\n");
        }
    }

    printf("Number of stones: %zu\n", number_of_stones);

    return 0;
}