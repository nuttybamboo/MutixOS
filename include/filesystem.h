#ifndef FILESYSTEM_H
#define FILESYSTEM_H


class FileSystem
{
    private:
        FileSystem();
    public:
        static void on_process_die(int index);
        static void fcreate();
        static void fdelete();
        static void fread();
        static void fwrite();
        static void fopen();
        static void fseek();
        static void dircreate();
        static void dirdelete();
    private:
        static char * file_buffer;// shouled be alloced 2M memory at the very begining...

};

#endif // FILESYSTEM_H
