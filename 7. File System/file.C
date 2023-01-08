/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File(FileSystem *_fs, int _id) {
    
    Console::puts("Opening file.\n"); 
    //assert(false);
    
    
    file_CurrPos = 0;
    fs = _fs;
    bool file_block_exist = false;
    
    for (int inode_num=0;inode_num<fs->MAX_INODES;inode_num=inode_num+1){
        
        if (fs->inodes[inode_num].id!=_id){
            continue;
        }
        else{  
            inode_index = inode_num; 
            Console::puts("File found at inode number : ");
            Console::puti(inode_num); Console::puts("\n");
            file_block_exist = true;
        }
    }
    if (file_block_exist=true){
    	blk_num = fs->inodes[inode_index].blk_num;
    	file_length = fs->inodes[inode_index].file_length;
    	fs->disk->read(blk_num,block_cache);
    }
    else{
        assert(false);
    }
}

File::~File() {
    Console::puts("Closing file.\n");
    /* Make sure that you write any cached data to disk. */
    /* Also make sure that the inode in the inode list is updated. */
    
    
    fs->disk->write(blk_num,block_cache);
    fs->inodes[inode_index].file_length = file_length;
    
    Inode * tmp_inodes = fs->inodes;
    unsigned char * tmp = (unsigned char *)tmp_inodes;
    fs->disk->write(1,tmp);  
    
    
}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char *_buf) {
    Console::puts("reading from file\n");
    //assert(false);

    int char_read = 0;
    for (int i=file_CurrPos;i<file_length;i=i+1){
        if (not EoF()){
           _buf[char_read]=block_cache[i];
           char_read++;
        }
        else{
           break;
        }

    }
    return char_read;
}

int File::Write(unsigned int _n, const char *_buf) {
    Console::puts("writing to file\n");
    //assert(false);
    
    
    int char_read = 0;
    int start = file_CurrPos;
    int i=start;
    int currblock_size=SimpleDisk::BLOCK_SIZE;
    while (i< start+ _n){
    	
    	if (i!=currblock_size){

	   block_cache[file_CurrPos] = _buf[char_read];
	   char_read=char_read+1;
	   i=i+1;
	   file_CurrPos=i;
	   file_length=file_length + 1;
        }
        else{
        	break;
        }
        
    }

    return char_read;
    
    
}

void File::Reset() {
    Console::puts("resetting file\n");
    //assert(false);
    file_CurrPos = 0;
}

bool File::EoF() {
    Console::puts("checking for EoF\n");
    //assert(false);
    
    
    if (file_CurrPos<file_length){
        return false;
    }
    return true;
}
