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
struct patrol_area_cell {
    int start_position;
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

struct patrol_area {
    struct patrol_area_cell **cells;
    int width;
    int height;
};

void move_north(struct guard *);
void move_east(struct guard *);
void move_south(struct guard *);
void move_west(struct guard *);
void obstacle_north(struct guard *);
void obstacle_east(struct guard *);
void obstacle_south(struct guard *);
void obstacle_west(struct guard *);
void no_more_moves_north(struct guard *);
void no_more_moves_east(struct guard *);
void no_more_moves_south(struct guard *);
void no_more_moves_west(struct guard *);

void initialize_move_north(struct patrol_area_cell* cell, struct patrol_area *patrol_area, int y_north, int x_north, struct patrol_area_map *patrol_area_map) {
    cell->number_of_times_guard_has_moved_north_in_this_cell = 0;
    if (y_north < 0) {
        cell->north = NULL;
        cell->move_guard_north = no_more_moves_north;
    } else if (patrol_area_map->map[y_north][x_north] == '#') {
        cell->north = NULL;
        cell->move_guard_north = obstacle_north;
    } else {
        cell->north = &patrol_area->cells[y_north][x_north];
        cell->move_guard_north = move_north;
    }
}

void initialize_move_east(struct patrol_area_cell* cell, struct patrol_area *patrol_area, int y_east, int x_east, struct patrol_area_map *patrol_area_map) {
    cell->number_of_times_guard_has_moved_east_in_this_cell = 0;
    if (x_east >= patrol_area_map->width) {
        cell->east = NULL;
        cell->move_guard_east = no_more_moves_east;
    } else if (patrol_area_map->map[y_east][x_east] == '#') {
        cell->east = NULL;
        cell->move_guard_east = obstacle_east;
    } else {
        cell->east = &patrol_area->cells[y_east][x_east];
        cell->move_guard_east = move_east;
    }
}

void initialize_move_west(struct patrol_area_cell* cell, struct patrol_area *patrol_area, int y_west, int x_west, struct patrol_area_map *patrol_area_map) {
    cell->number_of_times_guard_has_moved_west_in_this_cell = 0;
    if (x_west < 0) {
        cell->west = NULL;
        cell->move_guard_west = no_more_moves_west;
    } else if (patrol_area_map->map[y_west][x_west] == '#') {
        cell->west = NULL;
        cell->move_guard_west = obstacle_west;
    } else {
        cell->west = &patrol_area->cells[y_west][x_west];
        cell->move_guard_west = move_west;
    }
}

void initialize_move_south(struct patrol_area_cell* cell, struct patrol_area *patrol_area, int y_south, int x_south, struct patrol_area_map *patrol_area_map) {
    cell->number_of_times_guard_has_moved_south_in_this_cell = 0;
    if (y_south >= patrol_area_map->height) {
        cell->south = NULL;
        cell->move_guard_south = no_more_moves_south;
    } else if (patrol_area_map->map[y_south][x_south] == '#') {
        cell->south = NULL;
        cell->move_guard_south = obstacle_south;
    } else {
        cell->south = &patrol_area->cells[y_south][x_south];
        cell->move_guard_south = move_south;
    }
}

void initialize_patrol_area_cell(struct patrol_area_cell *cell, struct patrol_area *patrol_area, int y, int x, struct patrol_area_map *patrol_area_map) {
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
        cell->start_position = 0;
        initialize_move_north(cell, patrol_area, y - 1, x, patrol_area_map);
        initialize_move_south(cell, patrol_area, y + 1, x, patrol_area_map);
        initialize_move_east(cell, patrol_area, y, x + 1, patrol_area_map);
        initialize_move_west(cell, patrol_area, y, x - 1, patrol_area_map);
    }
}

struct patrol_area* create_patrol_area_from_map(struct patrol_area_map *patrol_area_map, int x_initial, int y_initial) {
    struct patrol_area *patrol_area = (struct patrol_area*)malloc(sizeof(struct patrol_area));
    patrol_area->width = patrol_area_map->width;
    patrol_area->height = patrol_area_map->height;
    patrol_area->cells = (struct patrol_area_cell**)malloc(patrol_area->height * sizeof(struct patrol_area_cell*));
    for (int y = 0; y < patrol_area->height; y++) {
        patrol_area->cells[y] = (struct patrol_area_cell*)malloc(patrol_area->width * sizeof(struct patrol_area_cell));
    }

    for (int y = 0; y < patrol_area->height; y++) {
        for (int x = 0; x < patrol_area->width; x++) {
            initialize_patrol_area_cell(&patrol_area->cells[y][x], patrol_area, y, x, patrol_area_map);
        }
    }

    patrol_area->cells[y_initial][x_initial].start_position = 1;

    return patrol_area;
}

void destroy_patrol_area(struct patrol_area *patrol_area) {
    for (int y = 0; y < patrol_area->height; y++) {
        free(patrol_area->cells[y]);
    }
    free(patrol_area->cells);
    free(patrol_area);
}

int is_patrol_area_cell_start_position(struct patrol_area_cell *cell) {
    return cell->start_position;
}

