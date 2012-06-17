#ifndef IO_H
#define IO_H

#define IO_BUFFER_SIZE  100*4096

#include "../include/config.h"

class IO
{
public:
    IO();
    virtual ~IO();
protected:
private:
    static void on_keybord_down(char key);
    static void print(char* str)
    {
        /*g_output << str; */
    };
    static void print(char c)
    {
        /*g_output << c; */
    };
    static void print(double d)
    {
        /*g_output << d; */
    };
    static void print(int i)
    {
        /*g_output << i; */
    };
private:
    static char buffer[IO_BUFFER_SIZE];
    static int current_index;
};

#endif // IO_H
