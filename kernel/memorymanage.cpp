#include "../include/memorymanage.h"


void inline MemoryManage::set_GDT(unsigned long address){
    struct _gdt_desc{unsigned long a; unsigned long b;} gdt_desc;
    gdt_desc.a = ((unsigned long)(256 * 7 - 1)) << 16;
    gdt_desc.b = (unsigned long)address;
    char * gdt_desc_pointor = (char*)(unsigned long)(&gdt_desc);
    gdt_desc_pointor +=2;

    g_cpu.lgdt((long)gdt_desc_pointor);
    /*
   __asm__(
        "lgdt %0\n\t"
        ::"m" (*gdt_desc_pointor):
        );
        */
}

#define set_page_dir(address)    {\
        __asm__(\
            "movl %%eax %%cr3\n\t"\
            ::"a" (address):\
            );\
}

#define start_paging()  {\
    __asm__(\
            "movl %%cr0 %%eax\n\t"\
            "orl $0x80000000 %%eax\n\t"\
            "movl %%eax %%cr0\n\t"\
            :::\
            );\
}

MemoryManage * MemoryManage::currentMM = 0;

void MemoryManage::MemoryManageInit()
{
    //ctor
    MemoryManage::currentMM = this;
    gdt = {
        {
            0, 0
        },
        {
            0x000fff00, 0x00c09a00
        },
        {
            0x00000fff, 0x00c09200
        },
        {
            0, 0
        },
        {
            0, 0
        },
    };

    ldt_array = {
        {
            {
                0, 0
            },
            {
                0, 0
            },
            {
                0, 0
            }
        },
        {
            {
                0, 0
            },
            {
                0, 0
            },
            {
                0, 0
            }
        },
    };

    page_dir = {0, };
    memory_map = new char[PAGING_PAGES];

    g_cpu.set_cr3((long)&page_dir);
/*
    __asm__(
        "movl %%eax %%cr2\n\t"
        ::"a" (&page_dir)
        );
//*/
    //start_paging();

/*
    __asm__(
        "movl %%cr0 %%eax\n\t"
        "orl $0x80000000 %%eax\n\t"
        "movl %%eax %%cr0\n\t"
        ::
        );
//*/
    g_cpu.lgdt((unsigned long)&gdt);

    SystemCall::SetPageFaultTraps(&on_page_fault);
}

/*
unsigned long MemoryManage::get_current_ldt(){

}
//*/


inline void MemoryManage::set_base(desc_struct * ldt_address, unsigned long base){
    ldt_address -> a &= 0x0000FFFF;     //0000YYYY
    ldt_address -> a |= ((base & 0xFFFF) << 16);    // 0000YYYY | XXXXFFFF = XXXXYYYY
    ldt_address -> b &= 0x00FFFF00;    //00YYYY00
    ldt_address -> b |= (base & 0xFF000000 ) | (base & 0xFF0000) >> 16;  //00YYYY00 | 0xXXFFFFZZ = XXYYYYZZ
    return ;
}

inline void MemoryManage::set_limit(desc_struct * ldt_address, unsigned long limit){
    ldt_address -> a &= 0xFFFF0000;
    ldt_address -> a |= (limit & 0xFFFF);
    ldt_address -> b &= 0xFFF0FFFF;
    ldt_address -> b |= (limit & 0x000F0000);

    return ;
}

inline unsigned long MemoryManage::get_base(desc_struct ldt_address){
    unsigned long _base = ( (ldt_address.a & 0xFFFF0000) >> 16 ) & 0xFFFF;
    _base |= ldt_address.b & 0xFF000000;
    _base |= ( ( ldt_address.b & 0xFF ) << 16 ) & 0xFF0000;
    return _base;
}

inline unsigned long MemoryManage::get_limit(desc_struct ldt_address){
    unsigned long _limit = ldt_address.a & 0xFFFF;
    _limit += ldt_address.b & 0xF0000;
    return _limit;
}

