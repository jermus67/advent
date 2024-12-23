#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct garden {
    char** plots;
    int **visited_plots;
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

void garden_add_row(struct garden* garden, const char* row) {
    garden->plots = realloc(garden->plots, sizeof(char*) * (garden->rows + 1));
    garden->visited_plots = realloc(garden->visited_plots, sizeof(int*) * (garden->rows + 1));
    garden->plots[garden->rows] = strdup(row);
    if (garden->columns == 0) {
        garden->columns = strlen(row);
        if (garden->plots[garden->rows][garden->columns - 1] == '\n') {
            garden->plots[garden->rows][garden->columns - 1] = '\0';
            garden->columns--;
        }
    }
    garden->visited_plots[garden->rows] = calloc(garden->columns, sizeof(int));
    garden->rows++;
}

struct region {
    char type;
    unsigned long long area;
    unsigned long long perimeter;
};

struct region* region_create(char type) {
    struct region* region = malloc(sizeof(struct region));
    region->type = type;
    region->area = 0;
    region->perimeter = 0;

    return region;
}

void region_destroy(struct region* region) {
    free(region);
}

void region_print(struct region* region) {
    printf("Region type: %c, ", region->type);
    printf("Region area: %llu, ", region->area);
    printf("Region perimeter: %llu, ", region->perimeter);
    printf("fence price: %llu\n", region->perimeter * region->area);
}

struct regions {
    struct region** regions;
    int count;
};

struct regions* regions_create() {
    struct regions* regions = malloc(sizeof(struct regions));
    regions->regions = NULL;
    regions->count = 0;

    return regions;
}

void regions_destroy(struct regions* regions) {
    for (int i = 0; i < regions->count; i++) {
        region_destroy(regions->regions[i]);
    }

    free(regions->regions);
    free(regions);
}

void regions_add(struct regions* regions, struct region* region) {
    regions->regions = realloc(regions->regions, sizeof(struct region*) * (regions->count + 1));
    regions->regions[regions->count] = region;
    regions->count++;
}

void regions_print(struct regions* regions) {
    for (int i = 0; i < regions->count; i++) {
        region_print(regions->regions[i]);
    }
}

void garden_find_regions(struct garden* garden, struct regions* regions, struct region* region, int row, int column) {
    printf("Finding regions\n");

    int fence_north = 1;
    int fence_south = 1;
    int fence_west = 1;
    int fence_east = 1;
    
    if (row < 0 || row >= garden->rows || column < 0 || column >= garden->columns) {
        printf("Out of bounds, row: %d, column: %d\n", row, column);
        return;
    }

    if (garden->plots[row][column] == '.') {
        printf("Empty plot, row: %d, column: %d\n", row, column);
        return;
    }

    if (garden->visited_plots[row][column]) {
        printf("Already visited, row: %d, column: %d\n", row, column);
        return;
    }

    if (region != NULL && garden->plots[row][column] != region->type) {
        printf("Different type, row: %d, column: %d\n", row, column);
        return;
    }

    if (region == NULL) {
        printf("New type: %c, ", garden->plots[row][column]);
    } else {
        printf("Type: %c, ", region->type);
    }

    printf("row: %d, column: %d: ", row, column);

    if (region == NULL) {
        region = region_create(garden->plots[row][column]);
        regions_add(regions, region);
    }

    region->area++;
    region->perimeter += fence_north + fence_south + fence_west + fence_east;

    garden->visited_plots[row][column] = 1;

    if (row > 0 && garden->plots[row - 1][column] == garden->plots[row][column]) {
        region->perimeter -= fence_north;
        fence_north = 0;
    } else {
        printf("fence North, ");
    }

    if (row < garden->rows - 1 && garden->plots[row + 1][column] == garden->plots[row][column]) {
        region->perimeter -= fence_south;
        fence_south = 0;
    } else {
        printf("fence South, ");
    }

    if (column > 0 && garden->plots[row][column - 1] == garden->plots[row][column]) {
        region->perimeter -= fence_west;
        fence_west = 0;
    } else {
        printf("fence West, ");
    }

    if (column < garden->columns - 1 && garden->plots[row][column + 1] == garden->plots[row][column]) {
        region->perimeter -= fence_east;
        fence_east = 0;
    } else {
        printf("fence East, ");
    }

    printf("\n");

    if (!fence_north) {
        garden_find_regions(garden, regions, region, row - 1, column);
    }

    if (!fence_south) {
        garden_find_regions(garden, regions, region, row + 1, column);
    }

    if (!fence_west) {
        garden_find_regions(garden, regions, region, row, column - 1);
    }

    if (!fence_east) {
        garden_find_regions(garden, regions, region, row, column + 1);
    }
}

unsigned long long calculate_fence_price(struct regions* regions) {
    unsigned long long total_fence_price = 0;

    for (int i = 0; i < regions->count; i++) {
        total_fence_price += regions->regions[i]->perimeter * regions->regions[i]->area;
    }

    return total_fence_price;
}

int main() {
    unsigned long long total_fence_price = 0;
    struct garden* garden = garden_create();
    char buffer[BUFSIZ];

    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        garden_add_row(garden, buffer);
    }

    garden_print(garden);

    printf("Rows: %d, Columns: %d\n", garden->rows, garden->columns);
    struct regions* regions = regions_create();
    printf("Regions created\n");

    for (int i = 0; i < garden->rows; i++) {
        for (int j = 0; j < garden->columns; j++) {
            if (!garden->visited_plots[i][j]) {
                garden_find_regions(garden, regions, NULL, i, j);
            }
        }
    }

    regions_print(regions);

    total_fence_price = calculate_fence_price(regions);
    
    printf("Total fence price: %llu\n", total_fence_price);

    return 0;
}