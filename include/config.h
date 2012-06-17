#ifndef CONFIG_H
#define CONFIG_H

#define MAX_TASK_NUM    255
#define FIRST_TASK  0

//#include "../include/kernelrescue.h"
//#include "../include/process.h"
//#include "../include/processmanage.h"
//#include "../include/memorymanage.h"
//#include "../include/filesystem.h"
//#include "../include/systemcall.h"
//#include "../include/interprocesscommu.h"
//#include "../include/global.h"

//#define sti() __asm__ ("sti"::)
//#define cli() __asm__ ("cli"::)
//#define nop() __asm__ ("nop"::)
//#define iret() __asm__ ("iret"::)

#define KRM_BASE    0X00
#define KRM_LIMIT   4096*10

#define KERNEL_BASE (KRM_BASE + KRM_LIMIT)

#define MM_BASE KERNEL_BASE
#define MM_LIMIT    4096*100

#define PM_BASE (MM_BASE + MM_LIMIT)
#define PM_LIMIT    4096*265

#define SCI_BASE    (PM_BASE + PM_LIMIT)
#define SCI_LIMIT    4096*10

#define FS_BASE (SCI_BASE + SCI_LIMIT)
#define FS_LIMIT    4096*10

#define IPC_BASE    (FS_BASE + FS_LIMIT)
#define IPC_LIMIT    4096*10

#define KERNEL_MEM_END (IPC_BASE + IPC_LIMIT)


#define PROCESS_FORK    0
#define PROCESS_SLEEP   1
#define PROCESS_WAKE_UP    2
#define PROCESS_DIE   3

#endif // CONFIG_H