unsigned long MemoryManage::ldt_copy_mem(int index, int current_index){

    unsigned long old_data_base,new_data_base,data_limit;
	unsigned long old_code_base,new_code_base,code_limit;

	ldt_table * p_ldt = &(currentMM -> ldt_array[index]);// * LDT_TABLE_SIZE];
	ldt_table * c_ldt = &(currentMM -> ldt_array[current_index]);// * LDT_TABLE_SIZE];

	code_limit=get_limit( (*c_ldt)[1] );
	data_limit=get_limit( (*c_ldt)[2] );//???what's this
	old_code_base = get_base( (*c_ldt)[1] );
	old_data_base = get_base( (*c_ldt)[2] );
	/*
	if (old_data_base != old_code_base)
		panic("We don't support separate I&D");
	if (data_limit < code_limit)
		panic("Bad data_limit");
    //*/
	new_data_base = new_code_base = index * SEG_LENGTH;
	//p -> setStart_code(new_code_base);

	set_base( &( (*p_ldt)[1] ), new_code_base);
	set_base( &( (*p_ldt)[2] ), new_data_base);

	set_limit( &( (*p_ldt)[1] ), code_limit);
	set_limit( &( (*p_ldt)[2] ), data_limit);

	if (!copy_page_tables(old_data_base,new_data_base,data_limit)) {
		//printk("free_page_tables: from copy_mem\n");
		free_page_tables(new_data_base,data_limit);
		return 0;
	}
	return new_code_base;

}

//well the copy and free op is base on every 4MB.....well, linus is so lasy...an so am I
bool MemoryManage::copy_page_tables(unsigned long source_base, unsigned long dect_base, unsigned long limit){
    unsigned long * source_table, * dect_table;
	unsigned long source_dir, dect_dir,tmp;
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
		if (( currentMM -> page_dir[dect_dir + j] ) & 1){
		    //panic("dect page is exsit...");
		    return false;
		}

		if (!(1 & (currentMM -> page_dir[source_dir + j]) ))
			continue;
		source_table = (unsigned long *) (0xfffff000 & (currentMM -> page_dir[source_dir + j]) );

        //the page_table should not exist...we just create it...
        if (!(tmp=get_free_page()))
            return false;
        currentMM -> page_dir[dect_dir + j] = tmp | 7;
        dect_table = (unsigned long *) tmp;

		for (int i=0 ; i < ( ( source_base == 0 ) ? 0xA0 : PAGE_TABLE_SIZE ) ; i++) {
            unsigned long current_page = source_table[i];
			if (!(1 & current_page))
				continue;

			current_page &= ~2;    //make it copy on write....
			dect_table[i] = current_page;
			if (current_page > LOW_MEM) {
				source_table[i] = current_page;
				currentMM -> memory_map[MAP_NR(current_page)]++;
			}
		}
	}
	return true;
}

void MemoryManage::on_process_die(int index){
    ldt_table * p_ldt = &(currentMM -> ldt_array[index]);

    free_page_tables(get_base((*p_ldt)[1]),get_limit( (*p_ldt)[1] ));
	free_page_tables(get_base((*p_ldt)[2]),get_limit( (*p_ldt)[2] ));
}





bool MemoryManage::free_page_tables(unsigned long from,unsigned long size)
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
		if (!(1 & (currentMM -> page_dir[dir + j]) ))
			continue;
		pg_table = (unsigned long *) (0xfffff000 & (currentMM -> page_dir[dir + j]) );
		for (int i=0 ; i < PAGE_TABLE_SIZE ; i++) {
			if (1 & pg_table[i])
				free_page(0xfffff000 & pg_table[i]);
			pg_table[i] = 0;
		}
		free_page(0xfffff000 & (currentMM -> page_dir[dir + j]) );
		currentMM -> page_dir[dir + j] = 0;
	}
	//invalidate();
	return true;
}


bool MemoryManage::free_page(unsigned long addr)
{
	if (addr < LOW_MEM) return true;
	//if (addr >= HIGH_MEMORY)
		//panic("trying to free nonexistent page");
	if((currentMM -> memory_map[MAP_NR(addr)])--){
        return true;
	}
	currentMM -> memory_map[MAP_NR(addr)] = 0;
	//panic  free free page
	return false;
}

