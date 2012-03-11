#ifndef MUTIXCPU_H
#define MUTIXCPU_H

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <map>


#define BIT_OF_REGISTER 32

struct cpu_struct {
    unsigned long   tr;
    unsigned long    cr2;
    unsigned long    idt;
    unsigned long    gdt;
	unsigned long	cr3;
	unsigned long	eip;
	unsigned long	eflags;
	unsigned long	eax,ecx,edx,ebx;
	unsigned long	esp;
	unsigned long	ebp;
	unsigned long	esi;
	unsigned long	edi;
	unsigned long	es;
	unsigned long	cs;
	unsigned long	ss;
	unsigned long	ds;
	unsigned long	fs;
	unsigned long	gs;
	unsigned long	ldt;
};
typedef void (*op_function) (void);
/*
struct i387_struct {
	long	cwd;
	long	swd;
	long	twd;
	long	fip;
	long	fcs;
	long	foo;
	long	fos;
	long	st_space[20];//	/* 8*10 bytes for each FP-reg = 80 bytes */  /*
};
//*/

typedef struct _tss_struct {
	long	back_link;	/* 16 high bits zero */
	long	esp0;
	long	ss0;		/* 16 high bits zero */
	long	esp1;
	long	ss1;		/* 16 high bits zero */
	long	esp2;
	long	ss2;		/* 16 high bits zero */
	long	cr3;
	long	eip;
	long	eflags;
	long	eax,ecx,edx,ebx;
	long	esp;
	long	ebp;
	long	esi;
	long	edi;
	long	es;		/* 16 high bits zero */
	long	cs;		/* 16 high bits zero */
	long	ss;		/* 16 high bits zero */
	long	ds;		/* 16 high bits zero */
	long	fs;		/* 16 high bits zero */
	long	gs;		/* 16 high bits zero */
	long	ldt;		/* 16 high bits zero */
	long	trace_bitmap;	/* bits: trace 0, bitmap 16-31 */
	//struct i387_struct i387;
} tss_struct;

enum DescType {
    DESC_TYPE_LDT, DESC_TYPE_TSS, DESC_TYPE_CALL_GATE,
    DESC_TYPE_INTER_GATE, DESC_TYPE_TRAP_GATE, DESC_TYPE_TASK_GATE,
    DESC_TYPE_CODE_SEG, DESC_TYPE_DATA_SEG, DESC_TYPE_NOT_IN_MEM, DESC_TYPE_ILLEGAL
};
enum CPUState {
    RESCUE_LEVEL = 0, KERNEL_LEVEL = 1, KERNEL_IO_LEVEL = 2, USER_LEVEL = 3
};

typedef struct _choice{long a, b;} Desc;

typedef struct _jmp_point{ jmp_buf point;} JumpPoint;

class MutixCPU
{
    friend class ProcessManage;
    friend class MemoryManage;
    friend class SystemCall;
    public:
        MutixCPU();
        virtual ~MutixCPU();
    protected:
    private:

        void push(long l){
            long virtual_stack_addr = _cpu.fs << 16 + _cpu.esp;//XXX FIXME!!!!!!!!
            long actrual_stack_addr = line2physical_addr(virtual_stack_addr);
            long * addr_ptr = (long*) actrual_stack_addr;
            *addr_ptr =  l;
            _cpu.esp ++;
        };
        long pop(){
            long virtual_stack_addr = _cpu.fs << 16 + _cpu.esp;//XXX FIXME!!!!!!!!
            long actrual_stack_addr = line2physical_addr(virtual_stack_addr);
            long * addr_ptr = (long*) actrual_stack_addr;
            _cpu.esp --;
            return *addr_ptr;
        };

        long line2physical_addr(unsigned long line_addr){
            printf("changing line %d to physical\n", line_addr);
            unsigned long page_dir_index = (line_addr & 0xFFC00000) >> 22;
            unsigned long page_table_index = (line_addr & 0x003FF000) >> 12;
            unsigned long inner_page_offset = (line_addr & 0x00000FFF);
            unsigned long * page_dir_base = (unsigned long*)get_cr3();
            printf("page dir base is %d, index is %d\n", page_dir_base, page_dir_index);
            unsigned long  * page_table_base = (unsigned long *)((page_dir_base[page_dir_index]) & 0xFFFFFFF8);
            printf("page table base is %d, index is %d\n", page_table_base, page_table_index);
            unsigned long page_base = (page_table_base[page_table_index]) & 0xFFFFFFF8;
            printf("changing line %d to physical %d, page table value is %d \n",
                   line_addr, page_base + inner_page_offset, &page_table_base[page_table_index]);
            return page_base + inner_page_offset;
        };

        CPUState get_state(){ return _state; };

        void to_state(CPUState state){ _state = state; }

