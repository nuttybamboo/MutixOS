#include "../include/mutixcpu.h"

int MutixCPU::cur_task = 0;

MutixCPU::MutixCPU()
{
    //ctor
}

MutixCPU::~MutixCPU()
{
    //dtor
}

void MutixCPU::set_ss_seg_base(unsigned seg_base)
{
    ss_seg_base = seg_base;
}


void MutixCPU::push(long l)
{
    long virtual_stack_addr = _cpu.fs << 16 + _cpu.esp;//XXX FIXME!!!!!!!!
    long actrual_stack_addr = line2physical_addr(virtual_stack_addr);
    long * addr_ptr = (long*) actrual_stack_addr;
    *addr_ptr =  l;
    _cpu.esp ++;
};

long MutixCPU::pop()
{
    long virtual_stack_addr = _cpu.fs << 16 + _cpu.esp;//XXX FIXME!!!!!!!!
    long actrual_stack_addr = line2physical_addr(virtual_stack_addr);
    long * addr_ptr = (long*) actrual_stack_addr;
    _cpu.esp --;
    return *addr_ptr;
};

long MutixCPU::line2physical_addr(unsigned long line_addr)
{
    //printf("changing line %u to physical\n", line_addr);
    unsigned long page_dir_index = (line_addr & 0xFFC00000) >> 22;
    unsigned long page_table_index = (line_addr & 0x003FF000) >> 12;
    unsigned long inner_page_offset = (line_addr & 0x00000FFF);
    unsigned long * page_dir_base = (unsigned long*)get_cr3();
    //printf("page dir base is %u, index is %u\n", page_dir_base, page_dir_index);
    unsigned long  * page_table_base = (unsigned long *) MutixMemory::get_address((page_dir_base[page_dir_index]) & 0xFFFFFFF8);
    //printf("page table base is %d, index is %u\n", page_table_base, page_table_index);
    unsigned long page_base =  (page_table_base[page_table_index]) & 0xFFFFFFF8;
    //printf("changing line %u to physical %u, page table value is %u \n",
    //      line_addr, page_base + inner_page_offset, &page_table_base[page_table_index]);
    return page_base + inner_page_offset;
};

CPUState MutixCPU::get_state()
{
    return _state;
};

void MutixCPU::to_state(CPUState state)
{
    _state = state;
}

long MutixCPU::get_cr3()
{
    return _cpu.cr3;
};


long MutixCPU::get_eip()
{
    return _cpu.eip;
};

long MutixCPU::get_eflags()
{
    return _cpu.eflags;
};

long MutixCPU::get_eax()
{
    return _cpu.eax;
};

long MutixCPU::get_ecx()
{
    return _cpu.ecx;
};

long MutixCPU::get_edx()
{
    return _cpu.edx;
};

long MutixCPU::get_ebx()
{
    return _cpu.ebx;
};

long MutixCPU::get_esp()
{
    return _cpu.esp;
};

long MutixCPU::get_ebp()
{
    return _cpu.ebp;
};

long MutixCPU::get_esi()
{
    return _cpu.esi;
};

long MutixCPU::get_edi()
{
    return _cpu.edi;
};

long MutixCPU::get_es()
{
    return _cpu.es;
};

long MutixCPU::get_cs()
{
    return _cpu.cs;
};

long MutixCPU::get_ss()
{
    return _cpu.ss;
};

long MutixCPU::get_ds()
{
    return _cpu.ds;
};

long MutixCPU::get_fs()
{
    return _cpu.fs;
};

long MutixCPU::get_gs()
{
    return _cpu.gs;
};

long MutixCPU::get_ldt()
{
    return _cpu.ldt;
};

void MutixCPU::set_cr2(long cr2)
{
    _cpu.cr2 = cr2;
};

void MutixCPU::set_cr3(long cr3)
{
    _cpu.cr3 = cr3;
};

void MutixCPU::set_eip(long eip)
{
    _cpu.eip = eip;
};

void MutixCPU::set_eflags(long eflags)
{
    _cpu.eflags = eflags;
};

void MutixCPU::set_eax(long eax)
{
    _cpu.eax = eax;
};

void MutixCPU::set_ecx(long ecx)
{
    _cpu.ecx = ecx;
};

void MutixCPU::set_edx(long edx)
{
    _cpu.edx = edx;
};

void MutixCPU::set_ebx(long ebx)
{
    _cpu.ebx = ebx;
};

void MutixCPU::set_esp(long esp)
{
    _cpu.esp = esp;
};

void MutixCPU::set_ebp(long ebp)
{
    _cpu.ebp = ebp;
};

