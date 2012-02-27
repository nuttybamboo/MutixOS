#ifndef IO_H
#define IO_H

#define IO_BUFFER_SIZE  100*4096

#include <vector>

class IO
{
    public:
        IO();
        virtual ~IO();
    protected:
    private:
        static void on_keybord_down(char key);
        static void print(string str) { std::cout << str; };
        static void print(char c) { std::cout << c; };
        static void print(double d) { std::cout << d; };
        static void print(int i) { std::cout << i; };
    private:
        static char buffer[IO_BUFFER_SIZE];
        static int current_index;
};

#endif // IO_H
