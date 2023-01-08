#define PAGE_DIR_FRAME_SIZE 1
#define PG_DIR_BIT	22
#define PDE_MASK	0xFFFFF000

#define PRESENT_BIT	1
#define WRITE_BIT      2
#define LEVEL_BIT      4


#define PG_TAB_BIT     12
#define PG_TAB_MASK	0x3FF

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



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
	//assert(false);
	PageTable::kernel_mem_pool = _kernel_mem_pool;
	PageTable::process_mem_pool = _process_mem_pool;
	PageTable::shared_size = _shared_size;

	Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
	//assert(false);

	page_directory = (unsigned long *)(kernel_mem_pool->get_frames (PAGE_DIR_FRAME_SIZE)*PAGE_SIZE);

	unsigned long adr_mask = 0;
	unsigned long * direct_mem = (unsigned long *)(kernel_mem_pool->get_frames (PAGE_DIR_FRAME_SIZE)*PAGE_SIZE);

	unsigned long shared_frm = ( PageTable::shared_size / PAGE_SIZE);

	for(int i = 0; i < shared_frm; i++) {
		direct_mem[i] = adr_mask | WRITE_BIT | PRESENT_BIT ;
		adr_mask += PAGE_SIZE;
	}

	//shared memory 
	page_directory[0] = (unsigned long)direct_mem | WRITE_BIT | PRESENT_BIT;

	adr_mask = 0;
	
	//non shared memory 
	for(int i = 1; i< shared_frm; i++) {
	page_directory[i] = adr_mask | WRITE_BIT;
	}

	Console::puts("Constructed Page Table object\n");
   
}


void PageTable::load()
{
   //assert(false);

    current_page_table = this;
    write_cr3((unsigned long)page_directory);
    Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
	//assert(false);

	paging_enabled = 1;

	write_cr0(read_cr0() | 0x80000000);

	Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
	//assert(false);

	
	unsigned long * cur_pg_dir = (unsigned long *) read_cr3();
	unsigned long pg_adr = read_cr2();
	unsigned long pg_dir_adr   = pg_adr >> PG_DIR_BIT;
	unsigned long pg_tab_adr   = pg_adr >> PG_TAB_BIT;
	unsigned long * page_table = NULL;
	unsigned long error_code = _r->err_code;
	unsigned long adr_mask = 0;


	if ((error_code & PRESENT_BIT) == 0 ) {
	if ((cur_pg_dir[pg_dir_adr] & PRESENT_BIT ) == 1) {  
	    page_table = (unsigned long *)(cur_pg_dir[pg_dir_adr] & PDE_MASK);
	    page_table[pg_tab_adr & PG_TAB_MASK] = (PageTable::process_mem_pool->get_frames
		                                (PAGE_DIR_FRAME_SIZE)*PAGE_SIZE) | WRITE_BIT | PRESENT_BIT ;

	} else {
	    cur_pg_dir[pg_dir_adr] = (unsigned long) ((kernel_mem_pool->get_frames (PAGE_DIR_FRAME_SIZE)*PAGE_SIZE) | WRITE_BIT | PRESENT_BIT);

	    page_table = (unsigned long *)(cur_pg_dir[pg_dir_adr] & PDE_MASK);

	    for (int i = 0; i<1024; i++) {
		page_table[i] = adr_mask | LEVEL_BIT ; 
	    }

	    page_table[pg_tab_adr & PG_TAB_MASK] = (PageTable::process_mem_pool->get_frames (PAGE_DIR_FRAME_SIZE)*PAGE_SIZE) | WRITE_BIT | PRESENT_BIT ;

	}
	}

	Console::puts("handled page fault\n");
}

