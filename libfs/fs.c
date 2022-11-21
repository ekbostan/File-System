// FS Info:
// total_blk_count=8198
// fat_blk_count=4
// rdir_blk=5
// data_blk=6
// data_blk_count=8192
// fat_free_ratio=8191/8192
// rdir_free_ratio=128/128

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"


#define FAT_EOC 0XFFFF

// /* TODO: Phase 1 
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
struct __attribute__ ((__packed__)) fd{
        char file_name[16];
        uint16_t offset;
        int idx;

};


struct superblock* super_block;
struct FAT* flat_array;
struct rootdir rootdir_array[128];
char signature[8]="ECS150FS";
struct fd fd_arr[32];
int fd_count =0;

int file_count = 0;

int fs_mount(const char *diskname)
{ 
	if(block_disk_open(diskname) == -1){
            return -1;
	}
	super_block = malloc(4096);
	
	if(block_read(0, (void*)super_block) == -1){
			return -1;
	}
	if(block_disk_count() != super_block->total_num_blocks){
		return -1;
	}
	if(memcmp(super_block->sign,"ECS150FS",8) != 0){
		return -1;
	}
	flat_array = malloc((super_block->num_FAT_blocks) * 4096 * sizeof(uint16_t));
	if(flat_array == NULL){
			return -1;
	}
	for (int i = 0; i < super_block->num_FAT_blocks;i++){
		if(block_read(i+1, &flat_array[i*2048]) == -1){

			return -1;
		}
		
	}
	if(block_read(super_block->root_dir_idx,&rootdir_array) == -1){
			return -1;
   	}

    return 0;

}


int fs_umount(void)
{	
	if(super_block == NULL){
         return -1;

        }
	//Check
        if(block_write(0,(void*)super_block) == -1 ||block_write(super_block->root_dir_idx,rootdir_array) == -1){
                return -1;
        }
	
    //    if(super_block->total_num_blocks != (super_block->num_FAT_blocks* 2048)){
      //           return -1;
  //       }
	
        for (int i = 0; i < super_block->num_FAT_blocks;i++){
                 if(block_read(i+1, &flat_array[i*2048]) != -1){

                         continue;
                 }
                 else
                         return -1;
        }

        memset(rootdir_array[0].file_name,0,16);
        rootdir_array[0].idx = 0;
        rootdir_array[0].file_size =0;
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
	return 0;
}


int fs_info(void)
{
	// FS Info:
	// total_blk_count=8198
	// fat_blk_count=4
	// rdir_blk=5
	// data_blk=6
	// data_blk_count=8192
	// fat_free_ratio=8191/8192
	// rdir_free_ratio=128/128
    printf("FS Info:\n");
    printf("total_blk_count=%d\n",super_block->total_num_blocks);
    printf("fat_blk_count=%d\n",super_block->num_FAT_blocks);
    printf("rdir_blk=%d\n",super_block->root_dir_idx);
    printf("data_blk=%d\n",super_block->data_init_idx);
    printf("data_blk_count=%d\n",super_block->num_data_blocks);
	
    // Cuz we are getting you can not compare a struct with an integer at the line flat_array[i] == 0
    int fat_counter = 0;
    int i = 0, while_counter = 0;
 	while(while_counter < super_block->num_data_blocks)
	{
		//printf("enteered while loop\n");
		if(flat_array[i].data_block == 0)
		{
			//printf("data block is 0 %d\n", i);
			fat_counter = fat_counter + 1;
		}
		i++;
		while_counter++;
	}
  	printf("fat_free_ratio=%d/%d\n", fat_counter, super_block->num_data_blocks);
	//printf("root dir %s\n", rootdir_array[0].file_name);
	int root_num = 0;
	while_counter = 0;
	while(while_counter<128) 
	{
		//printf("while loop\n");
		if(rootdir_array[while_counter].file_size == 0)
		{
			//printf("while counter\n");
			root_num = root_num + 1;
		}
		while_counter++;
	} 
	printf("rdir_free_ratio=%d/%d\n", root_num,128);
	//printf("root dir %s\n", rootdir_array[0]->file_name);
	return 0;
}



int fs_create(const char *filename)
{
	if(filename == NULL){
		return -1;
	}
	if(strlen(filename)>16){
		return -1;
	}
	
	int counter =0;
	  while(counter < 128){
                if(strcmp(rootdir_array[counter].file_name,filename) == 0){
			
                        return -1;
                }
                else{
			
                        counter++;
                }
	  }
	counter = 0;
	while(counter < 128){
		if(*rootdir_array[counter].file_name == '\0'){
			strcpy(rootdir_array[counter].file_name, filename);
			rootdir_array[counter].file_size = 0;
			rootdir_array[counter].idx =FAT_EOC ;
			counter++;
			break;
		}
		else{
			counter++;
		}
	}
	return 0;
}

int fs_delete(const char *filename)
{
 int counter =0;
 int fat_array_idx;
 int present = 0;

          while(counter < 128){
                if(strcmp(rootdir_array[counter].file_name,filename) == 0){
		      
			memset(rootdir_array[counter].file_name, '\0',strlen(filename));
			rootdir_array[counter].file_size = 0;
                     	fat_array_idx = rootdir_array[counter].idx;
			present =1 ;
			
		       	break;
                }
                else{
                       
                        counter++;
                }
          }
	  if(present != 1 ){return -1;}
	  
	  
	  rootdir_array[counter].idx = 0;
	  int flat_arr_next_idx = 0;
	  //printf("Before while\n");
	  while(flat_arr_next_idx != FAT_EOC){
	  	flat_arr_next_idx = flat_array[fat_array_idx].data_block;	
		flat_array[fat_array_idx].data_block = 0;
		fat_array_idx = flat_arr_next_idx;
		//printf("Infinite loop\n");
	  }
	return 0;
}

int fs_ls(void)
{
	/* TODO: Phase 2 */
//	fs_create("EROL");
	
//	fs_create("EROL");
	//fs_delete("EKB1");
	//fs_create("EKB2");


		int i =0;
		while(i<128){
		printf("file number%d : %s\n",i ,rootdir_array[i].file_name);
		i++;
		}

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
	char *file_name = fd_arr[fd].file_name;
        if(fd == -1){
                return -1;
        }
        if(fd >31){
                return -1;
        }
        int j =0;
        while(j<31){
                if(strcmp(rootdir_array[j].file_name,file_name) != 0){
                        j++;
                        continue;
                }
                else{
                         return rootdir_array[j].file_size;
                }
        }
        return -1;
}

int fs_lseek(int fd, size_t offset)
{
	if (fd == -1){
                return -1;
        }
        else if (fd > 31){
                return -1;
        }
        else if ((uint16_t*)offset < 0){
                return -1;
        }
        else if (offset > rootdir_array[fd].file_size){
                return -1;
        }
        else{
                //printf("offset before %d\n", fd_arr[fd].offset);
                fd_arr[fd].offset = offset;
               // printf("offset after %d\n", fd_arr[fd].offset);
        }

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