int is_patrol_area_cell_obstacle(struct patrol_area_cell *cell) {
    return cell->north == NULL && cell->east == NULL && cell->south == NULL && cell->west == NULL;
}

int is_patrol_area_cell_north_south(struct patrol_area_cell *cell) {
    return (cell->number_of_times_guard_has_moved_north_in_this_cell > 0 || cell->number_of_times_guard_has_moved_south_in_this_cell > 0) &&
            cell->number_of_times_guard_has_moved_east_in_this_cell == 0 && cell->number_of_times_guard_has_moved_west_in_this_cell == 0;
}

int is_patrol_area_cell_east_west(struct patrol_area_cell *cell) {
    return (cell->number_of_times_guard_has_moved_east_in_this_cell > 0 || cell->number_of_times_guard_has_moved_west_in_this_cell > 0) &&
            cell->number_of_times_guard_has_moved_north_in_this_cell == 0 && cell->number_of_times_guard_has_moved_south_in_this_cell == 0;
}

int is_patrol_area_north_south_east_west(struct patrol_area_cell *cell) {
    return (cell->number_of_times_guard_has_moved_north_in_this_cell > 0 || cell->number_of_times_guard_has_moved_south_in_this_cell > 0) &&
            (cell->number_of_times_guard_has_moved_east_in_this_cell > 0 || cell->number_of_times_guard_has_moved_west_in_this_cell > 0);
}

void print_patrol_area(struct patrol_area *patrol_area, int width, int height) {

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (is_patrol_area_cell_obstacle(&patrol_area->cells[y][x])) {
                printf("#");
            } else if (is_patrol_area_cell_start_position(&patrol_area->cells[y][x])) {
                printf("^");
            } else if (is_patrol_area_cell_north_south(&patrol_area->cells[y][x])) {
                printf("|");
            } else if (is_patrol_area_cell_east_west(&patrol_area->cells[y][x])) {
                printf("-");
            } else if (is_patrol_area_north_south_east_west(&patrol_area->cells[y][x])) {
                printf("+");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}

// Guard
typedef enum {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3,
    NO_MORE_MOVES = 4
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

void move_north(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_north_in_this_cell++;
    guard->current_cell = guard->current_cell->north;
}

void move_east(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_east_in_this_cell++;
    guard->current_cell = guard->current_cell->east;
}

void move_south(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_south_in_this_cell++;
    guard->current_cell = guard->current_cell->south;
}

void move_west(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_west_in_this_cell++;
    guard->current_cell = guard->current_cell->west;
}

void obstacle_north(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_north_in_this_cell++;
    guard->direction = EAST;
    guard->move = move_guard_east;
}

void obstacle_east(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_east_in_this_cell++;
    guard->direction = SOUTH;
    guard->move = move_guard_south;
}

void obstacle_south(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_south_in_this_cell++;
    guard->direction = WEST;
    guard->move = move_guard_west;
}

void obstacle_west(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_west_in_this_cell++;
    guard->direction = NORTH;
    guard->move = move_guard_north;
}

void no_more_moves_north(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_north_in_this_cell++;
    guard->direction = NO_MORE_MOVES;
    guard->move = NULL;
}

void no_more_moves_east(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_east_in_this_cell++;
    guard->direction = NO_MORE_MOVES;
    guard->move = NULL;
}

void no_more_moves_south(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_south_in_this_cell++;
    guard->direction = NO_MORE_MOVES;
    guard->move = NULL;
}

void no_more_moves_west(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_west_in_this_cell++;
    guard->direction = NO_MORE_MOVES;
    guard->move = NULL;
}

void move_guard_north(struct guard *guard) {
    printf("Moving guard north\n");
    guard->current_cell->move_guard_north(guard);
}

void move_guard_east(struct guard *guard) {
    printf("Moving guard east\n");
    guard->current_cell->move_guard_east(guard);
}

void move_guard_south(struct guard *guard) {
    printf("Moving guard south\n");
    guard->current_cell->move_guard_south(guard);
}

void move_guard_west(struct guard *guard) {
    printf("Moving guard west\n");
    guard->current_cell->move_guard_west(guard);
}

int main() {
    struct patrol_area_map* patrol_area_map;
    struct patrol_area *patrol_area;
    int loops_total = 0;

    patrol_area_map = create_patrol_area_map_from_stdin();

    int x,y;
    find_start_position_in_patrol_area_map(&x, &y, patrol_area_map);

    patrol_area = create_patrol_area_from_map(patrol_area_map, x, y);

    struct guard guard = {NORTH, &patrol_area->cells[y][x], move_guard_north};

    print_patrol_area(patrol_area, patrol_area_map->width, patrol_area_map->height);

    while (guard.direction != NO_MORE_MOVES) {
        guard.move(&guard);
    }

    printf ("Guard has stopped moving\n");

    print_patrol_area(patrol_area, patrol_area_map->width, patrol_area_map->height);

    printf("Patrol area width: %d\n", patrol_area_map->width);
    printf("Patrol area height: %d\n", patrol_area_map->height);
    printf("Start position: %d, %d\n", x, y);

    printf("Total loops: %d\n", loops_total);

    destroy_patrol_area(patrol_area);
    destroy_patrol_area_map(patrol_area_map);
}