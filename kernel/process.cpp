/**
File:process.c
Function:The implamention of class Process.
class Process: the abstract of the process in operation system
**/

#include "process.h"
//init constructor...

static int Process::p_counter = 0;

Process::Process()
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

Process::Process(const Process& other)
{
    pid = p_counter++;
    father = other.pid;
    state = P_STATE_WAITE;
    priority = other.priority;
    exit_code = other.exit_code;
    start_code = other.start_code;
    end_code = other.end_code;
    end_data = other.end_data;
    brk = other.brk;
    start_stack = other.start_stack;

    tss.eip = other.tss.eip;
	tss.eflags = other.tss.eflags;
	tss.eax = 0;
	tss.ecx = other.tss.ecx;
	tss.edx = other.tss.edx;
	tss.ebx = other.tss.ebx;
	tss.esp = other.tss.esp;
	tss.ebp = other.tss.ebp;
	tss.esi = other.tss.esi;
	tss.edi = other.tss.edi;
	tss.es = other.tss.es & 0x17;
	tss.cs = other.tss.cs & 0x17;
	tss.ss = other.tss.ss & 0x17;
	tss.ds = other.tss.ds & 0x17;
	tss.fs = other.tss.fs & 0x17;
	tss.gs = other.tss.gs & 0x17;
}
/*
Process& Process::operator=(const Process& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}
//*/
