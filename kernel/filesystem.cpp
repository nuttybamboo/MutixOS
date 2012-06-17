#include "../include/filesystem.h"

FileSystem * FileSystem::currentFS = 0;

void FileSystem::FileSystemInit()
{
    //ctor
    currentFS = this;
    file_buffer = 0;
}

void FileSystem::on_process_die(int index)
{

}

void FileSystem::fcreate()
{

}

void FileSystem::fdelete()
{

}

void FileSystem::fread()
{

}

void FileSystem::fwrite()
{

}

void FileSystem::fopen()
{

}

void FileSystem::fseek()
{

}

void FileSystem::dircreate()
{

}

void FileSystem::dirdelete()
{

}
