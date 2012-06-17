#ifndef MUTIXMEMORY_H
#define MUTIXMEMORY_H


class MutixMemory
{
public:
    MutixMemory(unsigned long page_size_bit, unsigned long page_num);
    virtual ~MutixMemory();
protected:
private:
    char* _memory;
    unsigned long _page_size_bit;
    unsigned long _page_num;
    unsigned long _memory_size;
    static MutixMemory * _current_memory;
public:
    static unsigned long get_memory_base()
    {
        return (unsigned long) _current_memory->_memory;
    }
    static unsigned long get_address(unsigned long phsical_addr)
    {
        if(_current_memory->_memory_size <= phsical_addr)
        {
            return 0;
        }
        return (unsigned long)((unsigned long)_current_memory->_memory+phsical_addr);
    }
    static unsigned long get_page_size_bit()
    {
        return _current_memory->_page_size_bit;
    }
    static unsigned long get_page_size()
    {
        return 1 << _current_memory->_page_size_bit;
    };
    static unsigned long get_page_num()
    {
        return _current_memory->_page_num;
    };
};

#endif // MUTIXMEMORY_H
