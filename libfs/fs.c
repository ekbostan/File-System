#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

struct __attribute__ ((__packed__)) superblock{
    char sign[8];
    uint16_t total_num_blocks, num_data_blocks, root_dir_idx, data_init_idx;
    uint8_t num_FAT_blocks;
    uint8_t padding[4079];
};

struct __attribute__ ((__packed__)) FAT{
    uint16_t data_block; //0 if free
    struct FAT* next;
};

struct __attribute__ ((__packed__)) rootdir{
    uint16_t file_name[8];
    uint32_t file_size;
    uint16_t idx;
    uint8_t padding[10];
};
struct superblock* super_block;
struct FAT* flat_array;
struct rootdir* rootdir_array[128];

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
        if(block_disck_count() != super_block->total_num_block && memcmp(super_block->sign,"ECS150FS",8) != 0){
                return -1;
        }
        //Initialize memmory for flat array
        flat_array = malloc((super_block->num_FAT_blocks) * 4096 * sizeof(unint16_t));
        //Check if memmory is initialized correctly
        if(flat_array == NULL){
                return -1
        }
        //Check flat array size and number of blocks 
        if(super_block->total_num_block > (super_block->num_FAT_blocks)* 2048){
                 return -1;
          }
        //Read the data
         for (int i = 0; i < super_block->num_FAT_blocks;i++ )
        {
                 if(block_read(i+1, &flat_array[i*2048]) == -1){

                         return -1;
                 }
         }
        //Check root dir
        if(block_read(super_block->root_dir_idx,&rootdir_array) == -1){
        return -1;
        }

        return 0;

	
}

int fs_umount(void)
{
	//Check if the superblock is empty
	if(super_block == NULL){
         return -1;

        }
	//Check
        if(block_Write(0,(void*)&super_block) == -1 ||block_write(super_block->root_dir_idx,&rootdir_array == -1 )){
                return -1;

        }
        if(super_block->total_num_block > (super_block->num_FAT_blocks)* 2048){
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
        int bit_count = 0
        while(bit_count < 128){
                memset(rootdir_array[bit_count]->file_name,0,strlen(rootdir_array[bit_count]->file_name))
                rootdir_array[bit_count]->idx =0;
                rootdir_array[bit_count]->file_Size =0;
                bit_count++;
        }
        free(flat_array);
        super_block->total_num_blocks =0;
        super_block->num_data_blocks = 0;
        super_block->root_dir_idx =0;
        super_block->data_init_idx=0;
        super_block->numt_FAT_blocks =0;
        memset(super_block->signature,'\0',8);
        if(block_disk_close == -1){
                return -1;
        }


        return 0;
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
