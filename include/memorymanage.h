#ifndef MEMORYMANAGE_H
#define MEMORYMANAGE_H

#define MEMORY_SIZE 4096

#define PAGE_DIR_NUM    1024

#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY+1)

class MemoryManage
{
    private:
        static char memory_map[MEMORY_SIZE];
        static long page_dir[PAGE_DIR_NUM];
    protected:
        static void on_page_fault();
        static long page_dir_address(){
            return &page_dir;
        }
        static long get_LDT_desc(int index){
            return ((((unsigned long) index)<<4)+(FIRST_LDT_ENTRY<<3));
        }
        static long get_TSS_desc(int index){
            return ((((unsigned long) index)<<4)+(FIRST_TSS_ENTRY<<3));
        }
        static long set_LDT_desc(int index, unsigned long address){
            set_ldt_desc((((unsigned long) index)<<4)+(FIRST_LDT_ENTRY<<3), address);
        }
        static long set_TSS_desc(int index,  unsigned long address){
            set_tss_desc((((unsigned long) index)<<4)+(FIRST_TSS_ENTRY<<3), address);
        }
        static long pute_page(long addr);
    private:
        MemoryManage();
        static long find_wapped_out();
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

#define set_tss_desc(n,addr) _set_tssldt_desc(((char *) (n)),((int)(addr)),"0x89")
#define set_ldt_desc(n,addr) _set_tssldt_desc(((char *) (n)),((int)(addr)),"0x82")
