#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct garden {
    char** plots;
    int **visited_plots;
    int **visited_fences;
    int rows;
    int columns;
};

struct garden* garden_create() {
    struct garden* garden = malloc(sizeof(struct garden));
    garden->plots = NULL;
    garden->rows = 0;
    garden->columns = 0;

    return garden;
}

void garden_destroy(struct garden* garden) {
    for (int i = 0; i < garden->rows; i++) {
        free(garden->plots[i]);
        free(garden->visited_plots[i]);
    }

    free(garden->plots);
    free(garden->visited_plots);
    free(garden);
}

void garden_print(struct garden* garden) {
    for (int i = 0; i < garden->rows; i++) {
        for (int j = 0; j < garden->columns; j++) {
            printf("%c", garden->plots[i][j]);
        }

        printf("\n");
    }
}

void garden_add_row(struct garden* garden, const char* input) {
    int input_length = strlen(input);
    if (input[input_length - 1] == '\n') {
        input_length--;
    }

    if (garden->columns == 0) {
        garden->columns = (input_length * 2) + 1;
    }

    if (garden->rows == 0) {
        garden->plots = realloc(garden->plots, sizeof(char*) * (garden->rows + 1));
        garden->visited_plots = realloc(garden->visited_plots, sizeof(int*) * (garden->rows + 1));
        garden->visited_fences = realloc(garden->visited_fences, sizeof(int*) * (garden->rows + 1));

        garden->plots[garden->rows] = calloc(garden->columns, sizeof(char));
        memset(garden->plots[garden->rows], ' ', garden->columns);
        garden->visited_plots[garden->rows] = calloc(garden->columns, sizeof(int));
        garden->visited_fences[garden->rows] = calloc(garden->columns, sizeof(int));
        garden->rows++;
    }

    garden->plots = realloc(garden->plots, sizeof(char*) * (garden->rows + 1));
    garden->visited_plots = realloc(garden->visited_plots, sizeof(int*) * (garden->rows + 1));
    garden->visited_fences = realloc(garden->visited_fences, sizeof(int*) * (garden->rows + 1));

    garden->plots[garden->rows] = calloc(garden->columns, sizeof(char));
    memset(garden->plots[garden->rows], ' ', garden->columns);
    garden->visited_plots[garden->rows] = calloc(garden->columns, sizeof(int));
    garden->visited_fences[garden->rows] = calloc(garden->columns, sizeof(int));
    
    for (int i = 0; i < input_length; i++) {
        garden->plots[garden->rows][(i * 2) + 1] = input[i];
    }

    garden->rows++;

    garden->plots = realloc(garden->plots, sizeof(char*) * (garden->rows + 1));
    garden->visited_plots = realloc(garden->visited_plots, sizeof(int*) * (garden->rows + 1));
    garden->visited_fences = realloc(garden->visited_fences, sizeof(int*) * (garden->rows + 1));

    garden->plots[garden->rows] = calloc(garden->columns, sizeof(char));
    memset(garden->plots[garden->rows], ' ', garden->columns);
    garden->visited_plots[garden->rows] = calloc(garden->columns, sizeof(int));
    garden->visited_fences[garden->rows] = calloc(garden->columns, sizeof(int));
    
    garden->rows++;
}

void garden_reset_visited_plots(struct garden* garden) {
    for (int i = 0; i < garden->rows; i++) {
        memset(garden->visited_plots[i], 0, garden->columns * sizeof(int));
    }
}

void garden_reset_visited_fences(struct garden* garden) {
    for (int i = 0; i < garden->rows; i++) {
        memset(garden->visited_fences[i], 0, garden->columns * sizeof(int));
    }
}

