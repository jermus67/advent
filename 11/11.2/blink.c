#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char current_label[BUFSIZ];
char new_label[BUFSIZ];

int blink(size_t stone, int times_to_blink) {
    size_t number_of_stones = 0;

    if (times_to_blink == 0) {
        number_of_stones = 1;
    } else {
        snprintf(current_label, BUFSIZ, "%zu", stone);
        size_t len = strlen(current_label);
        if (len % 2 == 0) {
            snprintf(new_label, BUFSIZ, "%zu", atol(current_label + (len / 2)));
            current_label[len / 2] = '\0';

            size_t left_value = atol(current_label);
            size_t right_value = atol(new_label);

            size_t stones_left = blink(left_value, times_to_blink - 1);
            size_t stones_right = blink(right_value, times_to_blink - 1);

            number_of_stones = stones_left + stones_right;
        } else if (len == 1 && stone == 0) {
            number_of_stones = blink(1, times_to_blink - 1);
        } else {
            number_of_stones = blink(stone * 2024, times_to_blink - 1);
        }
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
            size_t number = atol(token);
            number_of_stones += blink(number, times_to_blink);
            token = strtok(NULL, " \t\n");
        }
    }

    printf("Number of stones: %zu\n", number_of_stones);

    return 0;
}