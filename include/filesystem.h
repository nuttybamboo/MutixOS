#ifndef FILESYSTEM_H
#define FILESYSTEM_H


class FileSystem
{
    private:
        FileSystem();
    protected:
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
