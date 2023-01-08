/* 
    Author: R. Bettati, Joshua Capehart
            Department of Computer Science
            Texas A&M University
			
	    A thread scheduler.

*/
#ifndef SCHEDULER_H
#define SCHEDULER_H


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "thread.H"
#include "utils.H"
/*--------------------------------------------------------------------------*/
/* !!! IMPLEMENTATION HINT !!! */
/*--------------------------------------------------------------------------*/
/*
    One way to proceed is to implement the FIFO scheduling policy inside
    class 'Scheduler'. 

    If you plan to implement a Round-Robin Scheduler, derive it from class
    'Scheduler', say as class 'RRScheduler'. The class 'RRScheduler' is 
    really just a FIFO scheduler with THREE MODIFICATIONS:
    1. It manages a timer, which fires at the end-of-quantum (EOQ). 
    (For details on how to set up a timer and how to handle timer interrupts 
    see the 1-second timer in 'kernel.C'.)  The timer is set up in the
    constructor.
    2. It uses an additional function, the EOQ handler. This function gets
    called whenever an EOQ timer event fires. The EOQ handler forces the 
    current thread to call the scheduler's 'yield' function.
    3. The 'yield' function must be modified to account for unused quantum
    time. If a thread voluntarily yields, the EOQ timer must be reset in order
    to not penalize the next thread.
 
    (Note that this qualifies as programming at about the level of a baboon.
     Much better woudl be to have the abstract class 'Scheduler' implement 
     the basic scheduling MECHANISMS and provide abstract funtions to define
     the queue management POLICIES in derived classes, 
     such as 'FIFOScheduler'.)
    
 */



// Class Queue defined for implementing FIFO scheduler



class Queue {
    private:
        Queue*      q_nxt;
        Thread*     th;
        

    public:
        Queue() {
            	th  = NULL;
            	q_nxt   = NULL;
        }
	
        Queue(Thread * input_thread ){
        	th = input_thread;
            	q_nxt   = NULL;
            	
        }

    
    void insert_thread (Thread * input_thread) {
        if (th != NULL) {
	    if (q_nxt != NULL) {
               q_nxt->insert_thread(input_thread);
            } 
            else {
              	q_nxt = new Queue(input_thread);
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
            Queue* to_delete = q_nxt;
            q_nxt = q_nxt->q_nxt;
            delete (void *)to_delete;
             
        }
        return popTh;
    }

    void remove_thread(Thread * _thread, int &RQ_len){
    	int cnt=0;
	while(cnt< RQ_len){
		Thread * curr_th = pop_thread();
		if (_thread->ThreadId() != curr_th->ThreadId()){
		   insert_thread(curr_th);
		}
		else{
		   RQ_len--;
		}
		cnt++;
	}
    }
};




/*--------------------------------------------------------------------------*/
/* SCHEDULER */
/*--------------------------------------------------------------------------*/

class Scheduler {

  /* The scheduler may need private members... */
   Queue readyQ; // creates Queue object
   int readyQ_len; 
public:

   Scheduler();
   /* Setup the scheduler. This sets up the ready queue, for example.
      If the scheduler implements some sort of round-robin scheme, then the 
      end_of_quantum handler is installed in the constructor as well. */

   /* NOTE: We are making all functions virtual. This may come in handy when
            you want to derive RRScheduler from this class. */
  
   virtual void yield();
   /* Called by the currently running thread in order to give up the CPU. 
      The scheduler selects the next thread from the ready queue to load onto 
      the CPU, and calls the dispatcher function defined in 'Thread.H' to
      do the context switch. */

   virtual void resume(Thread * _thread);
   /* Add the given thread to the ready queue of the scheduler. This is called
      for threads that were waiting for an event to happen, or that have 
      to give up the CPU in response to a preemption. */

   virtual void add(Thread * _thread);
   /* Make the given thread runnable by the scheduler. This function is called
      after thread creation. Depending on implementation, this function may 
      just add the thread to the ready queue, using 'resume'. */

   virtual void terminate(Thread * _thread);
   /* Remove the given thread from the scheduler in preparation for destruction
      of the thread. 
      Graciously handle the case where the thread wants to terminate itself.*/
  
};
	
	

#endif
