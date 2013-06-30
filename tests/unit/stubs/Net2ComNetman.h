#ifndef _NET2COM_NETMAN_H_
#define _NET2COM_NETMAN_H_
#include <Net2Com.h>
class Net2ComNetman : public Net2Com {

    public : 
        Net2ComNetman();
        Net2ComNetman(const char*, const char*, const char*, const char*);
        ~Net2ComNetman(); 
        
        int WriteToDataPipe(const char* str);
        char* ReadFromDataPipe(char* buffer);
        int WriteToInfoComPipe(const char* str);
        char* ReadFromInfoComPipe(char* buffer);

    private : 
        bool CreatePipes();                             // The Netman is launched before the Commander, therefore it creates the pipes.


};

#endif
