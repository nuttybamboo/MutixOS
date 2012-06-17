#include "../include/interprocesscommu.h"

InterProcessCommu * InterProcessCommu::currentIPC = 0;

void InterProcessCommu::InterProcessCommuInit()
{
    //ctor
    currentIPC = this;
}

void InterProcessCommu::shm_get()
{

}

void InterProcessCommu::shm_at()
{

}

void InterProcessCommu::sem_open()
{

}

void InterProcessCommu::sem_wait()
{

}

void InterProcessCommu::sem_post()
{

}

void InterProcessCommu::sem_unlink()
{

}

void InterProcessCommu::ShowIPCDetail()
{

}

void InterProcessCommu::ShowIPCBasic()
{

}
