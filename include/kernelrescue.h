#ifndef KERERRESCUE_H
#define KERERRESCUE_H

#include "../include/systemcall.h"
#include "../include/filesystem.h"
#include "../include/memorymanage.h"
#include "../include/processmanage.h"
#include "../include/interprocesscommu.h"



class KernelRescue
{
public:
    KernelRescue();
    void on_panic();

    void InitSCI();
    void InitMM();
    void InitPM();

    static void ShowState();

    static void ShowMMBasic();
    static void ShowMMDetial();
    static void ShowPMBasic();
    static void ShowPMDetial();
    static void ShowFSBasic();
    static void ShowFSDetial();
    static void ShowIPCBasic();
    static void ShowIPCDetial();
    static void ShowSCIBasic();
    static void ShowSCIDetial();
};

#endif // KERERRESCUE_H
