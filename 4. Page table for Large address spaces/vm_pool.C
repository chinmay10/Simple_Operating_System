/*
 File: vm_pool.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "page_table.H"

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
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
	base_address = _base_address;
	size = _size;
	frame_pool = _frame_pool;
	page_table = _page_table;
	vm_pool_next_ptr= NULL;
	region_count= 0;

	page_table->register_pool(this);

	virtual_memory_region *region_pt = (virtual_memory_region*)base_address;
	region_pt[0].base_address= base_address;
	region_pt[0].length= PageTable::PAGE_SIZE;
	regions = region_pt;
	remaining_size-=PageTable::PAGE_SIZE; 
	region_count++; 

	Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) {
	 
	if(_size > remaining_size)
	{
	Console::puts("Not enough space \n");
	assert(false);
	}
	unsigned long no_pages = (_size /PageTable::PAGE_SIZE) + (( _size %PageTable::PAGE_SIZE) > 0 ? 1 : 0);
	regions[region_count].base_address = regions[region_count-1].base_address +  regions[region_count-1].length;
	regions[region_count].length = no_pages*PageTable::PAGE_SIZE;
	region_count++;
	remaining_size-=no_pages*PageTable::PAGE_SIZE;

	Console::puts("Allocated region of memory.\n");
	return regions[region_count-1].base_address; 
}

void VMPool::release(unsigned long _start_address) {
	/*Find the region in which address is present. */
	int region = -1;
	for(int	 i=1;i<region_count;i++)
	{
	    if(regions[i].base_address  == _start_address)
		   region =i;	
	}	 	
	unsigned long no_pages = regions[region].length / PageTable::PAGE_SIZE ;
	while(no_pages >0)
	{
	page_table->free_page(_start_address);
		no_pages--;
		_start_address+=PageTable::PAGE_SIZE;
	}
	for(int i = region; i<region_count;i++ )
			regions[i]=regions[i+1];	
	region_count--;
	remaining_size+=regions[region].length;

	Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) {
	
	if((_address > (base_address + size)) || (_address <  base_address))
	 return false;
	return true;
	Console::puts("Checked whether address is part of an allocated region.\n");
}

