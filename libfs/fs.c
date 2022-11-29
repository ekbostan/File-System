
//The file system is implemented on top of a virtual disk. This virtual disk is actually a simple binary file that is stored on the “real” file system provided by your computer.

//Exactly like real hard drives which are split into sectors, the virtual disk is logically split into blocks. The first software layer involved in the file system implementation is the block API and is provided to you. This block API is used to open or close a virtual disk, and read or write entire blocks from it.

//Above the block layer, the FS layer is in charge of the actual file system management. Through the FS layer, you can mount a virtual disk, list the files that are part of the disk, add or delete new files, read from files or write to file
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
    uint16_t total_num_blocks // Total amount of blocks of virtual disk;
    uint16_t root_dir_idx;//Root directory block index
    uint16_t data_init_idx;//Data block start index
    uint16_t num_data_blocks;//	Amount of data blocks
    uint8_t num_FAT_blocks;//Number of blocks for FAT
    uint8_t padding[4079];//	Unused/Padding
};
//8192 data blocks, the size of the FAT will be 8192 x 2 = 16384 bytes long, thus spanning 16384 / 4096 = 4 blocks.
//The FAT is a flat arraywhich entries are composed of 16-bit unsigned words. 
//There are as many entries as data blocks in the disk.
//first entry of the FAT (entry #0) is always invalid and contains the special FAT_EOC (End-of-Chain)
//contents must be added to the data block start index in order to find the real block number on disk.
// DUTY keeps track of both the free data blocks and the mapping between files and the data blocks holding their content.
struct __attribute__ ((__packed__)) FAT{
    uint16_t data_block; //0 if free
};

//is an array of 128 entries stored in the block following the FAT. Each entry is 32-byte wide
//The entry for an empty file size is 0, index of first data block would be FAT_EOC.

struct __attribute__ ((__packed__)) rootdir{
    char file_name[16];//Filename (including NULL character)
    uint32_t file_size;//Size of the file (in bytes)
    uint16_t idx;//Index of the first data block
    uint8_t padding[10];//Unused/Padding
};
struct __attribute__ ((__packed__)) fd{
        char file_name[16];//Filename (including NULL character)
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
// fs_mount() makes the file system contained in the specified virtual disk “ready to be used”. 
//You need to open the virtual disk, using the block API, and load the meta-information that is necessary to handle the file system operations 
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

//fs_umount() makes sure that the virtual disk is properly closed 
//and that all the internal data structures of the FS layer are properly cleaned.
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

 int present = 0;

          while(counter < 128){
                if(strcmp(rootdir_array[counter].file_name,filename) == 0){
		      
			memset(rootdir_array[counter].file_name, '\0',strlen(filename));
			rootdir_array[counter].file_size = 0;
			present =1 ;
			
		       	break;
                }
                else{
                       
                        counter++;
                }
          }
	  if(present != 1 ){return -1;}
	  
	  
	  rootdir_array[counter].idx = 0;
	 int data_block_count = rootdir_array[counter].file_size*2/4096;

 //There are as many entries as data blocks in the disk.
        for (int j = 0; j < data_block_count; j++){
                if (flat_array[j].data_block != FAT_EOC){
                        flat_array[j].data_block = 0;
                }
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
	   int i = 0;
        if (filename == NULL || strlen(filename) > 16){
                return -1;
        }
        if(strcmp(filename,"") == 0){
                return -1;
        }
        if(fd_count >31){
                return -1;

        }
        while (i < 128) {
                if(rootdir_array[i].file_name != filename){
                        
                        break;
                }
                i++;
        }
        int j = 0;

        while(j<31){
                if (strcmp(fd_arr[j].file_name,"")==0){
                        fd_arr[j].idx = j;
                        fd_arr[j].offset = 0;
                        strcpy(fd_arr[j].file_name,filename);
                     
                        return fd_arr[j].idx;
                }
                j++;
        }
        fd_count++;

        return -1;
	
}

int fs_close(int fd)
{
	   if(fd == -1){
                return -1;
        }
        if(fd >31){
                return -1;
        }
        else{
                fd_arr[fd].idx = -1;
                fd_arr[fd].offset = 0;
                strcpy(fd_arr[fd].file_name,"");
        }

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
	
	return 0;

}

int fs_read(int fd, void *buf, size_t count)
{
	
	// First get the offstt calcualte the num blocks and num bytes to read.
        size_t offset = fd_arr[fd].offset;

         void *bounce_buffer = malloc(4096);
         memset(bounce_buffer, 0, 4096);

        int block_idx = offset / 4096;

        int byt_num = offset % 4096;

        int read_num_count;
        int shift_count = 0;
        char *buffer_read = (char *)buf;

        int fat_idx = 0;
	
	
	// Check if FD is valid or not
        if (fd == -1 || fd > 31 || buf == NULL|| count<= 0){
                        return -1;
        }
        int root_dir_number;
	
	//Find the fd witihin the rootdirectory entries
        for(int i = 0; i<128; i++){

                if(strcmp(rootdir_array[i].file_name, fd_arr[fd].file_name) == 0){
                        root_dir_number = i;
                        break;
                }
        }

                read_num_count = count;

	//calcualte the numb of lbocks to reAD/
        size_t num_blocks_to_read = 1+ (count / 4096);
        int j = 0;
	
        int fat_loop_idx = rootdir_array[root_dir_number].idx;
       //block idx is offset/4096 how many block are in there
	// In this while loop we figure out the idx of the data block that we are going to read
	while(j < block_idx){

                if(rootdir_array[root_dir_number].idx != FAT_EOC){
                        fat_loop_idx = flat_array[fat_loop_idx].data_block;
                }
                else{
                        return -1;

                }
                j++;
        }

        fat_idx = fat_loop_idx;
        int z = 0;
	//  num_blocks_to_read = 1+ (count / 4096); we already calcualte that
        while(z < num_blocks_to_read){
		// Block size is bigger number of blocks to read + count
                if(BLOCK_SIZE >= num_blocks_to_read + read_num_count){
                        shift_count = count;
                 }
                 else{
			 // Block size is less than number of bytes to read
                        shift_count = BLOCK_SIZE - read_num_count;
                 }
                 block_read(super_block->data_init_idx + fat_idx, (void*)bounce_buffer);
                 memcpy(buffer_read, bounce_buffer + byt_num,shift_count);
                 z++;
        };
        fd_arr[fd].offset = fd_arr[fd].offset + count;
        return count;
	


	

}
