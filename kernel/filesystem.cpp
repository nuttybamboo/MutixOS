#include "../include/filesystem.h"

FileSystem * FileSystem::currentFS = 0;

void FileSystem::FileSystemInit()
{
    //ctor
    currentFS = this;
    file_buffer = 0;
}

void FileSystem::on_process_die(int index){

}