void garden_place_fences(struct garden* garden) {

    for (int i = 0; i < garden->rows; i++) {
        for (int j = 0; j < garden->columns; j++) {
            if (isupper(garden->plots[i][j]) && (i - 2 < 0 || garden->plots[i-2][j] != garden->plots[i][j])) {
                garden->plots[i-1][j-1] = '+';
                garden->plots[i-1][j] = '-';
                garden->plots[i-1][j+1] = '+';
            }
            if (isupper(garden->plots[i][j]) && (i + 2 >= garden->rows || garden->plots[i+2][j] != garden->plots[i][j])) {
                garden->plots[i+1][j-1] = '+';
                garden->plots[i+1][j] = '-';
                garden->plots[i+1][j+1] = '+';
            }
            if (isupper(garden->plots[i][j]) && (j - 2 < 0 || garden->plots[i][j-2] != garden->plots[i][j])) {
                garden->plots[i-1][j-1] = '+';
                garden->plots[i][j-1] = '|';
                garden->plots[i+1][j-1] = '+';
            }
            if (isupper(garden->plots[i][j]) && (j + 2 >= garden->columns || garden->plots[i][j+2] != garden->plots[i][j])) {
                garden->plots[i-1][j+1] = '+';
                garden->plots[i][j+1] = '|';
                garden->plots[i+1][j+1] = '+';
            }
        }
    }
}

unsigned long long garden_get_area_size(struct garden* garden, char type, int row, int column) {
    unsigned long long area_size = 0;

    if (row >= 0 && row < garden->rows && column >= 0 && column < garden->columns && !garden->visited_plots[row][column] && garden->plots[row][column] == type) {
        garden->visited_plots[row][column] = 1;
        area_size = 1 + garden_get_area_size(garden, type, row - 2, column) + 
                        garden_get_area_size(garden, type, row + 2, column) +
                        garden_get_area_size(garden, type, row, column - 2) +
                        garden_get_area_size(garden, type, row, column + 2);
    }

    return area_size;
}

unsigned long long garden_get_fences(struct garden* garden, char type, int row, int column) {
    unsigned long long fences = 0;

    if (row >= 0 && row < garden->rows && column >= 0 && column < garden->columns && !garden->visited_fences[row][column] && garden->plots[row][column] == type) {
        garden->visited_fences[row][column] = 1;
        if (garden->plots[row-1][column] == '-') {
            fences++;
        }
        if (garden->plots[row+1][column] == '-') {
            fences++;
        }
        if (garden->plots[row][column-1] == '|') {
            fences++;
        }
        if (garden->plots[row][column+1] == '|') {
            fences++;
        }
        fences += garden_get_fences(garden, type, row - 2, column) + 
                 garden_get_fences(garden, type, row + 2, column) +
                 garden_get_fences(garden, type, row, column - 2) +
                 garden_get_fences(garden, type, row, column + 2);
    }

    return fences;
}

void garden_mark_north_fence_side(struct garden* garden, char type, int row, int column) {
    if (row >= 0 && row < garden->rows && column >= 0 && column < garden->columns &&
        !garden->visited_fences[row][column] && garden->plots[row][column] == '-' && garden->plots[row+1][column] == type) {

        garden->visited_fences[row][column] = 1;

        garden_mark_north_fence_side(garden, type, row, column - 2);
        garden_mark_north_fence_side(garden, type, row, column + 2);
    }
}

void garden_mark_south_fence_side(struct garden* garden, char type, int row, int column) {
    if (row >= 0 && row < garden->rows && column >= 0 && column < garden->columns &&
        !garden->visited_fences[row][column] && garden->plots[row][column] == '-' && garden->plots[row-1][column] == type) {

        garden->visited_fences[row][column] = 1;

        garden_mark_south_fence_side(garden, type, row, column - 2);
        garden_mark_south_fence_side(garden, type, row, column + 2);
    }
}

void garden_mark_west_fence_side(struct garden* garden, char type, int row, int column) {
    if (row >= 0 && row < garden->rows && column >= 0 && column < garden->columns &&
        !garden->visited_fences[row][column] && garden->plots[row][column] == '|' && garden->plots[row][column+1] == type) {

        garden->visited_fences[row][column] = 1;

        garden_mark_west_fence_side(garden, type, row - 2, column);
        garden_mark_west_fence_side(garden, type, row + 2, column);
    }
}

void garden_mark_east_fence_side(struct garden* garden, char type, int row, int column) {
    if (row >= 0 && row < garden->rows && column >= 0 && column < garden->columns &&
        !garden->visited_fences[row][column] && garden->plots[row][column] == '|' && garden->plots[row][column-1] == type) {

        garden->visited_fences[row][column] = 1;

        garden_mark_east_fence_side(garden, type, row - 2, column);
        garden_mark_east_fence_side(garden, type, row + 2, column);
    }
}

