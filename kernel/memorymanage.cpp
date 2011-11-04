#include "memorymanage.h"

MemoryManage::MemoryManage()
{
    //ctor
}

ldt * MemoryManage::get_current_ldt(){

}

static unsigned long MemoryManage::ldt_copy_mem(int index, int current_index){

    unsigned long old_data_base,new_data_base,data_limit;
	unsigned long old_code_base,new_code_base,code_limit;

	ldt_table p_ldt = ldt_array[index];
	ldt_table c_ldt = ldt_array[current_index];

	code_limit=get_limit(0x0f);
	data_limit=get_limit(0x17);//???what's this
	old_code_base = get_base( c_ldt[1]);
	old_data_base = get_base( c_ldt[2]);
	/*
	if (old_data_base != old_code_base)
		panic("We don't support separate I&D");
	if (data_limit < code_limit)
		panic("Bad data_limit");
    //*/
	new_data_base = new_code_base = index * SEG_LENGTH;
	//p -> setStart_code(new_code_base);

	set_base( p_ldt[1], new_code_base);
	set_base( p_ldt[2], new_data_base);

	if (copy_page_tables(old_data_base,new_data_base,data_limit)) {
		printk("free_page_tables: from copy_mem\n");
		free_page_tables(new_data_base,data_limit);
		return -ENOMEM;
	}
	return new_code_base;

}

//well the copy and free op is base on every 4MB.....well, linus is so lasy...an so am I
static bool MemoryManage::copy_page_tables(unsigned long source_base, unsigned long dect_base, unsigned long limit){
    unsigned long * source_table, * dect_table;
	unsigned long source_dir, dect_dir;
/*
	if (from & 0x3fffff)
		panic("free_page_tables called with wrong alignment");
	if (!from)
		panic("Trying to free up swapper memory space");
//*/
	unsigned long size = (limit + 0x3fffff) >> 22;
	source_dir = source_base >> 22;
	dect_dir = dect_base >> 22;
	for ( int j = 0; size > j; j++ ) {
		if (( page_dir[dect_dir + j] ) & 1){
		    panic("dect page is exsit...");
		    return false;
		}

		if (!(1 & page_dir[source_dir + j]))
			continue;
		source_table = (unsigned long *) (0xfffff000 & page_dir[source_dir + j]);

        //the page_table should not exist...we just create it...
        if (!(tmp=get_free_page()))
            return false;
        page_dir[dect_dir + j] = tmp|7;
        dect_table = (unsigned long *) tmp;

		for (int i=0 ; i < ( ( source_base == 0 ) ? 0xA0 : 1024 ) ; i++) {
            current_page = source_table[i];
			if (!(1 & current_page))
				continue;

			current_page &= ~2;    //make it copy on write....
			dect_table[i] = current_page;
			if (current_page > LOW_MEM) {
				source_table[i] = current_page;
				current_page -= LOW_MEM;
				current_page >>= 12;
				mem_map[current_page]++;
			}
		}
	}
	return true;
}

static void MemoryManage::on_process_die(int index){
    ldt_table p_ldt = ldt_array[index];

    free_page_tables(get_base(p_ldt[1]),get_limit(0x0f));
	free_page_tables(get_base(p_ldt[2]),get_limit(0x17));
}





static bool MemoryManage::free_page_tables(unsigned long from,unsigned long size)
{
	unsigned long * pg_table;
	unsigned long dir;
/*
	if (from & 0x3fffff)
		panic("free_page_tables called with wrong alignment");
	if (!from)
		panic("Trying to free up swapper memory space");
//*/
	size = (size + 0x3fffff) >> 22;
	dir = from >> 22;
	for ( int j = 0; size > j; j++ ) {
		if (!(1 & page_dir[dir + j]))
			continue;
		pg_table = (unsigned long *) (0xfffff000 & page_dir[dir + j]);
		for (int i=0 ; i < 1024 ; i++) {
			if (1 & pg_table[i])
				free_page(0xfffff000 & pg_table[i]);
			pg_table[i] = 0;
		}
		free_page(0xfffff000 & page_dir[dir + j]);
		page_dir[dir + j] = 0;
	}
	//invalidate();
	return true;
}


static bool MemoryManage::free_page(unsigned long addr)
{
	if (addr < LOW_MEM) return;
	if (addr >= HIGH_MEMORY)
		panic("trying to free nonexistent page");
	addr -= LOW_MEM;
	addr >>= 12;
	if(memory_map[addr]--){
        return true;
	}
	memory_map[addr] = 0;
	//panic  free free page
	return false;
}

static unsigned long MemoryManage::get_free_page(){
    for(int i = 0; i < PAGING_PAGES; ++i){
        if(memory_map[i] == 0){
            memory_map[i] ++;
             return (i << 12) + LOW_MEM;
        }
    }
    panic("no more free memory !");
    return 0;
    /*
register unsigned long __res asm("ax");

__asm__("std ; repne ; scasb\n\t"
	"jne 1f\n\t"
	"movb $1,1(%%edi)\n\t"
	"sall $12,%%ecx\n\t"
	"addl %2,%%ecx\n\t"
	"movl %%ecx,%%edx\n\t"
	"movl $1024,%%ecx\n\t"
	"leal 4092(%%edx),%%edi\n\t"
	"rep ; stosl\n\t"
	"movl %%edx,%%eax\n"
	"1:"
	:"=a" (__res)
	:"0" (0),"i" (LOW_MEM),"c" (PAGING_PAGES),
	"D" (mem_map+PAGING_PAGES-1)
	);
return __res;
*/
}

static void MemoryManage::put_page(unsigned long liner_address)
{
    int physical_address = get_free_page();

    write_page_table(liner_address, physical_address);
}


static bool MemoryManage::write_page_table(unsigned long address, unsigned long page){

	unsigned long tmp, *page_table, table_item_offset;

/* NOTE !!! This uses the fact that _pg_dir=0 */

	if (page < LOW_MEM || page >= HIGH_MEMORY)
		printk("Trying to put page %p at %p\n",page,address);
	if (memory_map[(page-LOW_MEM) >> 12] != 1)
		printk("memory_map disagrees with %p at %p\n",page,address);

    //get the high 10 bit, its the offset of the liner address's page_table in the page_dir
	table_item_offset = address >> 22;
	if (( page_dir[table_item_offset] ) & 1)
		page_table = (unsigned long *) (0xfffff000 & page_dir[table_item_offset]);
	else {
	    //when the page_table is not exist...wo should create it...
		if (!(tmp=get_free_page()))
			return false;
		page_dir[table_item_offset] = tmp|7;
		page_table = (unsigned long *) tmp;
	}
	// the 10 bit in the middle is the offset of the table_item
	//associated with the address in the page_table
	// >> 12 then left the high 20 bit, &0x3ff then left the middle 10 bits...the high 10 is set 0
	page_table[(address>>12) & 0x3ff] = page | 7;
	//well the page_table is somthing to save which physical page is associated with one liner address...
	//so the value in the item of the page_table witch is correspond to the linear address
	//obviously is the physical page's id, the physical address this time..
	/* no need for invalidate */
	return true;
}
