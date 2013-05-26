#ifndef GETLOG_COMMAND_H
#define GETLOG_COMMAND_H

#include "icommand.h"

class GetLogCommand : public ICommand {
public:
    GetLogCommand(char subsystem) {
        this->subsystem = subsystem;
    };
    
    ~GetLogCommand() {};

    void Execute() {};
    char GetSubSystem() { return subsystem; };
private:
    char subsystem;
};
#endif