unsigned long long garden_get_fence_sides(struct garden* garden, char type, int row, int column) {
    unsigned long long fence_sides = 0;

    if (row >= 0 && row < garden->rows && column >= 0 && column < garden->columns && !garden->visited_fences[row][column] && garden->plots[row][column] == type) {
        garden->visited_fences[row][column] = 1;
        if (garden->plots[row-1][column] == '-' && !garden->visited_fences[row-1][column]) {
            fence_sides++;
            garden_mark_north_fence_side(garden, type, row - 1, column);
        }
        if (garden->plots[row+1][column] == '-' && !garden->visited_fences[row+1][column]) {
            fence_sides++;
            garden_mark_south_fence_side(garden, type, row + 1, column);
        }
        if (garden->plots[row][column-1] == '|' && !garden->visited_fences[row][column-1]) {
            fence_sides++;
            garden_mark_west_fence_side(garden, type, row, column - 1);
        }
        if (garden->plots[row][column+1] == '|' && !garden->visited_fences[row][column+1]) {
            fence_sides++;
            garden_mark_east_fence_side(garden, type, row, column + 1);
        }
        fence_sides += garden_get_fence_sides(garden, type, row - 2, column) + 
                 garden_get_fence_sides(garden, type, row + 2, column) +
                 garden_get_fence_sides(garden, type, row, column - 2) +
                 garden_get_fence_sides(garden, type, row, column + 2);
    }

    return fence_sides;
}

unsigned long long calculate_total_fence_price(struct garden* garden) {
    unsigned long long total_fence_price = 0;

    for (int i = 1; i < garden->rows; i += 2) {
        for (int j = 1; j < garden->columns; j += 2) {
            unsigned long long area_size = garden_get_area_size(garden, garden->plots[i][j], i, j);
            if (area_size > 0) {
                garden_reset_visited_fences(garden);

                unsigned long long fences = garden_get_fences(garden, garden->plots[i][j], i, j);

                printf("Type: %c\n", garden->plots[i][j]);
                printf("Row: %d, Column: %d\n", i, j);
                printf("Area size: %llu\n", area_size);
                printf("Fences: %llu\n", fences);

                total_fence_price += area_size * fences;
            }
        }
    }

    return total_fence_price;
}

unsigned long long calculate_discounted_fence_price(struct garden* garden) {
    unsigned long long total_fence_price = 0;

    for (int i = 1; i < garden->rows; i += 2) {
        for (int j = 1; j < garden->columns; j += 2) {
            unsigned long long area_size = garden_get_area_size(garden, garden->plots[i][j], i, j);
            if (area_size > 0) {
                garden_reset_visited_fences(garden);

                unsigned long long fence_sides = garden_get_fence_sides(garden, garden->plots[i][j], i, j);

                printf("Type: %c\n", garden->plots[i][j]);
                printf("Row: %d, Column: %d\n", i, j);
                printf("Area size: %llu\n", area_size);
                printf("Fences sides: %llu\n", fence_sides);

                total_fence_price += area_size * fence_sides;
            }
        }
    }

    return total_fence_price;
}

int main() {
    unsigned long long total_fence_price = 0;
    unsigned long long totoal_discounted_fence_price = 0;
    struct garden* garden = garden_create();
    char buffer[BUFSIZ];

    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        garden_add_row(garden, buffer);
    }

    garden_print(garden);

    garden_place_fences(garden);

    garden_print(garden);

    garden_reset_visited_plots(garden);
    garden_reset_visited_fences(garden);

    total_fence_price = calculate_total_fence_price(garden);

    garden_reset_visited_plots(garden);
    garden_reset_visited_fences(garden);

    totoal_discounted_fence_price = calculate_discounted_fence_price(garden);

    printf("Total fence price: %llu\n", total_fence_price);
    printf("Total discounted fence price: %llu\n", totoal_discounted_fence_price);

    garden_destroy(garden);

    return 0;
}