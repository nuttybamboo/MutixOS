#include "../include/mutixmemory.h"

MutixMemory::MutixMemory(int page_size_bit, int page_num)
{
    if(page_size_bit > 12)page_size_bit = 12;
    _memory = new char[1 >> page_size_bit, page_num];
    _page_size_bit = page_size_bit;
    _page_num = page_num;
}

MutixMemory::~MutixMemory()
{
    //dtor
}
