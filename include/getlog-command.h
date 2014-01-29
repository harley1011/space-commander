#ifndef GETLOG_COMMAND_H
#define GETLOG_COMMAND_H

#include "icommand.h"
#include <cstdlib>
#include <string>

using namespace std;

class GetLogCommand : public ICommand {
public:
    static const size_t MAX_LENGTH;

    GetLogCommand(char subsystem, size_t length) {
        this->subsystem  = subsystem;
        this->length     = length;
    };

    ~GetLogCommand() {}

    void* Execute();

#ifdef PC
    const char* GetLogFolder() { return "/home/spaceconcordia/space/space-commander/logs"; }
#else
    const char* GetLogFolder() { return "/home/logs"; }
#endif

    char GetSubSystem() { return subsystem; }
    size_t GetLength()  { return length; }
private:
    char* ReadLogFile(char* filename, size_t length);
    char* GetLogFilename(char subsystem);
    char subsystem;
    size_t length;
};
#endif
