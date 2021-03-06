#include "../include/config.h"
#include "../include/global.h"
#include "../include/processmanage.h"
#include "../include/systemcall.h"

#ifndef MEMORYMANAGE_H
#define MEMORYMANAGE_H

#define PAGE_DIR_SIZE    1024
#define PAGE_TABLE_SIZE  1024

#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY+1)

#define GDT_TABLE_MAX_SIZE   256
#define LDT_TABLE_SIZE   3

#define SEG_LENGTH  0x4000000

#define LOW_MEM KERNEL_MEM_END
#define PAGE_SIZE   (g_memory->get_page_size())
#define PAGE_SIZE_BIT   (g_memory->get_page_size_bit())
#define PAGING_MEMORY   (g_memory->get_page_size() * g_memory->get_page_num() - LOW_MEM)
#define PAGING_PAGES    (PAGING_MEMORY / PAGE_SIZE)
#define MAP_NR(addr)    (((addr)-LOW_MEM)>>PAGE_SIZE_BIT)

#define HIGH_MEMORY (PAGING_MEMORY + LOW_MEM)


#define TSS_SEG_DESC    0x89
#define LDT_SEG_DESC    0x82



typedef desc_struct gdt_desc_table[GDT_TABLE_MAX_SIZE];
typedef desc_struct ldt_table[LDT_TABLE_SIZE];

class MemoryManage
{
    friend class KernelRescue;
private:
    unsigned long page_dir[PAGE_DIR_SIZE];
    gdt_desc_table gdt;   //the gdt table
    ldt_table ldt_array[MAX_TASK_NUM];//[LDT_TABLE_SIZE];
    char* memory_map;
    static MemoryManage * currentMM;
public:
    MemoryManage() {};
    static unsigned long page_dir_address()
    {
        return (unsigned long)(currentMM -> page_dir);
    }
    static int get_LDT_choice(int index)
    {
        return ((((unsigned long) index) << 4) + (FIRST_LDT_ENTRY << 3));
    }
    static int get_TSS_choice(int index)
    {
        return ((((unsigned long) index) << 4) + (FIRST_TSS_ENTRY << 3));
    }
    static void set_TSS_desc(int index, unsigned long address)
    {
        set_tssldt_desc(
            & (currentMM -> gdt[ (index << 1) + FIRST_TSS_ENTRY ] ),
            address, 0x89);
        return ;
    }
    static void Init_LDT_desc(int index)
    {
        ldt_table * c_ldt = &(currentMM -> ldt_array[index] );
        (*c_ldt)[1].a = 0x9f;
        (*c_ldt)[1].b = 0xc0fa00;
        (*c_ldt)[2].a = 0x9f;
        (*c_ldt)[2].b = 0xc0f200;
        //unsigned long start_code = ldt_copy_mem(index, current_index);

        set_tssldt_desc(
            & (currentMM -> gdt[ (index << 1) + FIRST_LDT_ENTRY ] ),
            (unsigned long)(& (currentMM -> ldt_array[index]) ), 0x82);
        return ;
    }
    static unsigned long set_LDT_desc(int index, int current_index)
    {
        //printf("before ldt_copy_mem\n");
        unsigned long start_code = ldt_copy_mem(index, current_index);
        //printf("ldt_copy_mem finished\n");
        set_tssldt_desc(
            & (currentMM -> gdt[ (index << 1) + FIRST_LDT_ENTRY ] ),
            (unsigned long)(& (currentMM -> ldt_array[index]) ), 0x82);
        return start_code;
    }
    static unsigned long get_empty_page();   // find a free physical page and alloc a linner address for it then return the linner address
    static void put_page(unsigned long liner_address);   // find a free physical page and associate it with the linner address
    static void on_process_die(int index);

private:
    void MemoryManageInit();
    static long find_wapped_out();

    static void on_page_fault();
    static void copy_on_write(unsigned long error_code,unsigned long address);
    static void un_wp_page(unsigned long * table_entry);

    static unsigned long ldt_copy_mem(int index, int current_index);
    static bool free_page_tables(unsigned long from, unsigned long size);
    static bool copy_page_tables(unsigned long source_base, unsigned long dect_base, unsigned long limit);
    static bool write_page_table(unsigned long address, unsigned long page);
    static bool free_page(unsigned long addr);
    static unsigned long get_free_page();

    static inline void set_tssldt_desc(desc_struct * desc_address, unsigned long aim_addr, unsigned type)
    {
        desc_address -> a &= 0x00;
        desc_address -> b &= 0x00;
        desc_address -> a |= 0x00000068;
        desc_address -> a |= ( (aim_addr & 0xFFFF) << 16 ) & 0xFFFF0000;
        desc_address -> b |= 0x0000FF00 & (type << 8);
        desc_address -> b |= 0xFF0000FF & ( (aim_addr & 0xFF000000) | ( (aim_addr & 0xFF0000) >> 16 ) );
        desc_address -> b &= 0xFF00FFFF;

        //printf("set aim %lx desc a %lx, b %lx, in addr %ld\n",aim_addr, desc_address -> a, desc_address ->b, desc_address);
        return;
    }
    static inline void set_base(desc_struct * ldt_address, unsigned long base);
    static inline void set_limit(desc_struct * ldt_address, unsigned long limit);
    static inline unsigned long get_base(desc_struct ldt_address);
    static inline unsigned long get_limit(desc_struct ldt_address);

    static inline void set_GDT(unsigned long address);
};

#endif // MEMORYMANAGE_H





