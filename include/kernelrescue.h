#ifndef KERERRESCUE_H
#define KERERRESCUE_H


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
