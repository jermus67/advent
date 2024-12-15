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
    int x;
    int y;
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
    } else if (patrol_area_map->map[y_north][x_north] == '#' || patrol_area_map->map[y_north][x_north] == 'O') {
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
    } else if (patrol_area_map->map[y_east][x_east] == '#' || patrol_area_map->map[y_east][x_east] == 'O') {
        cell->east = NULL;
        cell->move_guard_east = obstacle_east;
    } else {
        cell->east = &patrol_area->cells[y_east][x_east];
        cell->move_guard_east = move_east;
    }
}

void initialize_move_south(struct patrol_area_cell* cell, struct patrol_area *patrol_area, int y_south, int x_south, struct patrol_area_map *patrol_area_map) {
    cell->number_of_times_guard_has_moved_south_in_this_cell = 0;
    if (y_south >= patrol_area_map->height) {
        cell->south = NULL;
        cell->move_guard_south = no_more_moves_south;
    } else if (patrol_area_map->map[y_south][x_south] == '#' || patrol_area_map->map[y_south][x_south] == 'O') {
        cell->south = NULL;
        cell->move_guard_south = obstacle_south;
    } else {
        cell->south = &patrol_area->cells[y_south][x_south];
        cell->move_guard_south = move_south;
    }
}

void initialize_move_west(struct patrol_area_cell* cell, struct patrol_area *patrol_area, int y_west, int x_west, struct patrol_area_map *patrol_area_map) {
    cell->number_of_times_guard_has_moved_west_in_this_cell = 0;
    if (x_west < 0) {
        cell->west = NULL;
        cell->move_guard_west = no_more_moves_west;
    } else if (patrol_area_map->map[y_west][x_west] == '#' || patrol_area_map->map[y_west][x_west] == 'O') {
        cell->west = NULL;
        cell->move_guard_west = obstacle_west;
    } else {
        cell->west = &patrol_area->cells[y_west][x_west];
        cell->move_guard_west = move_west;
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
        cell->x = x;
        cell->y = y;
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

int patrol_area_cell_is_start_position(struct patrol_area_cell *cell) {
    return cell->start_position;
}

int patrol_area_cell_is_obstacle(struct patrol_area_cell *cell) {
    return cell->north == NULL && cell->east == NULL && cell->south == NULL && cell->west == NULL;
}

int patrol_area_cell_is_visited(struct patrol_area_cell *cell) {
    return cell->number_of_times_guard_has_moved_north_in_this_cell > 0 ||
           cell->number_of_times_guard_has_moved_east_in_this_cell > 0 ||
           cell->number_of_times_guard_has_moved_south_in_this_cell > 0 ||
           cell->number_of_times_guard_has_moved_west_in_this_cell > 0;
}

int patrol_area_cell_is_north_south(struct patrol_area_cell *cell) {
    return (cell->number_of_times_guard_has_moved_north_in_this_cell > 0 || cell->number_of_times_guard_has_moved_south_in_this_cell > 0) &&
            cell->number_of_times_guard_has_moved_east_in_this_cell == 0 && cell->number_of_times_guard_has_moved_west_in_this_cell == 0;
}

int patrol_area_cell_is_east_west(struct patrol_area_cell *cell) {
    return (cell->number_of_times_guard_has_moved_east_in_this_cell > 0 || cell->number_of_times_guard_has_moved_west_in_this_cell > 0) &&
            cell->number_of_times_guard_has_moved_north_in_this_cell == 0 && cell->number_of_times_guard_has_moved_south_in_this_cell == 0;
}

int patrol_area_cell_is_north_south_east_west(struct patrol_area_cell *cell) {
    return (cell->number_of_times_guard_has_moved_north_in_this_cell > 0 || cell->number_of_times_guard_has_moved_south_in_this_cell > 0) &&
            (cell->number_of_times_guard_has_moved_east_in_this_cell > 0 || cell->number_of_times_guard_has_moved_west_in_this_cell > 0);
}

void print_patrol_area(struct patrol_area *patrol_area, int width, int height) {

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (patrol_area_cell_is_obstacle(&patrol_area->cells[y][x])) {
                printf("#");
            } else if (patrol_area_cell_is_start_position(&patrol_area->cells[y][x])) {
                printf("^");
            } else if (patrol_area_cell_is_north_south(&patrol_area->cells[y][x])) {
                printf("|");
            } else if (patrol_area_cell_is_east_west(&patrol_area->cells[y][x])) {
                printf("-");
            } else if (patrol_area_cell_is_north_south_east_west(&patrol_area->cells[y][x])) {
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
    NO_MORE_MOVES = 4,
    LOOP_DETECTED = 5
} Direction;

struct guard {
    Direction direction;
    int loop_detected;
    struct patrol_area_cell *current_cell;
    void (*move)(struct guard *);
};

void move_guard_north(struct guard *guard);
void move_guard_east(struct guard *guard);
void move_guard_south(struct guard *guard);
void move_guard_west(struct guard *guard);

void move_north(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_north_in_this_cell++;
    guard->loop_detected = guard->current_cell->number_of_times_guard_has_moved_north_in_this_cell > 1;
    guard->current_cell = guard->current_cell->north;
}

void move_east(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_east_in_this_cell++;
    guard->loop_detected = guard->current_cell->number_of_times_guard_has_moved_east_in_this_cell > 1;
    guard->current_cell = guard->current_cell->east;
}

void move_south(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_south_in_this_cell++;
    guard->loop_detected = guard->current_cell->number_of_times_guard_has_moved_south_in_this_cell > 1;
    guard->current_cell = guard->current_cell->south;
}

void move_west(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_west_in_this_cell++;
    guard->loop_detected = guard->current_cell->number_of_times_guard_has_moved_west_in_this_cell > 1;
    guard->current_cell = guard->current_cell->west;
}

void obstacle_north(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_north_in_this_cell++;
    guard->direction = EAST;
    guard->move = move_guard_east;
    guard->move(guard);
}

void obstacle_east(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_east_in_this_cell++;
    guard->direction = SOUTH;
    guard->move = move_guard_south;
    guard->move(guard);
}

void obstacle_south(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_south_in_this_cell++;
    guard->direction = WEST;
    guard->move = move_guard_west;
    guard->move(guard);
}

void obstacle_west(struct guard *guard) {
    guard->current_cell->number_of_times_guard_has_moved_west_in_this_cell++;
    guard->direction = NORTH;
    guard->move = move_guard_north;
    guard->move(guard);
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
    guard->current_cell->move_guard_north(guard);
}

void move_guard_east(struct guard *guard) {
    guard->current_cell->move_guard_east(guard);
}

void move_guard_south(struct guard *guard) {
    guard->current_cell->move_guard_south(guard);
}

void move_guard_west(struct guard *guard) {
    guard->current_cell->move_guard_west(guard);
}

int count_positions_total(struct patrol_area *patrol_area) {
    int positions_total = 0;
    for (int y = 0; y < patrol_area->height; y++) {
        for (int x = 0; x < patrol_area->width; x++) {
            if (patrol_area->cells[y][x].number_of_times_guard_has_moved_north_in_this_cell > 0 ||
                patrol_area->cells[y][x].number_of_times_guard_has_moved_east_in_this_cell > 0 ||
                patrol_area->cells[y][x].number_of_times_guard_has_moved_south_in_this_cell > 0 ||
                patrol_area->cells[y][x].number_of_times_guard_has_moved_west_in_this_cell > 0) {
                positions_total++;
            }
        }
    }
    return positions_total;
}

struct patrol_area* duplicate_patrol_area(struct patrol_area *patrol_area) {
    struct patrol_area *patrol_area_copy = (struct patrol_area*)malloc(sizeof(struct patrol_area));
    patrol_area_copy->width = patrol_area->width;
    patrol_area_copy->height = patrol_area->height;
    patrol_area_copy->cells = (struct patrol_area_cell**)malloc(patrol_area_copy->height * sizeof(struct patrol_area_cell*));
    for (int y = 0; y < patrol_area_copy->height; y++) {
        patrol_area_copy->cells[y] = (struct patrol_area_cell*)malloc(patrol_area_copy->width * sizeof(struct patrol_area_cell));
    }

    for (int y = 0; y < patrol_area_copy->height; y++) {
        for (int x = 0; x < patrol_area_copy->width; x++) {
            patrol_area_copy->cells[y][x] = patrol_area->cells[y][x];
        }
    }

    return patrol_area_copy;
}

int patrol_area_has_loop(struct patrol_area *patrol_area, int x, int y) {
    struct guard guard = {NORTH, 0, &patrol_area->cells[y][x], move_guard_north};
    while (guard.direction != NO_MORE_MOVES && !guard.loop_detected) {
        guard.move(&guard);
    }
    return guard.loop_detected;
}

int main() {
    int positions_total = 0;
    int loops_total = 0;

    struct patrol_area_map* patrol_area_map = create_patrol_area_map_from_stdin();

    int x,y;
    find_start_position_in_patrol_area_map(&x, &y, patrol_area_map);

    struct patrol_area* trajectory = create_patrol_area_from_map(patrol_area_map, x, y);
    trajectory = create_patrol_area_from_map(patrol_area_map, x, y);

    struct guard guard = {NORTH, 0, &trajectory->cells[y][x], move_guard_north};   
    while (guard.direction != NO_MORE_MOVES) {
        guard.move(&guard);
        if (guard.direction != NO_MORE_MOVES) {
            if (!patrol_area_cell_is_visited(guard.current_cell)) {
                int obstruction_x = guard.current_cell->x;
                int obstruction_y = guard.current_cell->y;
                if (! trajectory->cells[obstruction_y][obstruction_x].start_position) {
                    patrol_area_map->map[obstruction_y][obstruction_x] = 'O';
                    struct patrol_area* patrol_area = create_patrol_area_from_map(patrol_area_map, x, y);
                    if (patrol_area_has_loop(patrol_area, x, y)) {
                        loops_total++;
                    }
                    destroy_patrol_area(patrol_area);
                    patrol_area_map->map[obstruction_y][obstruction_x] = '.';
                }
            }
        }
    }

    positions_total = count_positions_total(trajectory);

    printf("Positions total: %d\n", positions_total);
    printf("Total loops: %d\n", loops_total);

    destroy_patrol_area(trajectory);
    destroy_patrol_area_map(patrol_area_map);
}