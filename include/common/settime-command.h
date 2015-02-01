#ifndef SETTIME_COMMAND_H
#define SETTIME_COMMAND_H

#define SETTIME_CMD_SIZE 2 + sizeof(time_t)
#define SETTIME_RTN_SIZE sizeof(time_t)

#include "icommand.h"
#include "iinfobytes.h"
#include <time.h>
#include <iostream>
#include <cstdio>

using namespace std;

struct InfoBytesSetTime : public IInfoBytes
{
    char time_status;
    time_t time_set;

    string* ToString() {
        string* infoStatus = new string(1, time_status);
        return infoStatus;
    }
};
class SetTimeCommand : public ICommand {
public:
    SetTimeCommand(time_t time);  
    SetTimeCommand(time_t time, char rtc_bus_number);   
    time_t GetSeconds() { return seconds; };
    void* Execute(size_t* pSize);
    static void* ParseResult(const char * result);
    char rtc_bus_number;        
private:
    time_t seconds;
};
#endif
