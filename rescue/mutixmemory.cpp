#include "../include/mutixmemory.h"

MutixMemory * MutixMemory::_current_memory = 0;

MutixMemory::MutixMemory(unsigned long page_size_bit, unsigned long page_num)
{
    if(0 != _current_memory) return;
    if(page_size_bit > 12)page_size_bit = 12;
    _memory = new char[(1 << page_size_bit)*page_num];
    _page_size_bit = page_size_bit;
    _page_num = page_num;
    _memory_size = (1 << page_size_bit)*page_num;
    _current_memory = this;
}

MutixMemory::~MutixMemory()
{
    delete[] _memory;
    //dtor
}
