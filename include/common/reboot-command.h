#ifndef REBOOT_COMMAND_H_
#define REBOOT_COMMAND_H_
#define REBOOT_RTN_SIZE 2
#include "icommand.h"
#include "infobytes.h"
using namespace std;

class InfoBytesReboot : public InfoBytes
{
    public:
    char reboot_status;

    string * ToString() {
        return new string(1, reboot_status);
    }
};

class RebootCommand : public ICommand {
public:
    RebootCommand() {};
    void* Execute(size_t* pSize);
    InfoBytes* ParseResult(char* result);        
};
#endif
