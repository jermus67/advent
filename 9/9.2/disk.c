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
    for (long i = 0; i < disk->disk_size; i++) {
        if (disk->disk_content[i] >= 0) {
            checksum += i * disk->disk_content[i];
        }
    }
    return checksum;
}

// obsolete
void disk_swap_blocks(struct disk* disk, long block1, long block2) {
    long tmp = disk->disk_content[block1];
    disk->disk_content[block1] = disk->disk_content[block2];
    disk->disk_content[block2] = tmp;
}

void disk_unfragment(struct disk* disk) {
    long free_block_index;
    long free_block_size;
    long last_allocated_file_index;
    long last_allocated_file_size;

    for (last_allocated_file_index = disk->disk_size-1; last_allocated_file_index >= 0; last_allocated_file_index--) {
        // find first block allocated to a file
        while (disk->disk_content[last_allocated_file_index] < 0) {
            last_allocated_file_index--;
        }
        long file_id = disk->disk_content[last_allocated_file_index];
        last_allocated_file_size = 1;
        // find the size of the file (and the index of the first block of the file ...)
        while (disk->disk_content[last_allocated_file_index-1] == file_id) {
            last_allocated_file_size++;
            last_allocated_file_index--;
        }

        for (free_block_index = 0; free_block_index < disk->disk_size && free_block_index < last_allocated_file_index; free_block_index++) {
            if (disk->disk_content[free_block_index] < 0) {
                free_block_size = 0;
                while (disk->disk_content[free_block_index+free_block_size] < 0) {
                    free_block_size++;
                }
                if (free_block_size >= last_allocated_file_size) {
                    break;
                }
            }
        }
        if (free_block_index < last_allocated_file_index && free_block_size >= last_allocated_file_size) {
            for (long i = 0; i < last_allocated_file_size; i++) {
                disk_swap_blocks(disk, free_block_index+i, last_allocated_file_index+i);
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