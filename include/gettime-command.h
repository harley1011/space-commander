#ifndef GETTIME_COMMAND_H
#define GETTIME_COMMAND_H

#define GETTIME_CMD_SIZE 1
#define GETTIME_RTN_SIZE sizeof(time_t)

#include "icommand.h"
#include <time.h>
#include <iostream>

using namespace std;

struct InfoBytesGetTime
{
    char time_status;
    time_t time_set;

};
class GetTimeCommand : public ICommand {
public:
    GetTimeCommand() {};
    void* Execute(size_t* pSize);    
    static void* ParseResult(const char *result);
};
#endif
