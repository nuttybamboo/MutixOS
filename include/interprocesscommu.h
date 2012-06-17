#ifndef INTERPROCESSCOMMU_H
#define INTERPROCESSCOMMU_H


class InterProcessCommu
{
    friend class KernelRescue;
public:
    void InterProcessCommuInit();
    //the return value and the parameter is up to you ^___^
    void shm_get();
    void shm_at();
    void sem_open();
    void sem_wait();
    void sem_post();
    void sem_unlink();
private:
    //these only called by the KRM parts to show the state of the system...its private so only the friend class KRM could access...
    static void ShowIPCDetail();
    static void ShowIPCBasic();
private:
    static InterProcessCommu * currentIPC;
    // the other members should be nonesatic..
    //the operations in static access the nonestatic members via the currentIPC Pointer,
    //which poiont to the only object of the InterProcessCommu class in the current system..
};

#endif // INTERPROCESSCOMMU_H
