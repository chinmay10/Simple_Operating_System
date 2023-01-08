#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;
VMPool  * PageTable::VMPoolList_HEAD = NULL;

void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
	
	Console::puts("Initialized Paging System\n");
	kernel_mem_pool = _kernel_mem_pool;
	process_mem_pool = _process_mem_pool;
	shared_size = _shared_size;
}

PageTable::PageTable()
{
	Console::puts("Constructed Page Table object\n");
	
	page_directory =  (unsigned long *)(kernel_mem_pool->get_frames(1) * PAGE_SIZE);
	page_directory[1023] = (unsigned long)(page_directory )| 3; // recursive page table. mark it as valid
	
	unsigned long *page_table = (unsigned long *) (process_mem_pool->get_frames(1) * PAGE_SIZE);
	unsigned long addr = 0;
	for(unsigned int i =0; i<1024; i++)
	{
		   page_table[i] = addr |3;  
		   addr += PAGE_SIZE;        
	}
	   
	/*Setting up Page Directory Entries */
	page_directory[0] = (unsigned long)page_table;  
	page_directory[0] = page_directory[0] |3 ;
   
	/* Entry 1023 and 0 are already set*/   
   	for(unsigned int i = 1; i<1023; i++)
   	{
		 page_directory[i]= 0|2;  
   	}   
  
	paging_enabled = 0;	
	
}


void PageTable::load()
{
    
	Console::puts("Loaded page table\n");
	current_page_table = this;
	
	write_cr3((unsigned long)(current_page_table->page_directory));     //defining PTBR
}

void PageTable::enable_paging()
{
	//assert(false);
	Console::puts("Enabled paging\n");
	write_cr0(read_cr0() | 0x80000000);
	paging_enabled = 1;
}

void PageTable::handle_fault(REGS * _r)
{
	unsigned long addr = read_cr2();    
	

	//checking legitimacy of logical addr
	unsigned int address_legit = 0;
	VMPool *ptr = PageTable::VMPoolList_HEAD;
	
	for(;ptr!=NULL;ptr=ptr->vm_pool_next_ptr)
	{
		if(ptr->is_legitimate(addr) == true)
		{
			address_legit = 1;
		    break;
		}
	}

	if(address_legit == 0 && ptr!= NULL)
	{
	Console::puts("INVALID ADDRESS \n");
	assert(false);	  	
	}

	unsigned long* ptr_page_dir = current_page_table->page_directory;

	unsigned long obtained_page_dir_index = addr >>22;
	unsigned long obtained_page_table_index = (addr & (0x03FF << 12) ) >>12 ;

	unsigned long *page_table; 
	unsigned long *page_table_entry; 
	
	//recusive page table implementation
	if((ptr_page_dir[obtained_page_dir_index] & 1) == 0) 
	{
	    
	   page_table = (unsigned long *)(process_mem_pool->get_frames(1) * PAGE_SIZE);
	   unsigned long *dir_ent = (unsigned long *)(0xFFFFF<<12);               
	   dir_ent[obtained_page_dir_index] = (unsigned long)(page_table)|3;
	}

	page_table_entry = (unsigned long *) (process_mem_pool->get_frames(1) * PAGE_SIZE);
	unsigned long *page_entry = (unsigned long *)((0x3FF<< 22)| (obtained_page_dir_index <<12)); 
	page_entry[obtained_page_table_index] = (unsigned long)(page_table_entry)|3;
			
	Console::puts("handled page fault\n");
}

void PageTable::register_pool(VMPool * _vm_pool)
{
	

	if( PageTable::VMPoolList_HEAD == NULL ) 
	{
	PageTable::VMPoolList_HEAD= _vm_pool;
	}
	else 
	{
		VMPool *ptr = PageTable::VMPoolList_HEAD;
	for(;ptr->vm_pool_next_ptr!=NULL;ptr= ptr->vm_pool_next_ptr);
	ptr->vm_pool_next_ptr= _vm_pool;
	}

	Console::puts("registered VM pool\n");
}

void PageTable::free_page(unsigned long _page_no) {
	// freeing the pages by marking invalid and also flushing TLB
	unsigned long obtained_page_dir_index =  ( _page_no & 0xFFC)>> 22; 
	unsigned long obtained_page_table_index = (_page_no & 0x003FF ) >>12 ;

	unsigned long *page_table_entry= (unsigned long *) ( (0x000003FF << 22) | (obtained_page_dir_index << 12) );
	unsigned long frm_number = (page_table_entry[obtained_page_table_index] & 0xFFFFF000)/ PAGE_SIZE;

	process_mem_pool->release_frames(frm_number);
	page_table_entry[obtained_page_table_index] |= 2; 
	Console::puts("freed page\n");

	
	load();
}