        long    get_cr3(){return _cpu.cr3;};
        long	get_eip(){return _cpu.eip;};
        long	get_eflags(){return _cpu.eflags;};
        long	get_eax(){return _cpu.eax;};
        long    get_ecx(){return _cpu.ecx;};
        long    get_edx(){return _cpu.edx;};
        long    get_ebx(){return _cpu.ebx;};
        long	get_esp(){return _cpu.esp;};
        long	get_ebp(){return _cpu.ebp;};
        long	get_esi(){return _cpu.esi;};
        long	get_edi(){return _cpu.edi;};
        long	get_es(){return _cpu.es;};
        long	get_cs(){return _cpu.cs;};
        long	get_ss(){return _cpu.ss;};
        long	get_ds(){return _cpu.ds;};
        long	get_fs(){return _cpu.fs;};
        long	get_gs(){return _cpu.gs;};
        long	get_ldt(){return _cpu.ldt;};

        void    set_cr2(long cr2){ _cpu.cr2 = cr2; };
        void    set_cr3(long cr3){ _cpu.cr3 = cr3; };
        void	set_eip(long eip){ _cpu.eip = eip; };
        void	set_eflags(long eflags){ _cpu.eflags = eflags; };
        void	set_eax(long eax){ _cpu.eax = eax; };
        void    set_ecx(long ecx){ _cpu.ecx = ecx; };
        void    set_edx(long edx){ _cpu.edx = edx; };
        void    set_ebx(long ebx){ _cpu.ebx = ebx; };
        void	set_esp(long esp){ _cpu.esp = esp; };
        void	set_ebp(long ebp){ _cpu.ebp = ebp; };
        void	set_esi(long esi){ _cpu.esi = esi; };
        void	set_edi(long edi){ _cpu.edi = edi; };
        void	set_es(long es){ _cpu.es = es; };
        void	set_cs(long cs){ _cpu.cs = cs; };
        void	set_ss(long ss){ _cpu.ss = ss; };
        void	set_ds(long ds){ _cpu.ds = ds; };
        void	set_fs(long fs){ _cpu.fs = fs; };
        void	set_gs(long gs){ _cpu.gs = gs; };
        void	lldt(long ldt){printf("seting gdt %d\n", ldt); _cpu.ldt = ldt; };
        void    lidt(long idt){printf("seting gdt %d\n", idt); _cpu.idt = idt; }
        void    lgdt(long gdt){printf("seting gdt %d\n", gdt); _cpu.gdt = gdt;}
        unsigned long   get_gdt(){return _cpu.gdt;};

        void str(){
            tss_struct* tss = (tss_struct*)get_tr();
            memcpy(&(tss -> cr3), &(_cpu.cr3), sizeof(_cpu) - (int)((long)&(_cpu.cr3) - (long)&_cpu));
        }
        unsigned long get_tr(){
            return _cpu.tr;
        };
        void set_tr(const tss_struct* tss){
            _cpu.tr = (unsigned long)tss;
        };
        bool is_32bit_gate(const Desc* desc){
            return desc->b & (1 << 11);
        };
        bool is_besy_tss(const Desc* desc){
            return desc->b & (1 << 9);
        };
        DescType check_type(const Desc* desc){
            printf("type is num %lx\n", desc->b);
            if(!(desc->b & (1 << 15))){
                return DESC_TYPE_NOT_IN_MEM;
            }
            if(desc->b & (1 << 12)){
                if(desc->b & (1 << 11)){
                    return DESC_TYPE_CODE_SEG;
                }
                else{
                    return DESC_TYPE_DATA_SEG;
                }
            }
            else{
                int type = (0x00000f00 & desc->b) >> 8;

                switch(type){
                    case 0:
                    case 8:
                    case 10:
                    case 13:
                        return DESC_TYPE_ILLEGAL;
                    case 1:
                    case 3:
                    case 9:
                    case 11:
                        return DESC_TYPE_TSS;
                    case 2:
                        return DESC_TYPE_LDT;
                    case 4:
                    case 12:
                        return DESC_TYPE_CALL_GATE;
                    case 5:
                        return DESC_TYPE_TASK_GATE;
                    case 6:
                    case 14:
                        return DESC_TYPE_INTER_GATE;
                    case 7:
                    case 15:
                        return DESC_TYPE_TRAP_GATE;
                }
            }
        };
        unsigned long get_base_from_desc(const Desc* desc){
             unsigned long _base = ( (desc->a & 0xFFFF0000) >> 16 ) & 0xFFFF;
            _base |= desc->b & 0xFF000000;
            _base |= ( ( desc->b & 0xFF ) << 16 ) & 0xFF0000;
            return _base;
        };
        unsigned long get_limit_frome_desc(const Desc* desc){
            unsigned long _limit = desc->a & 0xFFFF;
            _limit += desc->b & 0xF0000;
            return _limit;
        };

