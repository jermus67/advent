#include <stdio.h>
#include <stdlib.h>

void find_start_position(int *x, int *y, char **patrol_area, int patrol_area_width, int patrol_area_height) {
    for (*y = 0; *y < patrol_area_height; (*y)++) {
        for (*x = 0; *x < patrol_area_width; (*x)++) {
            if (patrol_area[*y][*x] == '^' || patrol_area[*y][*x] == 'v' || patrol_area[*y][*x] == '<' || patrol_area[*y][*x] == '>') {
                return;
            }
        }
    }
}

void traverse_patrol_area(int *x, int *y, char **patrol_area, int patrol_area_width, int patrol_area_height, int *positions_total) {
    
    char direction = patrol_area[*y][*x];

    while (*x >= 0 && *x < patrol_area_width && *y >= 0 && *y < patrol_area_height) {
        if (patrol_area[*y][*x] != 'X') {
            patrol_area[*y][*x] = 'X';
            *positions_total = *positions_total + 1;
        }

        if (direction == '^') {
            *y = *y - 1;
            if (*y >= 0) {
                if (patrol_area[*y][*x] == '#') {
                    direction = '>';
                    *y = *y + 1;
                    *x = *x + 1;
                }
            }
        } else if (direction == 'v') {
            *y = *y + 1;
            if (*y < patrol_area_height) {
                if (patrol_area[*y][*x] == '#') {
                    direction = '<';
                    *y = *y - 1;
                    *x = *x - 1;
                }
            }
        } else if (direction == '<') {
            *x = *x - 1;
            if (*x >= 0) {
                if (patrol_area[*y][*x] == '#') {
                    direction = '^';
                    *x = *x + 1;
                    *y = *y - 1;
                }
            }
        } else if (direction == '>') {
            *x = *x + 1;
            if (*x < patrol_area_width) {
                if (patrol_area[*y][*x] == '#') {
                    direction = 'v';
                    *x = *x - 1;
                    *y = *y + 1;
                }
            }
        }
    }
}

int main() {
    char buffer[BUFSIZ];
    char **patrol_area = NULL;
    int patrol_area_width = 0;
    int patrol_area_height = 0;
    int positions_total = 0;

    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        patrol_area_height++;
        patrol_area = (char **)realloc(patrol_area, patrol_area_height * sizeof(char *));
        
        patrol_area_width = 0;
        while(buffer[patrol_area_width] != '\n' && buffer[patrol_area_width] != '\0') {
            patrol_area_width++;
        }

        patrol_area[patrol_area_height - 1] = (char *)malloc(patrol_area_width * sizeof(char));

        for (int i = 0; i < patrol_area_width; i++) {
            patrol_area[patrol_area_height - 1][i] = buffer[i];
        }
    }

    int x,y;
    find_start_position(&x, &y, patrol_area, patrol_area_width, patrol_area_height);

    traverse_patrol_area(&x, &y, patrol_area, patrol_area_width, patrol_area_height, &positions_total);

    printf("Positions total: %d\n", positions_total);
}