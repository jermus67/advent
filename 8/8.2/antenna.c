#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct input {
    char** map;
    int map_height;
    int map_width;
};

struct input *input_create() {
    struct input *input = (struct input*)malloc(sizeof(struct input));
    input->map = NULL;
    input->map_height = 0;
    input->map_width = 0;
    return input;
}

void input_destroy(struct input* input) {
    for(int i = 0; i < input->map_height; i++) {
        free(input->map[i]);
    }
    free(input->map);
    free(input);
}

void input_add_line(struct input* input, char* line) {
    input->map = (char**)realloc(input->map, (input->map_height + 1) * sizeof(char*));
    if (input->map_width == 0) {
        int len = strlen(line);
        if (line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        input->map_width = strlen(line);
    }
    input->map[input->map_height] = (char*)malloc((input->map_width + 1) * sizeof(char));
    strcpy(input->map[input->map_height], line);
    input->map_height++;
}

struct vector2d {
    int x;
    int y;
};

struct vector2d* vector2d_create(int x, int y) {
    struct vector2d* vector = (struct vector2d*)malloc(sizeof(struct vector2d));
    vector->x = x;
    vector->y = y;
    return vector;
}

void vector2d_destroy(struct vector2d* vector) {
    free(vector);
}

struct vector2d* vector2d_add(struct vector2d* a, struct vector2d* b) {
    return vector2d_create(a->x + b->x, a->y + b->y);
}

struct vector2d* vector2d_substract(struct vector2d* a, struct vector2d* b) {
    return vector2d_create(a->x - b->x, a->y - b->y);
}

void vector2d_resize_length(struct vector2d* vector, int factor) {
    vector->x *= factor;
    vector->y *= factor;
}

int vector2d_is_equal(struct vector2d* a, struct vector2d* b) {
    return a->x == b->x && a->y == b->y;
}

struct vector2d_set {
    struct vector2d** vectors;
    size_t vectors_size;
};

struct vector2d_set* vector2d_set_create() {
    struct vector2d_set* set = (struct vector2d_set*)malloc(sizeof(struct vector2d_set));
    set->vectors = NULL;
    set->vectors_size = 0;
    return set;
}

void vector2d_set_destroy(struct vector2d_set* set) {
    free(set->vectors);
    free(set);
}

void vector2d_set_add(struct vector2d_set* set, struct vector2d* vector) {
    for(int i = 0; i < set->vectors_size; i++) {
        // printf("Comparing (%d, %d) with (%d, %d)\n", set->vectors[i]->x, set->vectors[i]->y, vector->x, vector->y);
        if (vector2d_is_equal(set->vectors[i], vector)) {
            return;
        }
    }
    set->vectors = (struct vector2d**)realloc(set->vectors, (set->vectors_size + 1) * sizeof(struct vector2d*));
    set->vectors[set->vectors_size] = vector;
    set->vectors_size++;
}

struct antenna {
    char type;
    struct vector2d** locations;
    size_t locations_size;
    size_t locations_capacity;
    struct vector2d** antinodes;
    size_t antinodes_size;
    size_t antinodes_capacity;
};

struct antenna* antenna_create(char type) {
    struct antenna* antenna = (struct antenna*)malloc(sizeof(struct antenna));
    antenna->type = type;
    antenna->locations = NULL;
    antenna->locations_size = 0;
    antenna->antinodes = NULL;
    antenna->antinodes_size = 0;
    return antenna;
}

void antenna_destroy(struct antenna* antenna) {
    for(int i = 0; i < antenna->locations_size; i++) {
        vector2d_destroy(antenna->locations[i]);
    }
    free(antenna->locations);
    for(int i = 0; i < antenna->antinodes_size; i++) {
        vector2d_destroy(antenna->antinodes[i]);
    }
    free(antenna->antinodes);
    free(antenna);
}

void antenna_add_location(struct antenna* antenna, struct vector2d* location) {
    antenna->locations = (struct vector2d**)realloc(antenna->locations, (antenna->locations_size + 1) * sizeof(struct vector2d*));
    antenna->locations[antenna->locations_size] = location;
    antenna->locations_size++;
}

void antenna_add_antinode(struct antenna* antenna, struct vector2d* antinode) {
    antenna->antinodes = (struct vector2d**)realloc(antenna->antinodes, (antenna->antinodes_size + 1) * sizeof(struct vector2d*));
    antenna->antinodes[antenna->antinodes_size] = antinode;
    antenna->antinodes_size++;
}

char antenna_get_type(struct antenna* antenna) {
    return antenna->type;
}

struct antennas {
    struct antenna** antennas;
    size_t antennas_size;
    size_t antennas_capacity;
};

struct antennas* antennas_create() {
    struct antennas* antennas = (struct antennas*)malloc(sizeof(struct antennas));
    antennas->antennas = NULL;
    antennas->antennas_size = 0;
    return antennas;
}

void antennas_destroy(struct antennas* antennas) {
    for(int i = 0; i < antennas->antennas_size; i++) {
        antenna_destroy(antennas->antennas[i]);
    }
    free(antennas->antennas);
    free(antennas);
}

void antennass_add_antenna(struct antennas* antennas, struct antenna* antenna) {
    antennas->antennas = (struct antenna**)realloc(antennas->antennas, (antennas->antennas_size + 1) * sizeof(struct antenna*));
    antennas->antennas[antennas->antennas_size] = antenna;
    antennas->antennas_size++;
}

struct antenna* antennas_find_by_type(struct antennas* antennas, char type) {
    for(int i = 0; i < antennas->antennas_size; i++) {
        if (antennas->antennas[i]->type == type) {
            return antennas->antennas[i];
        }
    }
    return NULL;
}

struct antenna* antennas_find_by_location(struct antennas* antennas, struct vector2d* location) {
    for(int i = 0; i < antennas->antennas_size; i++) {
        for(int j = 0; j < antennas->antennas[i]->locations_size; j++) {
            if (vector2d_is_equal(antennas->antennas[i]->locations[j], location)) {
                return antennas->antennas[i];
            }
        }
    }
    return NULL;
}

int antennas_is_known_antinode_location(struct antennas* antennas, struct vector2d* location) {
    for(int i = 0; i < antennas->antennas_size; i++) {
        for(int j = 0; j < antennas->antennas[i]->antinodes_size; j++) {
            if (vector2d_is_equal(antennas->antennas[i]->antinodes[j], location)) {
                return 1;
            }
        }
    }
    return 0;
}

void antennas_read_from_input(struct antennas* antennas, struct input* input) {
    for (int y = 0; y < input->map_height; y++) {
        for (int x = 0; x < input->map_width; x++) {
            if (isalnum(input->map[y][x])) {
                struct antenna* antenna = antennas_find_by_type(antennas, input->map[y][x]);
                if (antenna == NULL) {
                    antenna = antenna_create(input->map[y][x]);
                    antennass_add_antenna(antennas, antenna);
                }
                struct vector2d* location = vector2d_create(x, y);
                antenna_add_location(antenna, location);
            }
        }
    }
}

void antennas_print(struct antennas* antennas, struct input* input) {
    for (int y = 0; y < input->map_height; y++) {
        for (int x = 0; x < input->map_width; x++) {
            struct vector2d* location = vector2d_create(x, y);
            struct antenna* antenna = antennas_find_by_location(antennas, location);
            if (antenna != NULL) {
                printf("%c", antenna_get_type(antenna));
            } else if (antennas_is_known_antinode_location(antennas, location)) {
                printf("#");
            } else {
                printf("%c", input->map[y][x]);
            }
            vector2d_destroy(location);
        }
        printf("\n");
    }
}

int antennas_calculate_antinodes(struct antennas* antennas, struct input* input) {
    int number_of_antinodes = 0;
    for (int y = 0; y < input->map_height; y++) {
        for (int x = 0; x < input->map_width; x++) {
            struct vector2d* location = vector2d_create(x, y);
            struct antenna* antenna = antennas_find_by_location(antennas, location);
            if (antenna != NULL) {
                // printf("Calculating antinodes for antenna %c at location (%d, %d)\n", antenna_get_type(antenna), x, y);
                for (int i = 0; i < antenna->locations_size; i++) {
                    if (!vector2d_is_equal(antenna->locations[i], location)) {
                        // printf(" - Calculating antinode with location (%d, %d)\n", antenna->locations[i]->x, antenna->locations[i]->y);
                        struct vector2d* antinode_direction = vector2d_substract(antenna->locations[i], location);
                        // printf("   - Direction: (%d, %d)\n", antinode_direction->x, antinode_direction->y);
                        struct vector2d* antinode_location = antenna->locations[i];
                        // printf("   - Antinode location: (%d, %d)\n", antinode_location->x, antinode_location->y);
                        while (antinode_location->x >= 0 && antinode_location->x < input->map_width &&
                            antinode_location->y >= 0 && antinode_location->y < input->map_height) {
                            // printf("   - Antinode location is within map boundaries\n");
                            antenna_add_antinode(antenna, antinode_location);
                            antinode_location = vector2d_add(antinode_location, antinode_direction);
                        }
                        vector2d_destroy(antinode_direction);
                    }
                }
            }
            vector2d_destroy(location);
        }
    }

    struct vector2d_set* antinodes = vector2d_set_create();
    for (int i = 0; i < antennas->antennas_size; i++) {
        for (int j = 0; j < antennas->antennas[i]->antinodes_size; j++) {
            vector2d_set_add(antinodes, antennas->antennas[i]->antinodes[j]);
        }
    }
    number_of_antinodes = antinodes->vectors_size;
    vector2d_set_destroy(antinodes);

    return number_of_antinodes;
}

int main () {
    char buffer[BUFSIZ];
    struct input* input = input_create();
    int number_of_antinodes = 0;

    while(fgets(buffer, BUFSIZ, stdin) != NULL) {
        input_add_line(input, buffer);
    }

    struct antennas* antennas = antennas_create();
    antennas_read_from_input(antennas, input);
    antennas_print(antennas, input);
    printf("\n");
    number_of_antinodes = antennas_calculate_antinodes(antennas, input);
    antennas_print(antennas, input);

    printf("Number of antinodes: %d\n", number_of_antinodes);
}