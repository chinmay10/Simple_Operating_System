/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */


/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "scheduler.H"
#include "simple_disk.H"
#include "thread.H"

extern Scheduler * SYSTEM_SCHEDULER;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/
char flag;
char check_disk_locking(char *flag) 
{
    char check_flag = *flag;
    *flag = 'L';
    if (check_flag=='L'){
    	return true;
    	}
    else{
    	return false;
    	}
}

void flag_init(char *flag) 
{
   *flag = 'U';
}

void unlock_disk()
{
   flag = 'U';
}

void lock_disk() 
{
   while(check_disk_locking(&flag));
}



BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
  flag_init(&flag);
  #ifdef INTERRUPTS_ENABLED
  diskQ_len=0;
  #endif
}

void BlockingDisk::wait_until_ready()
{
	if (! SimpleDisk::is_ready()) 
    {		
    		Console::puts("disk is not yet ready: ");  Console::puts("\n");
    		#ifndef INTERRUPTS_ENABLED
    		SYSTEM_SCHEDULER->resume(Thread::CurrentThread());
    		#else
    		diskQ.insert_thread(Thread::CurrentThread());		
		#endif
		
		SYSTEM_SCHEDULER->yield();
	}
}


#ifdef INTERRUPTS_ENABLED
void BlockingDisk::handle_interrupt(REGS *_r) {
    Thread* nxt_th = diskQ.pop_thread();
    SYSTEM_SCHEDULER->resume(nxt_th);
}
#endif

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  lock_disk();
  SimpleDisk::read(_block_no, _buf);
  Console::puts("***reading from disk on block"); Console::puti(_block_no); Console::puts(" ***\n");
  unlock_disk();

}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  lock_disk();
  SimpleDisk::write(_block_no, _buf);
  Console::puts("***writing on disk on block "); Console::puti(_block_no); Console::puts(" ***\n");
  unlock_disk();
}
