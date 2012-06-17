#include "../include/systemcall.h"

SystemCall * SystemCall::currentSCI = 0;

void inline SystemCall::set_gate(desc_struct * gate_addr, unsigned type, unsigned dpl, unsigned long addr)
{
    gate_addr -> a = 0x00080000 | (0x0000FFFF & addr);
    gate_addr -> b = ( (0x8000 + (dpl << 13) + (type << 8)) & 0x0000FFFF) | (addr & 0xFFFF0000);
    return;
}

void inline SystemCall::set_intr_gate(int index, op_function addr)
{
    set_gate(&(currentSCI -> idt[index]), 14, 0, (unsigned long)addr);
}

void inline SystemCall::set_trap_gate(int index, op_function addr)
{
    set_gate(&(currentSCI -> idt[index]), 15, 0, (unsigned long)addr);
}

void inline SystemCall::set_system_gate(int index, op_function addr)
{
    set_gate(&(currentSCI -> idt[index]), 15, 3, (unsigned long)addr);
}

void inline SystemCall::set_IDT(unsigned long address)
{
    struct _idt_desc
    {
        unsigned long a;
        unsigned long b;
    } idt_desc;
    idt_desc.a = ((unsigned long)(128 * 7 - 1)) << 16;
    idt_desc.b = (unsigned long)address;
    char * idt_desc_pointor = (char*)(unsigned long)(&idt_desc);
    idt_desc_pointor +=2;

    g_cpu->lidt((unsigned long) idt_desc_pointor);

    /*
    __asm__(
        "lidt %0\n\t"
        ::"m" (*idt_desc_pointor):
        );
    */
}

void SystemCall::SystemCallInit()
{
    //ctor
    currentSCI = this;
    idt =
    {
        {
            0x00080000, 0x00008E00
        },
        {
            0x00080000, 0x00008E00
        },
        {
            0x00080000, 0x00008E00
        },
    };
    system_call_table = {0, };
    set_IDT((unsigned long)&idt);
//*
    set_trap_gate(0, &on_divide_error);
    set_trap_gate(1, &on_debug);
    set_trap_gate(2, &on_nmi);
    set_system_gate(3, &on_int3);
    set_system_gate(4, &on_overflow);
    set_system_gate(5, &on_bounds);
    set_trap_gate(6, &on_invalid_op);
    set_trap_gate(7, &on_device_not_available);
    set_trap_gate(8, &on_double_fault);
    set_trap_gate(9, &on_coprocessor_segment_overrun);
    set_trap_gate(10, &on_invalid_TSS);
    set_trap_gate(11, &on_segment_not_present);
    set_trap_gate(12, &on_stack_segment);
    set_trap_gate(13, &on_general_protection);
    set_trap_gate(14, &on_reserved); //page_fault
    set_trap_gate(15, &on_reserved);
    set_trap_gate(16, &on_coprocessor_error);
    for (int i = 17; i < 48; i++)
    {
        set_trap_gate(i, &on_reserved);
    }

    set_trap_gate(45, &on_irq13);

    set_trap_gate(39, &on_parallel_interrupt);

    set_intr_gate(0x20, &on_timer_interrupt);
//*/
    set_system_gate(0x74, &on_system_call);

    //traps and interaption setting...
}

void SystemCall::SetSystemCall(int system_call_number, syscall_op_function fuction)
{
    currentSCI -> system_call_table[system_call_number] = fuction;
    return;
}

void SystemCall::SetPageFaultTraps(op_function fuction)
{
    set_trap_gate(14, fuction);
}


void SystemCall::on_system_call()
{

    int eax = g_cpu->get_eax();
    //__asm__("nop;":"=a" (eax):);

    if(eax >= SYSTEMCALL_MAX_NUM || !currentSCI -> system_call_table[eax])
    {
        //panic("bad sys call");
        return ;
    }

    /*
      __asm__(
    "push %%ds\n\t"
    "push %%es\n\t"
    "push %%fs\n\t"
    "pushl %%edx\n\t"
    "pushl %%ecx\n\t"
    "pushl %%ebx\n\t"
    "movl $0x10,%%edx\n\t"
    "mov %%dx,%%ds\n\t"
    "mov %%dx,%%es\n\t"
    "movl $0x17,%%edx\n\t"
    "mov %%dx,%%fs\n\t"
    ::);
    */
    g_cpu->push(g_cpu->get_ds());
    g_cpu->push(g_cpu->get_es());
    g_cpu->push(g_cpu->get_fs());
    g_cpu->push(g_cpu->get_edx());
    g_cpu->push(g_cpu->get_ecx());
    g_cpu->push(g_cpu->get_ebx());
    g_cpu->set_edx( 0x10 );
    g_cpu->set_ds(g_cpu->get_edx());
    g_cpu->set_es(g_cpu->get_edx());
    g_cpu->set_edx( 0x17 );
    g_cpu->set_fs(g_cpu->get_edx());

    eax = currentSCI -> system_call_table[eax]();

    g_cpu->set_eax(eax);
    g_cpu->set_ebx(g_cpu->pop());
    g_cpu->set_ecx(g_cpu->pop());
    g_cpu->set_edx(g_cpu->pop());
    g_cpu->set_fs(g_cpu->pop());
    g_cpu->set_es(g_cpu->pop());
    g_cpu->set_ds(g_cpu->pop());

    //"call sys_call_table(,%eax,4)\n\t"
    /*
    	__asm__(
        "movl %0 %%eax\n\t"
    	"popl %%ebx\n\t"
    	"popl %%ecx\n\t"
    	"popl %%edx\n\t"
    	"pop %%fs\n\t"
    	"pop %%es\n\t"
    	"pop %%ds\n\t"
    	::"m" (*&eax));
    	//*/
    return;
}

//well, I don't know what to do with thess op yet....
void SystemCall::on_divide_error()
{
}

void SystemCall::on_debug()
{
}

void SystemCall::on_nmi()
{
}

void SystemCall::on_int3()
{
}

void SystemCall::on_overflow()
{
}

void SystemCall::on_bounds()
{
}

void SystemCall::on_invalid_op()
{
}

void SystemCall::on_device_not_available()
{
}

void SystemCall::on_double_fault()
{
}

void SystemCall::on_coprocessor_segment_overrun()
{
}

void SystemCall::on_invalid_TSS()
{
}

void SystemCall::on_segment_not_present()
{
}

void SystemCall::on_stack_segment()
{
}

void SystemCall::on_general_protection()
{
}

void SystemCall::on_reserved()
{
}

void SystemCall::on_coprocessor_error()
{
}

void SystemCall::on_irq13()
{
}

void SystemCall::on_parallel_interrupt()
{
}

void SystemCall::on_timer_interrupt()
{
}

