#ifndef _NET2COM_H_
#define _NET2COM_H_
#include <NamedPipe.h>
/*
enum PIPE_NUM{
    PIPE_ONE, PIPE_TWO, PIPE_THREE, PIPE_FOUR
};*/

class Net2Com{
    protected :
        NamedPipe* pipe1;
        NamedPipe* pipe2;
        NamedPipe* pipe3;
        NamedPipe* pipe4;

    public :
        Net2Com();
        Net2Com(const char*, const char*,const char*, const char*);
        virtual ~Net2Com();
        virtual int WriteToDataPipe(const char* str) = 0;
        virtual char* ReadFromDataPipe(char* buffer) = 0;
        virtual int WriteToInfoComPipe(const char* str) = 0;
        virtual char* ReadFromInfoComPipe(char* buffer) = 0;

    private :
        bool Initialize(const char*, const char*, const char*, const char*);

};

#endif
