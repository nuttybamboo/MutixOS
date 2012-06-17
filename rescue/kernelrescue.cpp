#include "../include/kernelrescue.h"

KernelRescue::KernelRescue()
{
    printf("SCI Initing!\n");
    InitSCI();
    printf("MM Initing!\n");
    InitMM();
    printf("PM Initing!\n");
    InitPM();
    printf("PM Inited!\n");
}


void KernelRescue::InitSCI()
{
    SystemCall * system_call = (SystemCall *)g_memory->get_address(SCI_BASE);
    printf("get address SCI_BASE %u is %u\n", SCI_BASE, system_call);
    new (system_call) SystemCall;
    system_call -> SystemCallInit();
}

void KernelRescue::InitMM()
{
    MemoryManage * memory_manage = (MemoryManage *)g_memory->get_address(MM_BASE);
    printf("get address MM_BASE %u is %u\n", MM_BASE, memory_manage);
    memory_manage -> MemoryManageInit();
}

void KernelRescue::InitPM()
{
    ProcessManage * process_manage = (ProcessManage *)g_memory->get_address(PM_BASE);
    process_manage -> ProcessManageInit();
}

void KernelRescue::ShowState()
{
    ShowMMBasic();
    ShowPMBasic();
}

void KernelRescue::ShowMMBasic()
{
    MemoryManage * memory_manage = MemoryManage::currentMM;
    printf("\n\n\n\*******************MemoryManage State****************************************\n"
           "Showing memory manage model state:\nPAGE_DIR_SIZE is : %d\npage_dir addr is : %u\ngdt addr is : %u\nldt addr is :%u\n"
           "memory_map addr is :%u\n\n\n"
           , sizeof(memory_manage->page_dir) / sizeof(unsigned long)
           , &(memory_manage->page_dir)
           , &(memory_manage->gdt)
           , &(memory_manage->ldt_array)
           , memory_manage->memory_map);
}

void KernelRescue::ShowMMDetial()
{
    MemoryManage * memory_manage = MemoryManage::currentMM;
    printf("\n\n\n\*******************MemoryManage Detial****************************************\n"
           "memory state as follows:\n"
           "R for kernel rescue, M for memory management, P for process managemant, \n"
           "S for system call interface, F for file system, C for inner process communite, \n"
           "the number 0 or 1 for the memory is free or used by the user process\n\n\n");
    for(size_t i = 0; i < KRM_LIMIT/PAGE_SIZE; ++i)
    {
        printf("R");
    }
    for(size_t i = 0; i < MM_LIMIT/PAGE_SIZE; ++i)
    {
        printf("M");
    }
    for(size_t i = 0; i < PM_LIMIT/PAGE_SIZE; ++i)
    {
        printf("P");
    }
    for(size_t i = 0; i < SCI_LIMIT/PAGE_SIZE; ++i)
    {
        printf("S");
    }
    for(size_t i = 0; i < FS_LIMIT/PAGE_SIZE; ++i)
    {
        printf("F");
    }
    for(size_t i = 0; i < IPC_LIMIT/PAGE_SIZE; ++i)
    {
        printf("C");
    }
    for(size_t i = 0; i < PAGING_PAGES; ++i)
    {
        printf("%d", memory_manage->memory_map[i]);
    }
    printf("\n\ntotal %d blocks\n\n\n", (KRM_LIMIT + MM_LIMIT + PM_LIMIT + SCI_LIMIT
                                     + FS_LIMIT + IPC_LIMIT)/PAGE_SIZE + PAGING_PAGES);
}

void KernelRescue::ShowPMBasic()
{
    ProcessManage * process_manage = ProcessManage::currentPM;
    printf("\n\n\n\*******************ProcessManage State****************************************\n"
           "current task pid is %d\ncurrent task index is %d\n",
           process_manage->getCurrentTaskAddr()->getPid(),
           process_manage->find_process(process_manage->getCurrentTaskAddr()->getPid()));
    printf("task_array addr is %u, task_array size is %u\n"
           "process_memeory_map addr is %u, process_memeory_map size is %u\n\n"
           , &(process_manage->task_array), sizeof(process_manage->task_array)
           , &(process_manage->process_memeory_map)
           , sizeof(process_manage->process_memeory_map));
    printf("pid\tfather\tstate\tpriority\n");
    for(size_t i = 0; i < MAX_TASK_NUM; ++i){
        if(process_manage -> task_array[i])
        {
            printf("%d\t%d\t%d\t%d\t\n",
                   process_manage->getTaskAddrByIndex(i)->getPid(),
                   process_manage->getTaskAddrByIndex(i)->getFather(),
                   process_manage->getTaskAddrByIndex(i)->getState(),
                   process_manage->getTaskAddrByIndex(i)->getPriority());
        }
    }
}

