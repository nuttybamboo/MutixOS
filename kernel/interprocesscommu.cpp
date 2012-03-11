#include "../include/interprocesscommu.h"

InterProcessCommu * InterProcessCommu::currentIPC = 0;

void InterProcessCommu::InterProcessCommuInit()
{
    //ctor
    currentIPC = this;
}
