/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/



#define block_size_in_disk SimpleDisk::BLOCK_SIZE


    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"

/*--------------------------------------------------------------------------*/
/* CLASS Inode */
/*--------------------------------------------------------------------------*/

/* You may need to add a few functions, for example to help read and store 
   inodes from and to disk. */

Inode::Inode()
{
  fs = NULL;
  unused_flag = 'y';
  file_length= 0;
  id = -1;
}

/*--------------------------------------------------------------------------*/
/* CLASS FileSystem */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem() {
    Console::puts("In file system constructor.\n");
    //assert(false);
    
    disk = NULL;
    size = 0;
    free_block_count = block_size_in_disk/sizeof(unsigned char);
    free_blocks = new unsigned char[free_block_count];
    //inode_counter= 0;
    inodes = new Inode[MAX_INODES];
}

FileSystem::~FileSystem() {
    Console::puts("unmounting file system\n");
    /* Make sure that the inode list and the free list are saved. */
    //assert(false);
    
    //this is a destructor function 
    disk->write(0,free_blocks);
    unsigned char* tmp = (unsigned char*) inodes;
    disk->write(1,tmp);
}


/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/


bool FileSystem::Mount(SimpleDisk * _disk) {
    Console::puts("mounting file system from disk\n");

    /* Here you read the inode list and the free list into memory */
    
    //assert(false);
    
    disk = _disk;
    unsigned char* temp_char;
    _disk->read(0,free_blocks);
    _disk->read(1,temp_char);
    inodes = (Inode *) temp_char;
    
    for(int i=0;i<100;i++){
    	//disk
 	//time for mounting
    }
    
    return true;
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) { // static!
    Console::puts("formatting disk\n");
    /* Here you populate the disk with an initialized (probably empty) inode list
       and a free list. Make sure that blocks used for the inodes and for the free list
       are marked as used, otherwise they may get overwritten. */
    //assert(false);
    
    unsigned char* free_blk_arr = new unsigned char[_size/block_size_in_disk];
    for (int i=0;i<_size/block_size_in_disk;i++){
    	if (i==0 || i==1){
		free_blk_arr[i] = 'U';
    	}
    	else{
    		free_blk_arr[i]='F';
    		}
    }

    _disk->write(0,free_blk_arr);
    
    Inode* tmp_inodes = new Inode[MAX_INODES];

    _disk->write(1,(unsigned char*) tmp_inodes);
    return true;
}

Inode * FileSystem::LookupFile(int _file_id) {
    Console::puts("looking up file with id = "); Console::puti(_file_id); Console::puts("\n");
    /* Here you go through the inode list to find the file. */
    //assert(false);
    int i=0;
    while (i<MAX_INODES){
    	if( inodes[i].id!= _file_id){
    		i=i+1;
    		continue;
    	}
    	else{
    		return &inodes[i];
    	}
    	i=i+1;
    }
    return NULL;
}

bool FileSystem::CreateFile(int _file_id) {
    Console::puts("creating file with id:"); Console::puti(_file_id); Console::puts("\n");
    /* Here you check if the file exists already. If so, throw an error.
       Then get yourself a free inode and initialize all the data needed for the
       new file. After this function there will be a new file on disk. */
    //assert(false);
    
    if (LookupFile(_file_id)!=NULL){
    	assert(false);
    }
    else{
	    int free_inode_index = -1;
	    bool inode_change_flag=false;
	    for (int i = 0;i<MAX_INODES;i++){
	        if (inodes[i].unused_flag=='n'){
	    	    continue;
	    	}
	    	else{
		    free_inode_index = i;
		    inode_change_flag=true;
		    break;
	        }
	    }
	    
	    int free_block_index = -1;
	    bool free_change_flag=false;
	    for (int i = 0;i<free_block_count;i++){
	      if (free_blocks[i]=='U'){
	          continue;
	      }
	      else if (free_blocks[i]=='F'){
		free_block_index = i;
		free_change_flag=true;
		break;
	      }
	    }
		  

	    if (!free_change_flag && inode_change_flag){
	    	if (!free_change_flag){
	    		Console::puts("no free block found"); Console::puts("\n");
	    	}
	    	else if (!free_change_flag){
	    		Console::puts("free inode block not found"); Console::puts("\n");
	    	}
	    	Console::puts("");
	    	assert(false)
	    }

	    inodes[free_inode_index].unused_flag = 'n';
	    inodes[free_inode_index].fs = this;
	    inodes[free_inode_index].blk_num = free_block_index;
	    inodes[free_inode_index].id = _file_id;
	    free_blocks[free_block_index] = 'U';   
	    return true;
    }
    return true;
}

bool FileSystem::DeleteFile(int _file_id) {
    Console::puts("deleting file with id:"); Console::puti(_file_id); Console::puts("\n");
    /* First, check if the file exists. If not, throw an error. 
       Then free all blocks that belong to the file and delete/invalidate 
       (depending on your implementation of the inode list) the inode. */

    int ino_del_num;
    
    int i=0;
    while(i<MAX_INODES){
	if (inodes[i].id != _file_id){
		i=i+1;
		continue;
	}
	else{		
		
    		free_blocks[inodes[ino_del_num].blk_num] = 'F';
    		Delete_inode_state(i);
    		disk->write(inodes[ino_del_num].blk_num,NULL);
        	return true;
		
	}
    }

    return false;

}

short FileSystem::GetFreeInode()
{
    short inodeNum = 0;
    for (int i = 0; i < MAX_INODES; i=i+1){
	if (inodes[i].unused_flag == 'y')
	{	
		inodeNum = i;
		inodes[i].unused_flag = 'n';
		break;
	}
    }
    return inodeNum;
}

void FileSystem::Delete_inode_state(int del_idx){
	inodes[del_idx].unused_flag = 'y';
    	inodes[del_idx].file_length = 0;
    	inodes[del_idx].id=NULL;
}

