#ifndef COMMAND_FACTORY_H
#define COMMAND_FACTORY_H

#include "icommand.h"
#include "getlog-command.h"
#include "update-command.h"
#include "settime-command.h"
#include "gettime-command.h"
#include "decode-command.h"

class CommandFactory {
public:
    static ICommand* CreateCommand(char* data);

private:
    static ICommand* CreateGetLog(char* data); 
    static ICommand* CreateUpdate(char* data); 
    static ICommand* CreateSetTime(char* data); 
    static ICommand* CreateGetTime(char* data); 
    static ICommand* CreateDecode(char* data); 
};

#endif
