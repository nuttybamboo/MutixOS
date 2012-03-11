/**
File:process.c
Function:The implamention of class Process.
class Process: the abstract of the process in operation system
**/

#include "../include/process.h"
//init constructor...

class MemoryManage;

int Process::p_counter = 0;
Process * Process::current = 0;

void Process::ProcessInit()
{
    pid = p_counter++;
    father = -1;
    state = P_STATE_DEAD;
    priority = 15;
    exit_code = 0;
    start_code = 0;
    end_code = 0;
    end_data = 0;
    brk = 0;
    start_stack = 0;
/*
    ldt = {
        {0,0},
        {0x9f,0xc0fa00},
		{0x9f,0xc0f200}
		};
//*/
	tss.back_link = 0;	/* 16 high bits zero */
	tss.esp0 = 0;
	tss.ss0 = 0;		/* 16 high bits zero */
	tss.esp1 = (long)(&stack + STACK_SIZE);
	tss.ss1 = 0x10;		/* 16 high bits zero */
	tss.esp2 = 0;
	tss.ss2 = 0;		/* 16 high bits zero */
	tss.cr3 = MemoryManage::page_dir_address();
    tss.eip = 0;
	tss.eflags = 0;
	tss.eax = 0;
	tss.ecx = 0;
	tss.edx = 0;
	tss.ebx = 0;
	tss.esp = 0;
	tss.ebp = 0;
	tss.esi = 0;
	tss.edi = 0;
	tss.es = 0x17;
	tss.cs = 0x17;
	tss.ss = 0x17;
	tss.ds = 0x17;
	tss.fs = 0x17;
	tss.gs = 0x17;
	tss.ldt = MemoryManage::get_LDT_choice(FIRST_TASK);
    tss.trace_bitmap = 0x80000000;
}


void Process::ProcessCopy(const Process* other, const int& task_index)
{
 //   *this = other;//bit wise?
printf("why? other is %d\n", other);
    pid = p_counter++;
    printf("why?\n");
    father = other->pid;
    state = P_STATE_WAITE;
    printf("why?\n");
//*
    priority = father == 0 ? 20:other->priority;
    exit_code = other->exit_code;
    start_code = other->start_code;
    end_code = other->end_code;
    end_data = other->end_data;
    brk = other->brk;
    start_stack = other->start_stack;
//*/
    tss = other->tss;//bit wise copy?
printf("why?\n");
    tss.back_link = 0;	/* 16 high bits zero */
	tss.esp1 = (long)(&stack + STACK_SIZE);
	tss.ss1 = 0x10;		/* 16 high bits zero */
	tss.eax = 0;

	tss.ldt = MemoryManage::get_LDT_choice(task_index);
	tss.trace_bitmap = 0x80000000;
}
/*
Process& Process::operator=(const Process& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}
//*/
