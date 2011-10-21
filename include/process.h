/**
File:process.h
Function:The head file for dafination of class Process.
class Process: the abstract of the process in operation system
**/

#ifndef PROCESS_H
#define PROCESS_H


class Process
{
    protected:
        static Process & current;
    private:
        int pid;
        long seg_adr;
        long seg_rang;
        int father_id;
        int state;
        //there will be the tab property..
    protected:
        Process();
        //Process(const Process& other); bitwise copy maybe a good idea...
        Process& operator=(const Process& other);

        Process& getCurrent();
        int getPid(){
            return pid;
        }
        int getState(){
            return state;
        }
        void setState(int s){
            this -> state = s;
            return ;
        }
    private:
};

#endif // PROCESS_H
