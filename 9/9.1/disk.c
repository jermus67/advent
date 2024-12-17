#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

struct disk {
    long *disk_content;
    long disk_size;
};

struct disk* disk_create() {
    struct disk *d = (struct disk *) malloc(sizeof(struct disk));
    d->disk_content = (long *) NULL;
    d->disk_size = 0L;
    return d;
}

void disk_destroy(struct disk *d) {
    if (d->disk_content != (long *) NULL) {
        free(d->disk_content);
    }
    free(d);
}

void disk_add(struct disk *d, long file_id, long size) {
    d->disk_content = (long *) realloc(d->disk_content, (d->disk_size + size) * sizeof(long));

    for (long i = 0; i < size; i++) {
        d->disk_content[d->disk_size] = file_id;
        d->disk_size++;
    }
}

void disk_print(struct disk *d) {
    for (long i = 0; i < d->disk_size; i++) {
        if (d->disk_content[i] < 0) {
            printf(".");
        } else {
            printf("%ld", d->disk_content[i]);
        }
    }
    printf("\n");
}

struct disk* disk_read(FILE *f) {
    struct disk *disk = disk_create();
    long file_id = 0L;
    long size = 0L;
    while (!feof(f)) {
        char c = fgetc(f);
        if (isdigit(c)) {
            size = c - '0';
            disk_add(disk, file_id++, size);
        }

        if (!feof(f)) {
            c = fgetc(f);
            if (isdigit(c)) {
                size = c - '0';
                disk_add(disk, -1L, size);
            }
        }
    }
    return disk;
}

long disk_calculate_checksum(struct disk* disk) {
    long checksum = 0;
    for (long i = 0; i < disk->disk_size && disk->disk_content[i] >=0 ; i++) {
        checksum += i * disk->disk_content[i];
    }
    return checksum;
}

void disk_swap_blocks(struct disk* disk, long block1, long block2) {
    long tmp = disk->disk_content[block1];
    disk->disk_content[block1] = disk->disk_content[block2];
    disk->disk_content[block2] = tmp;
}

void disk_unfragment(struct disk* disk) {
    long last_non_empty_block = disk->disk_size - 1;
    for (long first_empty_block = 0; first_empty_block < disk->disk_size && first_empty_block < last_non_empty_block; first_empty_block++) {
        if (disk->disk_content[first_empty_block] < 0) {
            while (last_non_empty_block > first_empty_block && disk->disk_content[last_non_empty_block] < 0) {
                last_non_empty_block--;
            }
            if (last_non_empty_block > first_empty_block) {
                disk_swap_blocks(disk, first_empty_block, last_non_empty_block);
            }
        }
    }
}

int main() {
    long checksum = 0L;

    struct disk *d = disk_read(stdin);

    disk_print(d);

    disk_unfragment(d);

    disk_print(d);

    checksum = disk_calculate_checksum(d);

    printf("Checksum: %ld\n", checksum);

    return 0;
}