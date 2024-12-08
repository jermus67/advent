#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct puzzle {
    char **grid;
    int rows;
    int columns;
};

int masses_left(struct puzzle puzzle, int row, int column) {
    int masses = 0;
    if (column - 1 >= 0 && row -1 >= 0 && puzzle.grid[row-1][column - 1] == 'M' &&
        column - 1 >= 0 && row + 1 < puzzle.rows && puzzle.grid[row+1][column - 1] == 'M' &&
        column + 1 < puzzle.columns && row - 1 >= 0 && puzzle.grid[row-1][column + 1] == 'S' &&
        column + 1 < puzzle.columns && row + 1 < puzzle.rows && puzzle.grid[row+1][column + 1] == 'S') {
        masses++;
    }
    return masses;
}

int masses_right(struct puzzle puzzle, int row, int column) {
    int masses = 0;
    if (column + 1 < puzzle.columns && row -1 >= 0 && puzzle.grid[row-1][column + 1] == 'M' &&
        column + 1 < puzzle.columns && row + 1 < puzzle.rows && puzzle.grid[row+1][column + 1] == 'M' &&
        column - 1 >= 0 && row - 1 >= 0 && puzzle.grid[row-1][column - 1] == 'S' &&
        column - 1 >= 0 && row + 1 < puzzle.rows && puzzle.grid[row+1][column - 1] == 'S') {
        masses++;
    }
    return masses;
}

int masses_up(struct puzzle puzzle, int row, int column) {
    int masses = 0;
    if (row - 1 >= 0 && column - 1 >= 0 && puzzle.grid[row - 1][column - 1] == 'M' &&
        row - 1 >= 0 && column + 1 < puzzle.columns && puzzle.grid[row - 1][column + 1] == 'M' &&
        row + 1 < puzzle.rows && column - 1 >= 0 && puzzle.grid[row + 1][column - 1] == 'S' &&
        row + 1 < puzzle.rows && column + 1 < puzzle.columns && puzzle.grid[row + 1][column + 1] == 'S') {
        masses++;
    }
    return masses;
}

int masses_down(struct puzzle puzzle, int row, int column) {
    int masses = 0;
    if (row + 1 < puzzle.rows && column - 1 >= 0 && puzzle.grid[row + 1][column - 1] == 'M' &&
        row + 1 < puzzle.rows && column + 1 < puzzle.columns && puzzle.grid[row + 1][column + 1] == 'M' &&
        row - 1 >= 0 && column - 1 >= 0 && puzzle.grid[row - 1][column - 1] == 'S' &&
        row - 1 >= 0 && column + 1 < puzzle.columns && puzzle.grid[row - 1][column + 1] == 'S') {
        masses++;
    }
    return masses;
}

int num_xmas(struct puzzle puzzle, int row, int column) {
    int xmas = 0;
    
    if (puzzle.grid[row][column] == 'A') {
        xmas += masses_left(puzzle, row, column);
        xmas += masses_right(puzzle, row, column);
        xmas += masses_up(puzzle, row, column);
        xmas += masses_down(puzzle, row, column);
    }

    return xmas;
}

int main() {
    int total_xmas = 0;
    char buffer[BUFSIZ];

    struct puzzle puzzle = {NULL, 0, 0};
    
    int rows = 0;
    int columns = 0;
    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        columns = strlen(buffer);
        puzzle.grid = realloc(puzzle.grid, (rows+1) * sizeof(char *));
        puzzle.grid[rows] = malloc(columns * sizeof(char));
        for (int i = 0; i < columns; i++) {
            puzzle.grid[rows][i] = buffer[i];
        }
        rows++;
    }
    puzzle.rows = rows;
    puzzle.columns = columns;

    for (rows = 0; rows < puzzle.rows; rows++) {
        for (columns = 0; columns < puzzle.columns; columns++) {
            total_xmas += num_xmas(puzzle, rows, columns);
        }
    }

    printf("Total number of XMAS = %d\n", total_xmas);
}