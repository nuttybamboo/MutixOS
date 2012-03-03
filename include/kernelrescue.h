#ifndef KERERRESCUE_H
#define KERERRESCUE_H

#include "../include/systemcall.h"
#include "../include/filesystem.h"
#include "../include/memorymanage.h"
#include "../include/processmanage.h"
#include "../include/interprocesscommu.h"

#define KRM_BASE    0X00
#define KRM_LIMIT   0X10000

#define KERNEL_BASE (KRM_BASE + KRM_LIMIT)

#define MM_BASE KERNEL_BASE
#define MM_LIMIT    0x10000

#define PM_BASE (MM_BASE + MM_LIMIT)
#define PM_LIMIT    0x10000

#define SCI_BASE    (PM_BASE + PM_LIMIT)
#define SCI_LIMIT    0x10000

#define FS_BASE (SCI_BASE + SCI_LIMIT)
#define FS_LIMIT    0x10000

#define IPC_BASE    (FS_BASE + FS_LIMIT)
#define IPC_LIMIT    0x10000

class KernelRescue
{
public:
    KernelRescue();
private:
    void on_panic();

    void InitSCI();
    void InitMM();
    void InitPM();

    void ShowState();

    void ShowMMBasic();
    void ShowMMDetial();
    void ShowPMBasic();
    void ShowPMDetial();
    void ShowFSBasic();
    void ShowFSDetial();
    void ShowIPCBasic();
    void ShowIPCDetial();
    void ShowSCIBasic();
    void ShowSCIDetial();
};

#endif // KERERRESCUE_H
