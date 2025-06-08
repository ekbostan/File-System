# Instructions for Implementing fs.write Module

## Overview
This document provides detailed instructions for implementing a new `fs.write` module that enables creating and writing content to files. The module will integrate with the existing File-System codebase.

## Current Codebase Analysis
The repository has a well-structured filesystem implementation with:
- **libfs/**: Core filesystem library (disk.c, fs.c, headers)
- **apps/**: Test applications and utilities
- **Existing API**: Complete filesystem functionality including fs_write() already implemented in fs.c
- **Build System**: Makefiles for library and applications
- **Testing**: Script-based test framework in test_fs.c

## Implementation Plan

### Step 1: Create Feature Branch
```bash
git checkout -b fs-write-module
```

### Step 2: Implement Missing Test Programs
The Makefile references `simple_writer.x` and `simple_reader.x` but the source files don't exist. We need to create these:

#### 2.1 Create `apps/simple_writer.c`
Location: `/apps/simple_writer.c`

This program should:
- Mount a filesystem
- Create a new file
- Write content to the file
- Close the file and unmount

#### 2.2 Create `apps/simple_reader.c` 
Location: `/apps/simple_reader.c`

This program should:
- Mount a filesystem
- Open an existing file
- Read content from the file
- Display the content
- Close the file and unmount

### Step 3: Update Build System
Ensure the Makefile in `apps/` can build the new programs by verifying the existing targets:
- `simple_writer.x`
- `simple_reader.x`

### Step 4: Write Unit Tests
Create comprehensive tests for the write functionality:

#### 4.1 Update `apps/scripts/example.script`
Add test cases that exercise the write functionality:
- File creation and writing
- Write operations with different buffer sizes
- Error conditions (disk full, invalid handles, etc.)

#### 4.2 Integration Tests
Add test cases in the script-based testing framework that verify:
- Writing to new files
- Appending to existing files
- Writing across block boundaries
- Error handling for various edge cases

### Step 5: Code Implementation

#### 5.1 simple_writer.c Implementation
```c
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
```

#### 5.2 simple_reader.c Implementation
```c
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
    size_t file_size = 0;
    if (fs_stat(fd, &file_size) < 0) {
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
```

#### 5.3 Enhanced Test Script
Update `apps/scripts/example.script` to include comprehensive write tests:
```
# Test file creation and writing
CREATE myfile.txt
OPEN myfile.txt
WRITE Hello World! This is a test file.
CLOSE
LS

# Test reading back the data
OPEN myfile.txt
READ 50
CLOSE

# Test large file writing
CREATE large.dat
OPEN large.dat
WRITE [Large data content that spans multiple blocks]
CLOSE
INFO

# Test error conditions
CREATE /invalid/path.txt  # Should fail
WRITE without_open        # Should fail
```

### Step 6: Testing Instructions

#### 6.1 Build the Programs
```bash
cd apps
make clean
make
```

#### 6.2 Test the Implementation
```bash
# Create a disk image
./fs_make.x disk.fs 100

# Test writing
./simple_writer.x disk.fs test.txt

# Test reading
./simple_reader.x disk.fs test.txt

# Run comprehensive tests
./test_fs.x disk.fs scripts/example.script
```

#### 6.3 Verify Functionality
- Ensure files can be created and written to
- Verify data integrity by reading back written content
- Test error conditions and edge cases
- Confirm integration with existing filesystem operations

### Step 7: Git Workflow

#### 7.1 Stage Changes
```bash
git add apps/simple_writer.c apps/simple_reader.c apps/scripts/example.script
```

#### 7.2 Commit Changes
```bash
git commit -m "Add fs.write module and tests"
```

#### 7.3 Push Branch
```bash
git push origin fs-write-module
```

#### 7.4 Create Pull Request
```bash
gh pr create --title "Add fs.write module" --body "Implements the write functionality in the file system, along with tests."
```

## Expected Outcomes
After following these instructions:
1. Two new utility programs (`simple_writer` and `simple_reader`) will be available
2. Enhanced test scripts will provide comprehensive coverage of write operations
3. The filesystem's write functionality will be thoroughly tested and validated
4. A complete pull request will be ready for review

## Notes
- The core `fs_write()` function is already implemented in `libfs/fs.c`
- This module focuses on creating user-facing utilities and comprehensive tests
- All changes integrate with the existing build system and testing framework
- The implementation follows the established code patterns and conventions