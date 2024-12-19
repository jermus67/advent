#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int blink(const char* stone, int times_to_blink) {
    char buffer[BUFSIZ];
    int len = strlen(stone);
    int number_of_stones = 1;
    if (times_to_blink > 0) {
        if (len == 1 && stone[0] == '0') {
            char *new_stone = "1";
            number_of_stones = blink(new_stone, times_to_blink - 1);
        } else if (len % 2 == 0) {
            char *new_stone = strdup(stone + (len / 2));
            size_t new_stone_len = strlen(new_stone);
            size_t new_stone_value = atol(new_stone);
            snprintf(new_stone, new_stone_len + 1, "%zu", new_stone_value);

            snprintf(buffer, BUFSIZ, "%.*s", len / 2, stone);

            number_of_stones = blink(buffer, times_to_blink - 1);
            number_of_stones += blink(new_stone, times_to_blink - 1);

            free(new_stone);
        } else {
            size_t number = atol(stone);
            number = number * 2024;
            snprintf(buffer, BUFSIZ, "%zu", number);
            number_of_stones = blink(buffer, times_to_blink - 1);
        }
    } else {
        // printf("%s ", stone);
    }

    return number_of_stones;
}

int main() {
    char buffer[BUFSIZ];
    size_t number_of_stones = 0;

    int times_to_blink = 75;

    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        char *token = strtok(buffer, " \t\n");
        while (token != NULL) {
            number_of_stones += blink(token, times_to_blink);
            token = strtok(NULL, " \t\n");
        }
    }

    printf("Number of stones: %zu\n", number_of_stones);
    return 0;
}