#include "../include/config.h"

KernelRescue::KernelRescue()
{
    //ctor
    //setting panic trap gate....
    InitSCI();
    InitMM();
    InitPM();
}

void KernelRescue::InitSCI(){
    SystemCall * system_call = (SystemCall *)SCI_BASE;
    system_call -> SystemCallInit();
}

void KernelRescue::InitMM(){
    MemoryManage * memory_manage = (MemoryManage *)MM_BASE;
    memory_manage -> MemoryManageInit();
}

void KernelRescue::InitPM(){
    ProcessManage * process_manage = (ProcessManage *)PM_BASE;
    process_manage -> ProcessManageInit();
}

void KernelRescue::ShowState(){
}

void KernelRescue::ShowMMBasic(){
}

void KernelRescue::ShowMMDetial(){
}

void KernelRescue::ShowPMBasic(){
}

void KernelRescue::ShowPMDetial(){
}

void KernelRescue::ShowFSBasic(){
}

void KernelRescue::ShowFSDetial(){
}

void KernelRescue::ShowIPCBasic(){
    InterProcessCommu::ShowIPCBasic();
    return;
}

void KernelRescue::ShowIPCDetial(){
    InterProcessCommu::ShowIPCDetail();
    return;
}

void KernelRescue::ShowSCIBasic(){
}

void KernelRescue::ShowSCIDetial(){
}

