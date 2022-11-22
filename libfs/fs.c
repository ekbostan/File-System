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
	int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
	if (fd == -1 || fd > 31 || buf == NULL){
                return -1;
        }

        //read @count bytes of data from the file referenced by file
        //descriptor @fd into buffer pointer by @buf
         char* file_name = fd_arr[fd].file_name;
         int  offset = fd_arr[fd].offset;

         int root_idx = 0 ;
         for(int i =0; i<128; i++){
                if(rootdir_array[i].file_name == file_name){
                        root_idx = i;
                        printf("Index of the file in the root directory:%d\n",root_idx);
                        break;
                 }
         }
	 struct rootdir *file_dir = &rootdir_array[root_idx];
	 int num_use_blocks = 0;
	 int loop_cond = offset % 4096;
	 int loop_var = 0;
	 int fat_idx = file_dir->idx;
	 int fat_iter_var = fat_idx;
	 printf("First while\n");

	while(loop_var < loop_cond){
		      printf("In the First while\n");
		if(fat_iter_var== FAT_EOC)
		{
			return -1;
		}
		fat_iter_var = flat_array[fat_iter_var].data_block;
		loop_var++;
	}

	if(file_dir->file_size ==0){
		num_use_blocks = (offset % 4096)+count;
		num_use_blocks = (num_use_blocks/4096)+1;
	}
	else
	{
		num_use_blocks = (offset + 4096* ((((offset % 4096)+ count)/4096)+1))/4096;
		num_use_blocks = num_use_blocks -1 - (file_dir->file_size/4096);
	
	}	
	int fat_arr[num_use_blocks];
	int z = 0 ;
	int remaining_data_blocks = 0;
	      printf("SECOND  while\n");
	      printf("num use blocks:%d\n",num_use_blocks);
	      printf("super_block->num_data_blocks : %d\n,",super_block->num_data_blocks);
	while(z < super_block->num_data_blocks){
		 printf("In the First while\n");
		 printf("z: %d\n",z);

		if(flat_array[z].data_block ==0){
			fat_arr[remaining_data_blocks] = z;
			remaining_data_blocks++;
			printf("remain data blocks:%d\n",remaining_data_blocks);
		}

			if(num_use_blocks == remaining_data_blocks)
				break;
		z++;
	
	}
int avaliable_num = remaining_data_blocks;
int idx_of_data_block = file_dir->idx;
 printf("idx_of_data_block.data_block %d\n",idx_of_data_block);
if(file_dir->idx != FAT_EOC){
	 printf("Third while\n");
/*	while(idx_of_data_block != FAT_EOC){
		   printf("IN Third while\n");
	idx_of_data_block = flat_array[idx_of_data_block].data_block;
	 printf("idx_of_data_block.data_block %d\n",idx_of_data_block);
	}*/
	int g =0;
	   printf("4th  while\n");
	while(g<avaliable_num){
	   printf("IN THE 4TH WHILEe\n");
   		printf("idx_of_data_block.data_block %d\n",flat_array[idx_of_data_block].data_block);	   
		flat_array[idx_of_data_block].data_block = fat_arr[g];
	idx_of_data_block = flat_array[idx_of_data_block].data_block;
		g++;
	}

flat_array[idx_of_data_block].data_block = FAT_EOC;
}

else{

	fat_idx = fat_arr[0];
	file_dir->idx = fat_idx;
}


int free_blocks = 1;
int num_free_blocks = 0;
while(free_blocks< super_block->num_data_blocks){
	if(flat_array[free_blocks].data_block != 0){
		free_blocks++;
		continue;}
	else{
		num_free_blocks++;
	
	}
	free_blocks++;
}
printf("num_free_blocks%d\n",num_free_blocks);

int to_Write = (((count+(offset%4096)) /4096) + 1);
if(to_Write > num_free_blocks){
	to_Write = num_free_blocks;
}

printf("which block to write: %d \n",to_Write);

int shift_count =0;
char *write_buffer = (char*)buf;
char bounce_buffer[BLOCK_SIZE];
int loopvar = 0;
int count_write = to_Write -1;
while(loopvar< to_Write){
	if (4096 < (fd_arr[fd].offset/4096 + count)){
       		shift_count = count;
   	 }
    else{
        shift_count = 4096 - fd_arr[fd].offset/4096;
    }


   memcpy(bounce_buffer+fd_arr[fd].offset/BLOCK_SIZE, write_buffer, shift_count);
	printf("write index that does not write : %d\n",fat_idx + super_block->data_init_idx);	
  	 block_write(fat_idx + super_block->data_init_idx,(char*)write_buffer);
	 printf("bounce buffer %s\n",(char*)bounce_buffer); 
	 printf("BUF: %s\n",(char*) write_buffer);

   if(loopvar > count_write){
   	flat_array[fat_idx].data_block = FAT_EOC;
	fat_idx = flat_array[fat_idx].data_block;
	if(fat_idx != FAT_EOC){return -1;}
   }
   else{
   flat_array[fat_idx].data_block = flat_array[loopvar +1].data_block;
   fat_idx = flat_array[fat_idx].data_block ;
   }
    int byte_written_count = 0;
    byte_written_count = byte_written_count + shift_count;
	loopvar++;
}

	return 0;

}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
	return 0;
}
