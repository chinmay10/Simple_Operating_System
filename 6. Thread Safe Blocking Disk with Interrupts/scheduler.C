/*
 File: scheduler.C
 
 Author:CHINMAY AJIT SAWKAR
 Date  :5 NOV 2022
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
   //assert(false);
   readyQ_len=0;
   Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
   if (Machine::interrupts_enabled())	
	Machine::disable_interrupts();	
	
   //assert(false);
   if (readyQ_len > 0) {
   	Thread* nxt_th = readyQ.pop_thread();
   	Thread::dispatch_to(nxt_th);
   	readyQ_len--;
   } 
   else {   	
   	Console::puts("Ready queue is empty, no more threads to dispatch \n");
   }
   if(!Machine::interrupts_enabled())
	Machine::enable_interrupts();
}

void Scheduler::resume(Thread * _thread) {
   if (Machine::interrupts_enabled())	
	Machine::disable_interrupts();
   //assert(false);
   //inserting element in ready queue
   readyQ.insert_thread(_thread);
   //increasing the length of ready queue
   readyQ_len++;
   if(!Machine::interrupts_enabled())
	Machine::enable_interrupts();
}

void Scheduler::add(Thread * _thread) {
   if (Machine::interrupts_enabled())	
   	Machine::disable_interrupts();
   //assert(false);
   //inserting element in ready queue
   readyQ.insert_thread(_thread);
   //increasing the length of ready queue
   readyQ_len++;
   
   if(!Machine::interrupts_enabled())
	Machine::enable_interrupts();
}

void Scheduler::terminate(Thread * _thread) {

   if (Machine::interrupts_enabled())	
   	Machine::disable_interrupts();
   //assert(false);
   Console::puts("A thread is terminated \n");
   readyQ.remove_thread(_thread, readyQ_len);
   Scheduler::yield();
   if(!Machine::interrupts_enabled())
	Machine::enable_interrupts();
}
	
      

