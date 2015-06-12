#ifndef COMMAND_FACTORY_H
#define COMMAND_FACTORY_H

#include "decode-command.h"
#include "deletelog-command.h"
#include "getlog-command.h"
#include "gettime-command.h"
#include "icommand.h"
#include "reboot-command.h"
#include "settime-command.h"
#include "update-command.h"

class CommandFactory {
public:
    static ICommand* CreateCommand(char* data);

private:
    static ICommand* CreateGetLog(char* data); 
    static ICommand* CreateUpdate(char* data); 
    static ICommand* CreateSetTime(char* data); 
    static ICommand* CreateGetTime(char* data); 
    static ICommand* CreateReboot(char* data);
    static ICommand* CreateDecode(char* data); 
    static ICommand* CreateDeleteLog(char* data); 
        
    static int GetLength3(char* data, int offset);
    static int GetLength10(char* data, int offset);

    // This method allocate memory on the heap. Free it!
    static char* GetPath(char* data, size_t length, int offset);        
};

#endif
