#ifndef REBOOT_COMMAND_H_
#define REBOOT_COMMAND_H_
#define REBOOT_RTN_SIZE 2
#include "icommand.h"
#include "iinfobytes.h"
using namespace std;

struct InfoBytesReboot : public IInfoBytes
{
    char reboot_status;

    string * ToString() {
        return new string(1, reboot_status);
    }
};

class RebootCommand : public ICommand {
public:
    RebootCommand() {};
    void* Execute();
    static void* ParseResult(const char * result);        
};
#endif
