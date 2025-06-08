#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <disk_image> <filename>\n", argv[0]);
        return 1;
    }
    
    char *disk_image = argv[1];
    char *filename = argv[2];
    
    // Mount filesystem
    if (fs_mount(disk_image) < 0) {
        fprintf(stderr, "Cannot mount disk\n");
        return 1;
    }
    
    // Create file
    if (fs_create(filename) < 0) {
        fprintf(stderr, "Cannot create file\n");
        fs_umount();
        return 1;
    }
    
    // Open file for writing
    int fd = fs_open(filename);
    if (fd < 0) {
        fprintf(stderr, "Cannot open file\n");
        fs_umount();
        return 1;
    }
    
    // Write data to file
    char *data = "Hello, File System! This is a test write operation.";
    int bytes_written = fs_write(fd, data, strlen(data));
    if (bytes_written < 0) {
        fprintf(stderr, "Write failed\n");
        fs_close(fd);
        fs_umount();
        return 1;
    }
    
    printf("Successfully wrote %d bytes to %s\n", bytes_written, filename);
    
    // Close file and unmount
    fs_close(fd);
    fs_umount();
    
    return 0;
}