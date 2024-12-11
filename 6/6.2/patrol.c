#include <stdio.h>
#include <stdlib.h>

struct patrol_area_map {
    char **map;
    int width;
    int height;
};

int determine_patrol_area_map_width(char *buffer) {
    int patrol_area_width = 0;
    while (buffer[patrol_area_width] != '\n' && buffer[patrol_area_width] != '\0') {
        patrol_area_width++;
    }
    return patrol_area_width;
}

struct patrol_area_map *create_patrol_area_map_from_stdin() {
    char buffer[BUFSIZ];
    struct patrol_area_map *patrol_area_map = (struct patrol_area_map *)malloc(sizeof(struct patrol_area_map));
    patrol_area_map->height = 0;

    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        patrol_area_map->height++;
        patrol_area_map->map = (char **)realloc(patrol_area_map->map, patrol_area_map->height * sizeof(char *));
        
        if (patrol_area_map->width == 0) {
            patrol_area_map->width = determine_patrol_area_map_width(buffer);
        }
        patrol_area_map->map[patrol_area_map->height - 1] = (char *)malloc(patrol_area_map->width * sizeof(char));

        for (int i = 0; i < patrol_area_map->width; i++) {
            patrol_area_map->map[patrol_area_map->height - 1][i] = buffer[i];
        }
    }

    return patrol_area_map;
}

void destroy_patrol_area_map(struct patrol_area_map *patrol_area_map) {
    for (int i = 0; i < patrol_area_map->height; i++) {
        free(patrol_area_map->map[i]);
    }
    free(patrol_area_map->map);
    free(patrol_area_map);
}

void find_start_position_in_patrol_area_map(int *x, int *y, struct patrol_area_map *patrol_area_map) {
    for (*y = 0; *y < patrol_area_map->height; (*y)++) {
        for (*x = 0; *x < patrol_area_map->width; (*x)++) {
            if (patrol_area_map->map[*y][*x] == '^' ||
                patrol_area_map->map[*y][*x] == 'v' ||
                patrol_area_map->map[*y][*x] == '<' ||
                patrol_area_map->map[*y][*x] == '>') {
                return;
            }
        }
    }
}

struct guard;

// Patrol area
//
// A patrol area is a two-dimensional grid of cells.
//
// A patrol area cell acts as a visitor for a guard. This visitor offers a single method interface, that can be called by current state of the guard.
// This method is called "move" and it takes a guard as an argument, and knows.
struct patrol_area_cell {
    int number_of_times_guard_has_moved_north_in_this_cell;
    int number_of_times_guard_has_moved_east_in_this_cell;
    int number_of_times_guard_has_moved_south_in_this_cell;
    int number_of_times_guard_has_moved_west_in_this_cell;
    struct patrol_area_cell *north;
    struct patrol_area_cell *east;
    struct patrol_area_cell *south;
    struct patrol_area_cell *west;
    void (*move_guard_north)(struct guard *);
    void (*move_guard_east)(struct guard *);
    void (*move_guard_south)(struct guard *);
    void (*move_guard_west)(struct guard *);
};

void move_north(struct guard *);
void move_east(struct guard *);
void move_south(struct guard *);
void move_west(struct guard *);
void obstacle_north(struct guard *);
void obstacle_east(struct guard *);
void obstacle_south(struct guard *);
void obstacle_west(struct guard *);
void no_more_moves(struct guard *);

void initialize_move_north(struct patrol_area_cell* cell, struct patrol_area_cell **patrol_area_rows, int x_north, int y_north, struct patrol_area_map *patrol_area_map) {
    cell->number_of_times_guard_has_moved_north_in_this_cell = 0;
    if (y_north < 0) {
        cell->north = NULL;
        cell->move_guard_north = no_more_moves;
    } else if (patrol_area_map->map[y_north][x_north] == '#') {
        cell->north = NULL;
        cell->move_guard_north = obstacle_north;
    } else {
        cell->north = &patrol_area_rows[y_north][x_north];
        cell->move_guard_north = move_north;
    }
}

void initialize_move_east(struct patrol_area_cell* cell, struct patrol_area_cell **patrol_area_rows, int x_east, int y_east, struct patrol_area_map *patrol_area_map) {
    cell->number_of_times_guard_has_moved_east_in_this_cell = 0;
    if (x_east >= patrol_area_map->width) {
        cell->east = NULL;
        cell->move_guard_east = no_more_moves;
    } else if (patrol_area_map->map[y_east][x_east] == '#') {
        cell->east = NULL;
        cell->move_guard_east = obstacle_east;
    } else {
        cell->east = &patrol_area_rows[y_east][x_east];
        cell->move_guard_east = move_east;
    }
}

void initialize_move_south(struct patrol_area_cell* cell, struct patrol_area_cell **patrol_area_rows, int x_south, int y_south, struct patrol_area_map *patrol_area_map) {
    cell->number_of_times_guard_has_moved_south_in_this_cell = 0;
    if (y_south >= patrol_area_map->height) {
        cell->south = NULL;
        cell->move_guard_south = no_more_moves;
    } else if (patrol_area_map->map[y_south][x_south] == '#') {
        cell->south = NULL;
        cell->move_guard_south = obstacle_south;
    } else {
        cell->south = &patrol_area_rows[y_south][x_south];
        cell->move_guard_south = move_south;
    }
}

