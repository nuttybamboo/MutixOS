#include "../include/filesystem.h"

FileSystem * FileSystem::currentFS = 0;

FileSystem::FileSystem()
{
    //ctor
    currentFS = this;
    file_buffer = 0;
}

void FileSystem::on_process_die(int index){

}
