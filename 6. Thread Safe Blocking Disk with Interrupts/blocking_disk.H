/*
     File        : blocking_disk.H

     Author      : 

     Date        : 
     Description : 

*/

#ifndef _BLOCKING_DISK_H_
#define _BLOCKING_DISK_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "simple_disk.H"
#include "thread.H"
#include "utils.H"
#include "interrupts.H"
/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* B l o c k i n g D i s k  */
/*--------------------------------------------------------------------------*/

class DQueue {
    private:
        DQueue*      q_nxt;
        Thread*     th;
        

    public:
        DQueue() {
            	th  = NULL;
            	q_nxt   = NULL;
        }
	
        DQueue(Thread * input_thread ){
        	th = input_thread;
            	q_nxt   = NULL;
            	
        }

    
    void insert_thread (Thread * input_thread) {
        if (th != NULL) {
	    if (q_nxt != NULL) {
               q_nxt->insert_thread(input_thread);
            } 
            else {
              	q_nxt = new DQueue(input_thread);
            }
            
        } 
        else  {
	    th=input_thread;
        }
        
    }

    Thread* pop_thread(){
        if (th==NULL){
            return NULL;
        }
        //storing value to pop
        Thread* popTh = th;
        
        if (q_nxt==NULL){           
            th = NULL;
        }
        else {
            
            th = q_nxt->th;
            DQueue* to_delete = q_nxt;
            q_nxt = q_nxt->q_nxt;
            delete (void *)to_delete;
             
        }
        return popTh;
    }

};



class BlockingDisk : public SimpleDisk {

   DQueue diskQ; // creates DQueue object for the diskQ
   int diskQ_len;
   
protected:
      virtual void wait_until_ready();
public:

   void handle_interrupt(REGS *_r);
   
   BlockingDisk(DISK_ID _disk_id, unsigned int _size); 
   /* Creates a BlockingDisk device with the given size connected to the 
      MASTER or SLAVE slot of the primary ATA controller.
      NOTE: We are passing the _size argument out of laziness. 
      In a real system, we would infer this information from the 
      disk controller. */

   /* DISK OPERATIONS */

   virtual void read(unsigned long _block_no, unsigned char * _buf);
   /* Reads 512 Bytes from the given block of the disk and copies them 
      to the given buffer. No error check! */

   virtual void write(unsigned long _block_no, unsigned char * _buf);
   /* Writes 512 Bytes from the buffer to the given block on the disk. */

};

#endif
