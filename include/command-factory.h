#ifndef COMMAND_FACTORY_H
#define COMMAND_FACTORY_H

#include "icommand.h"
#include "getlog-command.h"
#include "update-command.h"
#include "settime-command.h"
#include "gettime-command.h"
#include "decode-command.h"
#include "reboot-command.h"

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
        
    static int GetLength3(char* data, int offset);
    static int GetLength10(char* data, int offset);
    static char* GetPath(char* data, size_t length, int offset);
};

#endif
