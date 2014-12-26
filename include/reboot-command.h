#ifndef REBOOT_COMMAND_H_
#define REBOOT_COMMAND_H_
#define REBOOT_RTN_SIZE 2
#include "icommand.h"

struct InfoBytesReboot
{
    char reboot_status;
};

class RebootCommand : public ICommand {
public:
    RebootCommand() {};
    void* Execute();
    static void* ParseResult(const char * result);        
};
#endif
