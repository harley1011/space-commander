#ifndef SETTIME_COMMAND_H
#define SETTIME_COMMAND_H

#define RTC_BYTE_SIZE 1
#define SETTIME_CMD_SIZE (CMD_HEAD_SIZE + sizeof(time_t) + RTC_BYTE_SIZE)
#define SETTIME_RTN_SIZE sizeof(time_t)
#define SETTIME_RTN_SIZE_TOTAL (sizeof(time_t) + CMD_RES_HEAD_SIZE)

#include <cstdio>
#include <iostream>
#include <time.h>

#include "icommand.h"
#include "infobytes.h"

using namespace std;

class InfoBytesSetTime : public InfoBytes {
public:
    char time_status;
    time_t time_set;

    string* ToString() {
        string* infoStatus = new string(1, time_status);
        return infoStatus;
    }

    void setStatus(char status) {
        this->time_status = status;
    }

    void setTime(time_t time) {
        this->time_set = time;
    }

    char getStatus() {
        return time_status;
    }

    time_t getTime() {
        return time_set;
    }
};

class SetTimeCommand : public ICommand {
public:
    SetTimeCommand(time_t time);  
    SetTimeCommand(time_t time, char rtc_bus_number);   
    time_t GetSeconds() { return seconds; };
    void* Execute(size_t* pSize);

    virtual InfoBytes* ParseResult(char* result);
    //char* GetCmdStr(char *cmd_buf);

    char rtc_bus_number;        
private:
    time_t seconds;
};
#endif

