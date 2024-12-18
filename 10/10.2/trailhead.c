#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct coord {
    int x;
    int y;
};

struct trail {
    struct coord* coords;
    int coords_size;
    struct coord* nine_height;
};

struct trail* trail_create() {
    struct trail* t = (struct trail*)malloc(sizeof(struct trail));
    t->coords = NULL;
    t->coords_size = 0;
    t->nine_height = NULL;
    return t;
}

void trail_destroy(struct trail* t) {
    free(t->coords);
    free(t);
}

void trail_add_coord(struct trail* t, int x, int y) {
    t->coords = (struct coord*)realloc(t->coords, (t->coords_size + 1) * sizeof(struct coord));
    t->coords[t->coords_size].x = x;
    t->coords[t->coords_size].y = y;
    t->coords_size++;
}

struct trailhead {
    struct trail* trails;
    int trails_size;
};

struct trailhead* trailhead_create() {
    struct trailhead* th = (struct trailhead*)malloc(sizeof(struct trailhead));
    th->trails = NULL;
    th->trails_size = 0;
    return th;
}

void trailhead_destroy(struct trailhead* th) {
    for (int i = 0; i < th->trails_size; i++) {
        trail_destroy(&th->trails[i]);
    }
    free(th->trails);
    free(th);
}

void trailhead_add_trail(struct trailhead* th, struct trail* t) {
    th->trails = (struct trail*)realloc(th->trails, (th->trails_size + 1) * sizeof(struct trail));
    th->trails[th->trails_size] = *t;
    th->trails_size++;
}

int **merge_scores(int** score, int** new_score) {
    int size;
    for (size = 1;score[size - 1] != NULL; size++);

    for (int i = 0; new_score[i] != NULL; i++) {
        int known_score = 0;
        for (int j = 0; score[j] != NULL; j++) {
            if (new_score[i] == score[j]) {
                known_score = 1;
                break;
            }
        }
        if (!known_score) {
            score = (int**)realloc(score, ++size * sizeof(int*));
            score[size - 2] = new_score[i];
            score[size - 1] = NULL;
        }
    }
    return score;
}

void print_nine_heights(int** nine_heights) {
    printf("Nine Heights: ");
    for (int i = 0; nine_heights[i] != NULL; i++) {
        printf("(%p)", nine_heights[i]);
    }
    printf("\n");
}

int** get_trailhead_nine_heights(int** map, int rows, int cols, int x, int y, int height, int* rating) {
    int **score = (int**)malloc(sizeof(int*));
    *score = NULL;
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        if (map[y][x] == height) {
             printf("Height: %d, (%d, %d)\n", height, x, y);
             if (height == 9) {
                *rating += 1;
                score[0] = (int*)malloc(2 * sizeof(int));
                score[0] = &map[y][x];
                score[1] = NULL;
             } else {
                int **score_up = get_trailhead_nine_heights(map, rows, cols, x, y - 1, height + 1, rating);
                int **score_down = get_trailhead_nine_heights(map, rows, cols, x, y + 1, height + 1, rating);
                int **score_left = get_trailhead_nine_heights(map, rows, cols, x - 1, y, height + 1, rating);
                int **score_right = get_trailhead_nine_heights(map, rows, cols, x + 1, y, height + 1, rating);

                score = merge_scores(score, score_up);
                score = merge_scores(score, score_down);
                score = merge_scores(score, score_left);
                score = merge_scores(score, score_right);

                free(score_up);
                free(score_down);
                free(score_left);
                free(score_right);   
            }
        }
    } 
    return score;
}

int main() {
    char buffer[BUFSIZ];
    int trailhead_score = 0;
    int trailhead_rating = 0;
    int** map = NULL;
    int rows = 0;
    int cols = 0;

    // Read the number of rows and columns
    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        printf("Buffer: %s\n", buffer);
        if (cols == 0){
            cols = strlen(buffer);
            if (cols > 0 && buffer[cols - 1] == '\n') {
                buffer[--cols] = '\0';
            }
            printf("Init Cols: %d\n", cols);
        }
        map = (int**)realloc(map, (rows + 1) * sizeof(int*));
        map[rows] = (int*)malloc(cols * sizeof(int));
        for (int i = 0; i < cols; i++) {
            map[rows][i] = buffer[i] - '0';
        }
        rows++;
        printf("Rows: %d\n", rows);
    }

    printf("Rows: %d\n", rows);
    printf("Cols: %d\n", cols);

    // Calculate the trailhead score
        for (int j = 0; j < rows; j++) {
    for (int i = 0; i < cols; i++) {
            if (map[j][i] == 0) {
                printf("Trailhead at (%d, %d)\n", i, j);
                int **nine_heights = get_trailhead_nine_heights(map, rows, cols, i, j, 0, &trailhead_rating);
                print_nine_heights(nine_heights);
                for (int k = 0; nine_heights[k] != NULL; k++) {
                    trailhead_score++;
                }
                free(nine_heights);
            }
        }
    }

    printf("Trailhead Score: %d\n", trailhead_score);
    printf("Trailhead Rating: %d\n", trailhead_rating);

    return 0;
}