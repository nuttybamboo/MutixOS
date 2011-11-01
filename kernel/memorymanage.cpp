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

static void MemoryManage::on_process_die(int index){
    ldt_table * p_ldt = ldt_array[index];

    free_page_tables(get_base((*p_ldt)[1]),get_limit(0x0f));
	free_page_tables(get_base((*p_ldt)[2]),get_limit(0x17));
}





static void MemoryManage::free_page_tables(unsigned long from,unsigned long size)
{
	unsigned long * pg_table;
	unsigned long * dir, nr;
/*
	if (from & 0x3fffff)
		panic("free_page_tables called with wrong alignment");
	if (!from)
		panic("Trying to free up swapper memory space");
//*/
	size = (size + 0x3fffff) >> 22;
	dir = (unsigned long *) ((from>>20) & 0xffc); /* _pg_dir = 0 */
	for ( ; size-->0 ; dir++) {
		if (!(1 & *dir))
			continue;
		pg_table = (unsigned long *) (0xfffff000 & *dir);
		for (nr=0 ; nr<1024 ; nr++) {
			if (1 & *pg_table)
				free_page(0xfffff000 & *pg_table);
			*pg_table = 0;
			pg_table++;
		}
		free_page(0xfffff000 & *dir);
		*dir = 0;
	}
	invalidate();
	return 0;
}


static void MemoryManage::free_page(unsigned long addr)
{
	if (addr < LOW_MEM) return;
	if (addr >= HIGH_MEMORY)
		panic("trying to free nonexistent page");
	addr -= LOW_MEM;
	addr >>= 12;
	if(mem_map[addr]--){
        return;
	}
	mem_map[addr] = 0;
	//panic  free free page
	return;
}


static unsigned long MemoryManage::get_free_page()
{
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
}


static void MemoryManage::write_page_table(unsigned long address, unsigned long page){

	unsigned long tmp, *page_table;

/* NOTE !!! This uses the fact that _pg_dir=0 */

	if (page < LOW_MEM || page >= HIGH_MEMORY)
		printk("Trying to put page %p at %p\n",page,address);
	if (mem_map[(page-LOW_MEM)>>12] != 1)
		printk("mem_map disagrees with %p at %p\n",page,address);


	page_table = (unsigned long *) ((address>>20) & 0xffc);
	if ((*page_table)&1)
		page_table = (unsigned long *) (0xfffff000 & *page_table);
	else {
		if (!(tmp=get_free_page()))
			return 0;
		*page_table = tmp|7;
		page_table = (unsigned long *) tmp;
	}
	page_table[(address>>12) & 0x3ff] = page | 7;
/* no need for invalidate */
	return;
}
