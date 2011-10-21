/**
File:process.c
Function:The implamention of class Process.
class Process: the abstract of the process in operation system
**/

#include "process.h"

Process::Process()
{
    //ctor
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