void MutixCPU::set_esi(long esi)
{
    _cpu.esi = esi;
};

void MutixCPU::set_edi(long edi)
{
    _cpu.edi = edi;
};

void MutixCPU::set_es(long es)
{
    _cpu.es = es;
};

void MutixCPU::set_cs(long cs)
{
    _cpu.cs = cs;
};

void MutixCPU::set_ss(long ss)
{
    _cpu.ss = ss;
};

void MutixCPU::set_ds(long ds)
{
    _cpu.ds = ds;
};

void MutixCPU::set_fs(long fs)
{
    _cpu.fs = fs;
};

void MutixCPU::set_gs(long gs)
{
    _cpu.gs = gs;
};

void MutixCPU::lldt(long ldt)
{
    printf("seting ldt %u\n", ldt);
    _cpu.ldt = ldt;
};

void MutixCPU::lidt(long idt)
{
    printf("seting idt %u\n", idt);
    _cpu.idt = idt;
}

void MutixCPU::lgdt(long gdt)
{
    printf("seting gdt %u\n", gdt);
    _cpu.gdt = gdt;
}

unsigned long MutixCPU::get_gdt()
{
    return _cpu.gdt;
};

void MutixCPU::str()
{
    tss_struct* tss = (tss_struct*)get_tr();
    memcpy(&(tss -> cr3), &(_cpu.cr3), sizeof(_cpu) - (int)((long)&(_cpu.cr3) - (long)&_cpu));
}

unsigned long MutixCPU::get_tr()
{
    return _cpu.tr;
};

void MutixCPU::set_tr(const tss_struct* tss)
{
    _cpu.tr = (unsigned long)tss;
};

bool MutixCPU::is_32bit_gate(const Desc* desc)
{
    return desc->b & (1 << 11);
};

bool MutixCPU::is_besy_tss(const Desc* desc)
{
    return desc->b & (1 << 9);
};