        const tss_struct * get_tss(const Desc* tss_desc){
            printf("desc type is %d\n", check_type(tss_desc));
            if(DESC_TYPE_TSS != check_type(tss_desc))
            {
                return NULL;
            }
            return (tss_struct *)line2physical_addr(get_base_from_desc(tss_desc));
        }
        void load_tss(const tss_struct* tss){
            if(!tss) return;//KernelRescue::on_panic();
            memcpy(&(_cpu.cr3), &(tss -> cr3), sizeof(_cpu) - (int)((long)&(_cpu.cr3) - (long)&_cpu));
            printf("tr seted %d\n", tss);
            set_tr(tss);
        };
        void ltr(int tss_choice){
            const Desc* tss_desc = get_desc_from_choice(tss_choice);//(const Desc*)&gdt_base[tss_choice >> 3];
            const tss_struct * tss = get_tss(tss_desc);
            load_tss(tss);
            printf("set cur_task ,tss_choice is %d, cur_task is %d\n", tss_choice, cur_task);
            cur_task = tss_choice>>3;
        };
        void switch_to(int tss_choice){
            int prev_task = cur_task;
            printf("cur_task is %d\n", cur_task);
            printf("swithc to tss_choice = %d\n", tss_choice);
            str();
            printf("ltr\n");
            ltr(tss_choice);

            int var = setjmp(jmp_env[prev_task]);
            if(0 == var) {
                printf("switching to %d\n", cur_task);
                longjmp(jmp_env[cur_task], prev_task);
                printf("hei,what are you  doing ?!\n");
            }
            else{
                printf("switched from %d\n", var);
            }
        };
        void switch_to_first_task(int tss_choice){
            printf("switching to %d\n", cur_task);
            longjmp(jmp_env[cur_task], cur_task);
        };
        unsigned long get_offset_from_gate(const Desc* gate){
            unsigned long offset = gate->b & 0xFFFF0000;
            offset |= (0x0000FFFF & gate->a);
            return offset;
        }
        int get_choice_from_gate(const Desc* gate){
            return (gate->a & 0xffff0000) >> 16;
        };
        const Desc* get_desc_from_choice(int choice){
            bool is_in_ldt = (choice & (1 << 3));
            const Desc* desc = NULL;
            if(!is_in_ldt){
                const Desc* gdt_base = (Desc*)get_gdt();
                printf("gdt addr is %d, choice is %d, index is %d, index addr is %d\n",
                   get_gdt(), choice, choice >> 3, &gdt_base[choice >> 3]);
                return (const Desc*)&gdt_base[choice >> 3];
            }
            else{
                const Desc* ldt_base = (Desc*)get_ldt();
                printf("ldt addr is %d, choice is %d, index is %d, index addr is %d\n",
                   get_ldt(), choice, choice >> 3, &ldt_base[choice >> 3]);
                return (const Desc*)&ldt_base[choice >> 3];
            }
        };
        void deal_with_task(const Desc* gate){
            if(DESC_TYPE_TASK_GATE != check_type(gate)){
                return;//KernelRescue::on_panic();
            }
            int choice = get_choice_from_gate(gate);
            switch_to(choice);
        }
        void deal_with_inter(const Desc* gate){
        }
        void deal_with_trap(const Desc* gate){
            if(DESC_TYPE_TRAP_GATE != check_type(gate)){
                return;//KernelRescue::on_panic();
            }
            unsigned long offset = get_offset_from_gate(gate);
            int choice = get_choice_from_gate(gate);
            const Desc* seg_desc = get_desc_from_choice(choice);
            if(DESC_TYPE_CODE_SEG != check_type(gate)){
                return;//KernelRescue::on_panic();
            }
            unsigned long seg_base = get_base_from_desc(seg_desc);
            unsigned long seg_limit = get_limit_frome_desc(seg_desc);
            if(offset > seg_limit){
                return;
            }
            op_function addr = (op_function)seg_base + offset;
            addr();
        }
        void deal_with_call(const Desc* gate){

        }
        void deal_with_gate(const Desc* gate){
            const Desc * desc = NULL;
            switch(check_type(gate))
            {
                case DESC_TYPE_TASK_GATE:
                    deal_with_task(gate);
                    break;
                case DESC_TYPE_INTER_GATE:
                    deal_with_inter(gate);
                    break;
                case DESC_TYPE_CALL_GATE:
                    deal_with_call(gate);
                    break;
                case DESC_TYPE_TRAP_GATE:
                    deal_with_trap(gate);
                    break;
                default:
                    //KernelRescue::on_panic();
                    return ;
            }
            return ;
        };
    public:
        jmp_buf jmp_env[300];
    private:
        struct cpu_struct _cpu;
        CPUState _state;
        int cur_task;
};

#endif // MUTIXCPU_H