void KernelRescue::ShowPMDetial()
{
    ProcessManage * process_manage = ProcessManage::currentPM;
    printf("\n\n\n\*******************ProcessManage Detial****************************************\n"
           "current task's PCB as follows:\n\n\n"
           "\tback_link = 0X%hu\n"	/* 16 high bits zero */
           "\tesp0 = 0X%hu\n"
           "\tss0 = 0X%hu\n"	/* 16 high bits zero */
           "\tesp1 = 0X%hu\n"
           "\tss1 = 0X%hu\n"		/* 16 high bits zero */
           "\tesp2 = 0X%hu\n"
           "\tss2 = 0X%hu\n"		/* 16 high bits zero */
           "\tcr3 = 0X%hu\n"
           "\teip = 0X%hu\n"
           "\teflags = 0X%hu\n"
           "\teax = 0X%hu\n"
           "\tecx = 0X%hu\n"
           "\tedx = 0X%hu\n"
           "\tebx = 0X%hu\n"
           "\tesp = 0X%hu\n"
           "\tebp = 0X%hu\n"
           "\tesi = 0X%hu\n"
           "\tedi = 0X%hu\n"
           "\tes = 0X%hu\n"
           "\tss = 0X%hu\n"
           "\tss = 0X%hu\n"
           "\tds = 0X%hu\n"
           "\tfs = 0X%hu\n"
           "\tgs = 0X%hu\n"
           "\tldt = 0X%hu\n"
           "\ttrace_bitmap = 0X%hu\n"
           "pid = %d\n"
           "father = %d\n"
           "state = %d\n"
           "priority = %d\n"
           "exit_code = %d\n"
           "start_code = %d\n"
           "end_code = %d\n"
           "end_data = %d\n"
           "brk = %d\n"
           "start_stack = %d\n",
           process_manage->getCurrentTaskAddr()->tss.back_link,
           process_manage->getCurrentTaskAddr()->tss.esp0,
           process_manage->getCurrentTaskAddr()->tss.ss0,
           process_manage->getCurrentTaskAddr()->tss.esp1,
           process_manage->getCurrentTaskAddr()->tss.ss1,
           process_manage->getCurrentTaskAddr()->tss.esp2,
           process_manage->getCurrentTaskAddr()->tss.ss2,
           process_manage->getCurrentTaskAddr()->tss.cr3,
           process_manage->getCurrentTaskAddr()->tss.eip,
           process_manage->getCurrentTaskAddr()->tss.eflags,
           process_manage->getCurrentTaskAddr()->tss.eax,
           process_manage->getCurrentTaskAddr()->tss.ecx,
           process_manage->getCurrentTaskAddr()->tss.edx,
           process_manage->getCurrentTaskAddr()->tss.ebx,
           process_manage->getCurrentTaskAddr()->tss.esp,
           process_manage->getCurrentTaskAddr()->tss.ebp,
           process_manage->getCurrentTaskAddr()->tss.esi,
           process_manage->getCurrentTaskAddr()->tss.edi,
           process_manage->getCurrentTaskAddr()->tss.es,
           process_manage->getCurrentTaskAddr()->tss.cs,
           process_manage->getCurrentTaskAddr()->tss.ss,
           process_manage->getCurrentTaskAddr()->tss.ds,
           process_manage->getCurrentTaskAddr()->tss.fs,
           process_manage->getCurrentTaskAddr()->tss.gs,
           process_manage->getCurrentTaskAddr()->tss.ldt,
           process_manage->getCurrentTaskAddr()->tss.trace_bitmap,
           process_manage->getCurrentTaskAddr()->pid,
           process_manage->getCurrentTaskAddr()->father,
           process_manage->getCurrentTaskAddr()->state,
           process_manage->getCurrentTaskAddr()->priority,
           process_manage->getCurrentTaskAddr()->exit_code,
           process_manage->getCurrentTaskAddr()->start_code,
           process_manage->getCurrentTaskAddr()->end_code,
           process_manage->getCurrentTaskAddr()->end_data,
           process_manage->getCurrentTaskAddr()->brk,
           process_manage->getCurrentTaskAddr()->start_stack
           );
}

void KernelRescue::ShowFSBasic()
{
}

void KernelRescue::ShowFSDetial()
{
}

void KernelRescue::ShowIPCBasic()
{
    InterProcessCommu::ShowIPCBasic();
    return;
}

void KernelRescue::ShowIPCDetial()
{
    InterProcessCommu::ShowIPCDetail();
    return;
}

void KernelRescue::ShowSCIBasic()
{
}

void KernelRescue::ShowSCIDetial()
{
}