DescType MutixCPU::check_type(const Desc* desc)
{
    //printf("type is num %lx\n", desc->b);
    if(!(desc->b & (1 << 15)))
    {
        return DESC_TYPE_NOT_IN_MEM;
    }
    if(desc->b & (1 << 12))
    {
        if(desc->b & (1 << 11))
        {
            return DESC_TYPE_CODE_SEG;
        }
        else
        {
            return DESC_TYPE_DATA_SEG;
        }
    }
    else
    {
        int type = (0x00000f00 & desc->b) >> 8;

        switch(type)
        {
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

unsigned long MutixCPU::get_base_from_desc(const Desc* desc)
{
    unsigned long _base = ( (desc->a & 0xFFFF0000) >> 16 ) & 0xFFFF;
    _base |= desc->b & 0xFF000000;
    _base |= ( ( desc->b & 0xFF ) << 16 ) & 0xFF0000;
    return _base;
};

unsigned long MutixCPU::get_limit_frome_desc(const Desc* desc)
{
    unsigned long _limit = desc->a & 0xFFFF;
    _limit += desc->b & 0xF0000;
    return _limit;
};

const tss_struct * MutixCPU::get_tss(const Desc* tss_desc)
{
    //printf("desc type is %u\n", check_type(tss_desc));
    if(DESC_TYPE_TSS != check_type(tss_desc))
    {
        return NULL;
    }
    return (tss_struct *) MutixMemory::get_address(line2physical_addr(get_base_from_desc(tss_desc)));
}

void MutixCPU::load_tss(const tss_struct* tss)
{
    if(!tss) return;//KernelRescue::on_panic();
    memcpy(&(_cpu.cr3), &(tss -> cr3), sizeof(_cpu) - (int)((long)&(_cpu.cr3) - (long)&_cpu));
    //printf("tr seted %u\n", tss);
    set_tr(tss);
};

void MutixCPU::ltr(int tss_choice)
{
    const Desc* tss_desc = get_desc_from_choice(tss_choice);//(const Desc*)&gdt_base[tss_choice >> 3];
    const tss_struct * tss = get_tss(tss_desc);
    load_tss(tss);
    //printf("set cur_task ,tss_choice is %u, cur_task is %u\n", tss_choice, cur_task);
    cur_task = tss_choice>>3;
};


int MutixCPU::restore_stack(int index)
{
    if(stack_mirror_arr[index].pointer != NULL)
    {
        //printf("task %d esp is %u, base is %u, size is %d\n", index, stack_mirror_arr[index].esp, ss_seg_base, stack_mirror_arr[index].size);
        memcpy((void*)stack_mirror_arr[index].esp, (void*)stack_mirror_arr[index].pointer, stack_mirror_arr[index].size);
        return 0;
    }
    else
    {
        return -1;
    }
};

int MutixCPU::switch_to(int tss_choice, bool is_new_task)
{
    int __tmp = tss_choice;
    int prev_task = is_new_task? __tmp >> 3 : cur_task;

    {
        int index = prev_task;

        stack_mirror_arr[index].esp = (unsigned long)(&index);
        stack_mirror_arr[index].size = (ss_seg_base - stack_mirror_arr[index].esp);
        //printf("current esp is %u, base is %u, size is %d\n", stack_mirror_arr[index].esp, ss_seg_base, stack_mirror_arr[index].size);
        stack_mirror_arr[index].pointer =  new (std::nothrow) char[(int)stack_mirror_arr[index].size];
        if( stack_mirror_arr[index].pointer == NULL)
        {
            //printf("why can't alloc?!\n");
            fprintf(stderr, "error when save stack for %d\n", prev_task);
        }
        else
        {
            memcpy((void*)stack_mirror_arr[index].pointer, (void*)stack_mirror_arr[index].esp, stack_mirror_arr[index].size);
        }
    }

    int var = setjmp(jmp_env[prev_task]);
    //printf("setjmp retened %d\n", var);
    if(0 == var)
    {


        if(!is_new_task)
        {
            //printf("cur_task is %u\n", cur_task);
            //printf("swithc to tss_choice = %u\n", tss_choice);
            str();
            //printf("ltr\n");
            ltr(tss_choice);
            //printf("switching to %u\n", cur_task);
            //printf("jmping to %d\n", cur_task);

            if ( 0 != restore_stack(cur_task))
            {
                fprintf(stderr, "error when restore stack for %d\n", prev_task);
            };
            longjmp(jmp_env[cur_task], prev_task);
        }

        return is_new_task;
        //printf("hei,what are you  doing ?!\n");
    }
    else
    {
        if(var == cur_task) return 1;
        return 0;
    }
};


void MutixCPU::switch_to_first_task(int tss_choice)
{
    longjmp(jmp_env[cur_task], cur_task);
};


unsigned long MutixCPU::get_offset_from_gate(const Desc* gate)
{
    unsigned long offset = gate->b & 0xFFFF0000;
    offset |= (0x0000FFFF & gate->a);
    return offset;
}

int MutixCPU::get_choice_from_gate(const Desc* gate)
{
    return (gate->a & 0xffff0000) >> 16;
};

const Desc* MutixCPU::get_desc_from_choice(int choice)
{
    bool is_in_ldt = (choice & (1 << 3));
    const Desc* desc = NULL;
    if(!is_in_ldt)
    {
        const Desc* gdt_base = (Desc*)get_gdt();
        //printf("gdt addr is %u, choice is %u, index is %u, index addr is %u\n",
        //   get_gdt(), choice, choice >> 3, &gdt_base[choice >> 3]);
        return (const Desc*)&gdt_base[choice >> 3];
    }
    else
    {
        const Desc* ldt_base = (Desc*)get_ldt();
        //printf("ldt addr is %u, choice is %u, index is %u, index addr is %u\n",
        //   get_ldt(), choice, choice >> 3, &ldt_base[choice >> 3]);
        return (const Desc*)&ldt_base[choice >> 3];
    }
};

void MutixCPU::deal_with_task(const Desc* gate)
{
    if(DESC_TYPE_TASK_GATE != check_type(gate))
    {
        return;//KernelRescue::on_panic();
    }
    int choice = get_choice_from_gate(gate);
    switch_to(choice);
}

void MutixCPU::deal_with_inter(const Desc* gate)
{
}

void MutixCPU::deal_with_trap(const Desc* gate)
{
    if(DESC_TYPE_TRAP_GATE != check_type(gate))
    {
        return;//KernelRescue::on_panic();
    }
    unsigned long offset = get_offset_from_gate(gate);
    int choice = get_choice_from_gate(gate);
    const Desc* seg_desc = get_desc_from_choice(choice);
    if(DESC_TYPE_CODE_SEG != check_type(gate))
    {
        return;//KernelRescue::on_panic();
    }
    unsigned long seg_base = get_base_from_desc(seg_desc);
    unsigned long seg_limit = get_limit_frome_desc(seg_desc);
    if(offset > seg_limit)
    {
        return;
    }
    op_function addr = (op_function)seg_base + offset;
    addr();
}

void MutixCPU::deal_with_call(const Desc* gate)
{

}


void MutixCPU::deal_with_gate(const Desc* gate)
{
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

