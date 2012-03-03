#ifndef MUTIXMEMORY_H
#define MUTIXMEMORY_H


class MutixMemory
{
    public:
        MutixMemory(int page_size_bit, int page_num);
        virtual ~MutixMemory();
    protected:
    private:
        char* _memory;
        int _page_size_bit;
        int _page_num;
    public:
        unsigned long get_lowmem(){return (unsigned long) _memory;};
        int get_page_size_bit(){return _page_size_bit;}
        int get_page_size(){return 1 << _page_size_bit;};
        int get_page_num(){return _page_num;};
};

#endif // MUTIXMEMORY_H
