#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct puzzle {
    char **grid;
    int rows;
    int columns;
};

int xmas_n(struct puzzle puzzle, int row, int column) {
    int xmas = 0;
    if (row - 1 >= 0 && puzzle.grid[row - 1][column] == 'M' &&
        row - 2 >= 0 && puzzle.grid[row - 2][column] == 'A' &&
        row - 3 >= 0 && puzzle.grid[row - 3][column] == 'S') {
        xmas++;
    }
    return xmas;
}

int xmas_ne(struct puzzle puzzle, int row, int column) {
    int xmas = 0;
    if (row - 1 >= 0 && column + 1 < puzzle.columns && puzzle.grid[row - 1][column + 1] == 'M' &&
        row - 2 >= 0 && column + 2 < puzzle.columns && puzzle.grid[row - 2][column + 2] == 'A' &&
        row - 3 >= 0 && column + 3 < puzzle.columns && puzzle.grid[row - 3][column + 3] == 'S') {
        xmas++;
    }
    return xmas;
}

int xmas_e(struct puzzle puzzle, int row, int column) {
    int xmas = 0;
    if (column + 1 < puzzle.columns && puzzle.grid[row][column + 1] == 'M' &&
        column + 2 < puzzle.columns && puzzle.grid[row][column + 2] == 'A' &&
        column + 3 < puzzle.columns && puzzle.grid[row][column + 3] == 'S') {
        xmas++;
    }
    return xmas;
}

int xmas_se(struct puzzle puzzle, int row, int column) {
    int xmas = 0;
    if (row + 1 < puzzle.rows && column + 1 < puzzle.columns && puzzle.grid[row + 1][column + 1] == 'M' &&
        row + 2 < puzzle.rows && column + 2 < puzzle.columns && puzzle.grid[row + 2][column + 2] == 'A' &&
        row + 3 < puzzle.rows && column + 3 < puzzle.columns && puzzle.grid[row + 3][column + 3] == 'S') {
        xmas++;
    }
    return xmas;
}

int xmas_s(struct puzzle puzzle, int row, int column) {
    int xmas = 0;
    if (row + 1 < puzzle.rows && puzzle.grid[row + 1][column] == 'M' &&
        row + 2 < puzzle.rows && puzzle.grid[row + 2][column] == 'A' &&
        row + 3 < puzzle.rows && puzzle.grid[row + 3][column] == 'S') {
        xmas++;
    }
    return xmas;
}

int xmas_sw(struct puzzle puzzle, int row, int column) {
    int xmas = 0;
    if (row + 1 < puzzle.rows && column - 1 >= 0 && puzzle.grid[row + 1][column - 1] == 'M' &&
        row + 2 < puzzle.rows && column - 2 >= 0 && puzzle.grid[row + 2][column - 2] == 'A' &&
        row + 3 < puzzle.rows && column - 3 >= 0 && puzzle.grid[row + 3][column - 3] == 'S') {
        xmas++;
    }
    return xmas;
}

int xmas_w(struct puzzle puzzle, int row, int column) {
    int xmas = 0;
    if (column - 1 >= 0 && puzzle.grid[row][column - 1] == 'M' &&
        column - 2 >= 0 && puzzle.grid[row][column - 2] == 'A' &&
        column - 3 >= 0 && puzzle.grid[row][column - 3] == 'S') {
        xmas++;
    }
    return xmas;
}

int xmas_nw(struct puzzle puzzle, int row, int column) {
    int xmas = 0;
    if (row - 1 >= 0 && column - 1 >= 0 && puzzle.grid[row - 1][column - 1] == 'M' &&
        row - 2 >= 0 && column - 2 >= 0 && puzzle.grid[row - 2][column - 2] == 'A' &&
        row - 3 >= 0 && column - 3 >= 0 && puzzle.grid[row - 3][column - 3] == 'S') {
        xmas++;
    }
    return xmas;
}

int num_xmas(struct puzzle puzzle, int row, int column) {
    int xmas = 0;
    
    if (puzzle.grid[row][column] == 'X') {

        xmas += xmas_n(puzzle, row, column);
        xmas += xmas_nw(puzzle, row, column);
        xmas += xmas_w(puzzle, row, column);
        xmas += xmas_sw(puzzle, row, column);
        xmas += xmas_s(puzzle, row, column);
        xmas += xmas_se(puzzle, row, column);
        xmas += xmas_e(puzzle, row, column);
        xmas += xmas_ne(puzzle, row, column);
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