unsigned long MemoryManage::get_free_page(){
    for(int i = 0; i < PAGING_PAGES; ++i){
        if(currentMM -> memory_map[i] == 0){
            currentMM -> memory_map[i] ++;
             printf("free page is %d\n", (i << 12) + LOW_MEM);
             return (i << 12) + LOW_MEM;
        }
    }
    //panic("no more free memory !");
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

void MemoryManage::put_page(unsigned long liner_address)
{
    int physical_address = get_free_page();

    write_page_table(liner_address, physical_address);
}


bool MemoryManage::write_page_table(unsigned long address, unsigned long page){

	unsigned long tmp, *page_table, table_item_offset;

/* NOTE !!! This uses the fact that _pg_dir=0 */

	//if (page < LOW_MEM || page >= HIGH_MEMORY)
		//printk("Trying to put page %p at %p\n",page,address);
	//if (memory_map[(page-LOW_MEM) >> 12] != 1)
		//printk("memory_map disagrees with %p at %p\n",page,address);

    //get the high 10 bit, its the offset of the liner address's page_table in the page_dir
    printf("put page , page dir base is %d, index is %d\n", currentMM -> page_dir, address >> 22);
	table_item_offset = address >> 22;
	if (( currentMM -> page_dir[table_item_offset] ) & 1)
		page_table = (unsigned long *) (0xfffffff8 & (currentMM -> page_dir[table_item_offset]) );
	else {
	    //when the page_table is not exist...wo should create it...
		if (!(tmp=get_free_page()))
			return false;
		currentMM -> page_dir[table_item_offset] = tmp|7;
		printf("we create a page table ,and put it in dir its address is %d, dir value is %d, addr is %d\n",
         tmp, currentMM -> page_dir[table_item_offset], &(currentMM -> page_dir[table_item_offset]) );
		page_table = (unsigned long *) tmp;
	}
	// the 10 bit in the middle is the offset of the table_item
	//associated with the address in the page_table
	// >> 12 then left the high 20 bit, &0x3ff then left the middle 10 bits...the high 10 is set 0
    printf("put page , page table base is %d, index is %d\n", page_table, (address >> 12) &0x3ff);
	page_table[(address>>12) & 0x3ff] = page | 7;
	printf("we a page with linner addr %d in the physical addr %d\n",
        address, page);
	//well the page_table is somthing to save which physical page is associated with one liner address...
	//so the value in the item of the page_table witch is correspond to the linear address
	//obviously is the physical page's id, the physical address this time..
	/* no need for invalidate */
	return true;
}



//inline void MemoryManage::set_tssldt_desc(desc_struct * desc_address, unsigned long aim_addr, unsigned type)



void MemoryManage::copy_on_write(unsigned long error_code,unsigned long address)
{
/* we cannot do this yet: the estdio library writes to code space */
/* stupid, stupid. I really want the libc.a from GNU */

    unsigned long * page_table = (unsigned long *)(0xfffff000 & (currentMM -> page_dir[address >> 22]) ) ;
	un_wp_page( (unsigned long *) page_table[ (address>>12) & 0xffc] );

}

#define copy_page(from,to) \
__asm__("cld ; rep ; movsl"::"S" (from),"D" (to),"c" (1024))
/**
* copy on write...
**/
void MemoryManage::un_wp_page(unsigned long * table_entry)
{
	unsigned long old_page,new_page;

	old_page = 0xfffff000 & *table_entry;
	if (old_page >= LOW_MEM && currentMM -> memory_map[MAP_NR(old_page)] == 1) {
		*table_entry |= 2;
		//invalidate();
		return;
	}
	if (!(new_page=get_free_page())){
		//oom();
		//panic not free page....
    }
	if (old_page >= LOW_MEM)
		currentMM -> memory_map[MAP_NR(old_page)]--;
	*table_entry = new_page | 7;
	//invalidate();
	copy_page(old_page,new_page);
}



void do_no_page(unsigned long error_code,unsigned long address)
{
	//int nr[4];
	unsigned long tmp;
	//unsigned long page;
	//int block,i;
//*
	address &= 0xfffff000;
	tmp = address - ProcessManage::getCurrent() -> getStart_code();
	if (tmp >= ProcessManage::getCurrent() -> getEnd_data()) {
		MemoryManage::put_page(address);
		return;
	}
//*/
/*
	if (share_page(tmp)){
		return;
	}

//this is reading some data from the filesystem...and put it in the code seg of a process....
//to make the problem simple, now we just don't consider it....
	if (!(page = get_free_page()))
		oom();
 //remember that 1 block is used for header
	block = 1 + tmp/BLOCK_SIZE;
	for (i=0 ; i<4 ; block++,i++)
		nr[i] = bmap(current->executable,block);
	bread_page(page,current->executable->i_dev,nr);
	i = tmp + 4096 - current->end_data;
	tmp = page + 4096;
	while (i-- > 0) {
		tmp--;
		*(char *)tmp = 0;
	}
	if (put_page(page,address))
		return;
	free_page(page);
	oom();
	//*/
}

void MemoryManage::on_page_fault(){

}



