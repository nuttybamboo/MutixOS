/**
File:process.c
Function:The implamention of class Process.
class Process: the abstract of the process in operation system
**/

#include "process.h"
//init constructor...
Process::Process()
{
    pid = 0;
    father = -1;
    state = P_STATE_DEAD;
    priority = 15;
    exit_code = 0;
    start_code = 0;
    end_code = 0;
    end_data = 0;
    brk = 0;
    start_stack = 0;

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
}
/*
Process::Process(const Process& other)
{
    //copy ctor
}
//*/
Process& Process::operator=(const Process& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}
