#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

/* TODO: Phase 1 */

int fs_mount(const char *diskname)
{
	//Check if the disk can be openned
        if(block_disk_open(diskname) == -1 ){
                return -1;
        }
        //Check if block is readable
        if(block_read(0,(void*)super_block) == -1){
                return -1;
        }
        //Check number of blocks and signature
        if(block_disck_count() != super_block.total_num_block && memcp(super_block.sign,"ECS150FS",64) != 0){
                return -1;
        }
        //Initialize memmory for flat array
        flat_array = malloc((super_block.num_FAT_blocks) * 4096 * sizeof(unint16_t));
        //Check if memmory is initialized correctly
        if(flat_array == NULL){
                return -1
        }
}

int fs_umount(void)
{
	/* TODO: Phase 1 */
}

int fs_info(void)
{
	/* TODO: Phase 1 */
}

int fs_create(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_ls(void)
{
	/* TODO: Phase 2 */
}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}
