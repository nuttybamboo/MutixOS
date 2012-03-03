#include "../include/kernelrescue.h"

KernelRescue::KernelRescue()
{
    //ctor
    //setting panic trap gate....
    printf("SCI Initing!\n");
    InitSCI();
    printf("MM Initing!\n");
    InitMM();
    printf("PM Initing!\n");
    InitPM();
    printf("PM Inited!\n");
}

void KernelRescue::InitSCI(){
    SystemCall * system_call = new SystemCall; //(SystemCall *)g_cpu.line2physical_addr(SCI_BASE);
    system_call -> SystemCallInit();
}

void KernelRescue::InitMM(){
    MemoryManage * memory_manage = new MemoryManage(); //(MemoryManage *)g_cpu.line2physical_addr(MM_BASE);
    memory_manage -> MemoryManageInit();
}

void KernelRescue::InitPM(){
    ProcessManage * process_manage = new ProcessManage();//(ProcessManage *)g_cpu.line2physical_addr(PM_BASE);
    printf("PM Initing!\n");
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

