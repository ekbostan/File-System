#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

// /* TODO: Phase 1 */
// // define data structures corresponding to blocks containing meta-info about file system (superblock, FAT and root directory)
// // data types to use: stdint.h, such as int8_t, uint8_t, uint16_t, etc
// // attach attribute "packed" to these data structures

struct __attribute__ ((__packed__)) superblock{
    char sign[8];
    uint16_t total_num_blocks;
    uint16_t root_dir_idx;
    uint16_t data_init_idx;
    uint16_t num_data_blocks;
    uint8_t num_FAT_blocks;
    uint8_t padding[4079];
};
struct __attribute__ ((__packed__)) FAT{
    uint16_t data_block; //0 if free
};
struct __attribute__ ((__packed__)) rootdir{
    char file_name[16];
    uint32_t file_size;
    uint16_t idx;
    uint8_t padding[10];
};
struct superblock* super_block = NULL;
struct FAT* flat_array;
struct rootdir* rootdir_array[128];
struct rootdir*  root_array[FS_FILE_MAX_COUNT];
char signature[8]="ECS150FS";


int fs_mount(const char *diskname)
{ 
	printf("mounting\n");
	
	if(block_disk_open(diskname) == -1){
            return -1;
	}
	else {
		printf("disk openend\n");
	}

	printf("disk openend 2\n");

	//super_block = (struct superblock*)malloc(sizeof(struct superblock*));
	
	super_block = malloc(4096);
	//super_block->total_num_blocks = 0;

	if(block_read(0, (void*)super_block) == -1){
			return -1;
	}
	else {
		printf("read 2\n");
	}

	printf("Disk count is %d\n", super_block->total_num_blocks);

	if(block_disk_count() != super_block->total_num_blocks){
		printf("Disk count failed\n");
		return -1;
	}
	else {
		printf("Disk count passed\n");
	}

	if(memcmp(super_block->sign,"ECS150FS",8) != 0){
		printf("Disk sign failed\n");
		return -1;
	}
	else {
		printf("Disk sign passed\n");
	}

	flat_array = malloc((super_block->num_FAT_blocks) * 4096 * sizeof(uint16_t));
    //Check if memmory is initialized correctly
	if(flat_array == NULL){
			return -1;
	}
	else {
		printf("flat array\n");
	}

	  for (int i = 0; i < super_block->num_FAT_blocks;i++ )
	{
		if(block_read(i+1, &flat_array[i*2048]) == -1){

				return -1;
		}
		else 
		{
			printf("blocks\n");
		}
	}

	if(block_read(super_block->root_dir_idx,&rootdir_array) == -1){
			return -1;
    }
	else 
	{
		printf("root dir\n");
	}

    return 0;
}

int fs_umount(void)
{
/*	if(super_block == NULL){
         return -1;

        }
	//Check
        if(block_write(0,(void*)&super_block) == -1 ||block_write(super_block->root_dir_idx,&rootdir_array) == -1 ){
                return -1;

        }
        if(super_block->total_num_blocks > (super_block->num_FAT_blocks)* 2048){
                 return -1;
         }
        for (int i = 0; i < super_block->num_FAT_blocks;i++ )
        {
                 if(block_read(i+1, &flat_array[i*2048]) != -1){

                         continue;
                 }
                 else
                         return -1;
        }
        int bit_count = 0;
        while(bit_count < 128){
                memset(rootdir_array[bit_count]->file_name,0,16);
                rootdir_array[bit_count]->idx =0;
                rootdir_array[bit_count]->file_size =0;
                bit_count++;
        }
        free(flat_array);
        super_block->total_num_blocks =0;
        super_block->num_data_blocks = 0;
        super_block->root_dir_idx =0;
        super_block->data_init_idx=0;
        super_block->num_FAT_blocks =0;
        memset(super_block->sign,'\0',8);
        if(block_disk_close() == -1){
                return -1;
        }

*/
	/* TODO: Phase 1 */
	return 0;
}


int fs_info(void)
{
	/* TODO: Phase 1 */
	return 0;
}

int fs_create(const char *filename)
{
	/* TODO: Phase 2 */
	return 0;
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
	return 0;
}

int fs_ls(void)
{
	/* TODO: Phase 2 */
	return 0;
}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */
	return 0;
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */
	return 0;
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */
	return 0;
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */
	return 0;
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
	return 0;
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
	return 0;
}