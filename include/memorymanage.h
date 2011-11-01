#ifndef MEMORYMANAGE_H
#define MEMORYMANAGE_H

#define PAGE_DIR_NUM    1024

#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY+1)

#define GDT_TABLE_MAX_NUM   256
#define LDT_TABLE_NUM   3

#define SEG_LENGTH  0x4000000

#define LOW_MEM 0x100000
#define PAGING_MEMORY (15*1024*1024)
#define PAGING_PAGES (PAGING_MEMORY>>12)
#define MAP_NR(addr) (((addr)-LOW_MEM)>>12)

#define HIGH_MEMORY PAGING_MEMORY + LOW_MEM

typedef struct desc_struct {
	unsigned long a,b;
}desc_table[GDT_TABLE_MAX_NUM];

typedef struct desc_struct[LDT_TABLE_NUM] ldt_table;

class MemoryManage
{
    private:
        static desc_table gdt;   //the gdt table
        static ldt_table ldt_array[MAX_TASK_NUM];
        static char memory_map[PAGING_PAGES];
        static long page_dir[PAGE_DIR_NUM];
    protected:
        static void on_page_fault();
        static unsigned long page_dir_address(){
            return &page_dir;
        }
        static unsigned long get_LDT_choice(int index){
            return ((((unsigned long) index) << 4) + (FIRST_LDT_ENTRY << 3));
        }
        static unsigned long get_TSS_choice(int index){
            return ((((unsigned long) index) << 4) + (FIRST_TSS_ENTRY << 3));
        }
        static long set_TSS_desc(int index, unsigned long address){
            _set_tssldt_desc(
                             ( (char *) ( gdt + (((unsigned long) index) << 1) + FIRST_TSS_ENTRY ) ),
                             ( (int) address ), "0x89");
            return 0;
        }
        static unsigned long set_LDT_desc(int index, int current_index){
            unsigned long start_code = ldt_copy_mem(index, current_index);
            _set_tssldt_desc(
                             ( (char *) ( gdt + (((unsigned long) index) << 1) + FIRST_LDT_ENTRY ) ),
                             ( (int) (ldt_array + current_index) ), "0x82");
            return start_code;
        }
        static unsigned long get_empty_page();
        static void on_process_die(int index);
    private:
        MemoryManage();
        static long find_wapped_out();
        static unsigned long ldt_copy_mem(int index, int current_index);
        static void free_page_tables(unsigned long from, unsigned long size);
        static void copy_page_tables(unsigned long source_base, unsigned long dect_base, unsigned long limit);
        static void write_page_table(unsigned long address, unsigned long page);
        static void free_page(unsigned long addr);
        static unsigned long get_free_page();
};

#endif // MEMORYMANAGE_H




#define _set_tssldt_desc(n,addr,type) \
__asm__ ("movw $104,%1\n\t" \
	"movw %%ax,%2\n\t" \
	"rorl $16,%%eax\n\t" \
	"movb %%al,%3\n\t" \
	"movb $" type ",%4\n\t" \
	"movb $0x00,%5\n\t" \
	"movb %%ah,%6\n\t" \
	"rorl $16,%%eax" \
	::"a" (addr), "m" (*(n)), "m" (*(n+2)), "m" (*(n+4)), \
	 "m" (*(n+5)), "m" (*(n+6)), "m" (*(n+7)) \
	)

