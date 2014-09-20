#ifndef REBOOT_COMMAND_H_
#define REBOOT_COMMAND_H_
#include "icommand.h"

class RebootCommand : public ICommand {
public:
    RebootCommand() {};
    void* Execute();
};
#endif
