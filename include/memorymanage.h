#ifndef MEMORYMANAGE_H
#define MEMORYMANAGE_H


class MemoryManage
{
    private:
        MemoryManage();
    protected:
        static void on_page_fault();
    private:
        static long find_wapped_out();
};

#endif // MEMORYMANAGE_H
