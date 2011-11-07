#include "../include/config.h"

#ifndef MEMORYMANAGE_H
#define MEMORYMANAGE_H

#define PAGE_DIR_SIZE    1024
#define PAGE_TABLE_SIZE  1024

#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY+1)

#define GDT_TABLE_MAX_SIZE   256
#define LDT_TABLE_SIZE   3

#define SEG_LENGTH  0x4000000

#define PAGE_SIZE   4096

#define LOW_MEM 0x100000
#define PAGING_MEMORY (15*1024*1024)
#define PAGING_PAGES (PAGING_MEMORY>>12)
#define MAP_NR(addr) (((addr)-LOW_MEM)>>12)

#define HIGH_MEMORY PAGING_MEMORY + LOW_MEM


#define TSS_SEG_DESC    0x89
#define LDT_SEG_DESC    0x82



typedef struct desc_struct {
	unsigned long a,b;
}desc_table[GDT_TABLE_MAX_SIZE];

typedef struct desc_struct ldt_table[LDT_TABLE_SIZE];

class MemoryManage
{
    private:
        static desc_table gdt;   //the gdt table
        static ldt_table ldt_array[MAX_TASK_NUM];//[LDT_TABLE_SIZE];
        static char memory_map[PAGING_PAGES];
        static unsigned long page_dir[PAGE_DIR_SIZE];

    protected:
        static void on_page_fault();
        static unsigned long page_dir_address(){
            return (unsigned long)page_dir;
        }
        static unsigned long get_LDT_choice(int index){
            return ((((unsigned long) index) << 4) + (FIRST_LDT_ENTRY << 3));
        }
        static unsigned long get_TSS_choice(int index){
            return ((((unsigned long) index) << 4) + (FIRST_TSS_ENTRY << 3));
        }
        static long set_TSS_desc(int index, unsigned long address){
            _set_tssldt_desc(
                             & gdt [ (index << 1) + FIRST_TSS_ENTRY ],
                             address, "0x89");
            return 0;
        }
        static unsigned long set_LDT_desc(int index, int current_index){
            unsigned long start_code = ldt_copy_mem(index, current_index);
            _set_tssldt_desc(
                             & gdt [ (index << 1) + FIRST_LDT_ENTRY ],
                             (unsigned long)(&ldt_array[current_index]), "0x82");
            return start_code;
        }
        static unsigned long get_empty_page();   // find a free physical page and alloc a linner address for it then return the linner address
        static void put_page(unsigned long liner_address);   // find a free physical page and associate it with the linner address
        static void on_process_die(int index);

    private:
        MemoryManage();
        static long find_wapped_out();

        void copy_on_write(unsigned long error_code,unsigned long address);

        static unsigned long ldt_copy_mem(int index, int current_index);
        static bool free_page_tables(unsigned long from, unsigned long size);
        static bool copy_page_tables(unsigned long source_base, unsigned long dect_base, unsigned long limit);
        static bool write_page_table(unsigned long address, unsigned long page);
        static bool free_page(unsigned long addr);
        static unsigned long get_free_page();

        static inline void MemoryManage::set_tssldt_desc(desc_struct * desc_address, unsigned long aim_addr, unsigned type)
        static inline void set_base(desc_struct * ldt_address, unsigned long base);
        static inline void set_limit(desc_struct * ldt_address, unsigned long limit);
        static inline unsigned long get_base(desc_struct ldt_address);
        static inline unsigned long get_limit(desc_struct ldt_address);
};

#endif // MEMORYMANAGE_H





