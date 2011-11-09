#include "../include/interprocesscommu.h"

InterProcessCommu * InterProcessCommu::currentIPC = 0;
InterProcessCommu::InterProcessCommu()
{
    //ctor
    currentIPC = this;
}