void initialize_move_west(struct patrol_area_cell* cell, struct patrol_area_cell **patrol_area_rows, int x_west, int y_west, struct patrol_area_map *patrol_area_map) {
    cell->number_of_times_guard_has_moved_west_in_this_cell = 0;
    if (x_west < 0) {
        cell->west = NULL;
        cell->move_guard_west = no_more_moves;
    } else if (patrol_area_map->map[y_west][x_west] == '#') {
        cell->west = NULL;
        cell->move_guard_west = obstacle_west;
    } else {
        cell->west = &patrol_area_rows[y_west][x_west];
        cell->move_guard_west = move_west;
    }
}

void initialize_patrol_area_cell(struct patrol_area_cell *cell, struct patrol_area_cell **patrol_area_rows, int x, int y, struct patrol_area_map *patrol_area_map) {
    if (patrol_area_map->map[y][x] == '#') {
        cell->north = NULL;
        cell->east = NULL;
        cell->south = NULL;
        cell->west = NULL;
        cell->move_guard_north = NULL;
        cell->move_guard_east = NULL;
        cell->move_guard_south = NULL;
        cell->move_guard_west = NULL;
    } else {
        initialize_move_north(cell, patrol_area_rows, x, y - 1, patrol_area_map);
        initialize_move_east(cell, patrol_area_rows, x + 1, y, patrol_area_map);
        initialize_move_south(cell, patrol_area_rows, x, y + 1, patrol_area_map);
        initialize_move_west(cell, patrol_area_rows, x - 1, y, patrol_area_map);
    }
}

struct patrol_area_cell** create_patrol_area_from_map(struct patrol_area_map *patrol_area_map) {
    struct patrol_area_cell **patrol_area_rows = (struct patrol_area_cell**)malloc(patrol_area_map->height * sizeof(struct patrol_area_cell*));
    for (int y = 0; y < patrol_area_map->height; y++) {
        patrol_area_rows[y] = (struct patrol_area_cell*)malloc(patrol_area_map->width * sizeof(struct patrol_area_cell));
        for (int x = 0; x < patrol_area_map->width; x++) {
            initialize_patrol_area_cell(&patrol_area_rows[y][x], patrol_area_rows, x, y, patrol_area_map);
        }
    }
 
    return patrol_area_rows;
}

void destroy_patrol_area(struct patrol_area_cell **patrol_area, int height) {
    for (int y = 0; y < height; y++) {
        free(patrol_area[y]);
    }
    free(patrol_area);
}

void print_patrol_area(struct patrol_area_cell **patrol_area, int width, int height) {

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (patrol_area[y][x].north == NULL && patrol_area[y][x].east == NULL && patrol_area[y][x].south == NULL && patrol_area[y][x].west == NULL) {
                printf("#");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}

// Guard
//
// A guard can move in four directions: up (^), down (v), left (<), and right (>).
//
// So, we need four instances of guard, each moving in a different direction.
//
// A guard has a position in the patrol area, which is a two-dimensional grid of cells.
//
typedef enum {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3,
    END = 4
} Direction;

struct guard {
    Direction direction;
    struct patrol_area_cell *current_cell;
    void (*move)(struct guard *);
};

void move_guard_north(struct guard *guard);
void move_guard_east(struct guard *guard);
void move_guard_south(struct guard *guard);
void move_guard_west(struct guard *guard);

struct guard * moving_guard[] = {
    &(struct guard){NORTH, NULL, move_guard_north},
    &(struct guard){EAST, NULL, move_guard_east},
    &(struct guard){SOUTH, NULL, move_guard_south},
    &(struct guard){WEST, NULL, move_guard_west},
    &(struct guard){END, NULL, NULL}
};

void move_north(struct guard *guard) {
    if (guard);
}

void move_east(struct guard *guard) {
    if (guard);
}

void move_south(struct guard *guard) {
    if (guard);
}

void move_west(struct guard *guard) {
    if (guard);
}

void obstacle_north(struct guard *guard) {
    if (guard);
}

void obstacle_east(struct guard *guard) {
    if (guard);
}

void obstacle_south(struct guard *guard) {
    if (guard);
}

void obstacle_west(struct guard *guard) {
    if (guard);
}

void no_more_moves(struct guard *guard) {
    if (guard);
}

void move_guard_north(struct guard *guard) {
    if (guard);
}

void move_guard_east(struct guard *guard) {
    if (guard);
}

void move_guard_south(struct guard *guard) {
    if (guard);
}

void move_guard_west(struct guard *guard) {
    if (guard);
}

int main() {
    struct patrol_area_map* patrol_area_map;
    struct patrol_area_cell **patrol_area;
    int loops_total = 0;

    patrol_area_map = create_patrol_area_map_from_stdin();

    int x,y;
    find_start_position_in_patrol_area_map(&x, &y, patrol_area_map);

    patrol_area = create_patrol_area_from_map(patrol_area_map);
    print_patrol_area(patrol_area, patrol_area_map->width, patrol_area_map->height);

    printf("Patrol area width: %d\n", patrol_area_map->width);
    printf("Patrol area height: %d\n", patrol_area_map->height);
    printf("Start position: %d, %d\n", x, y);

    printf("Total loops: %d\n", loops_total);

    destroy_patrol_area(patrol_area, patrol_area_map->height);
    destroy_patrol_area_map(patrol_area_map);
}