#ifndef SETTIME_COMMAND_H
#define SETTIME_COMMAND_H

#define SETTIME_CMD_SIZE 1 + sizeof(time_t)
#define SETTIME_RTN_SIZE sizeof(time_t)

#include "icommand.h"
#include <time.h>
#include <iostream>
#include <cstdio>

using namespace std;

struct InfoBytesSetTime
{
    char time_status;
    time_t time_set;

};
class SetTimeCommand : public ICommand {
public:
    SetTimeCommand(time_t time);  
    SetTimeCommand(time_t time, char rtc_bus_number);   
    time_t GetSeconds() { return seconds; };
    void* Execute();
    void* ParseResult(const char * result);
    char rtc_bus_number;        
private:
    time_t seconds;
};
#endif
