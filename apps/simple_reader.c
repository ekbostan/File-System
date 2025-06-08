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
    
    // Open file for reading
    int fd = fs_open(filename);
    if (fd < 0) {
        fprintf(stderr, "Cannot open file\n");
        fs_umount();
        return 1;
    }
    
    // Get file size
    int file_size = fs_stat(fd);
    if (file_size < 0) {
        fprintf(stderr, "Cannot get file stats\n");
        fs_close(fd);
        fs_umount();
        return 1;
    }
    
    // Read data from file
    char *buffer = malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        fs_close(fd);
        fs_umount();
        return 1;
    }
    
    int bytes_read = fs_read(fd, buffer, file_size);
    if (bytes_read < 0) {
        fprintf(stderr, "Read failed\n");
        free(buffer);
        fs_close(fd);
        fs_umount();
        return 1;
    }
    
    buffer[bytes_read] = '\0';
    printf("File content (%d bytes):\n%s\n", bytes_read, buffer);
    
    // Cleanup
    free(buffer);
    fs_close(fd);
    fs_umount();
    
    return 0;
}