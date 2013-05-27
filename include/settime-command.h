#ifndef SETTIME_COMMAND_H
#define SETTIME_COMMAND_H

#include "icommand.h"
#include <time.h>
#include <iostream>

using namespace std;

class SetTimeCommand : public ICommand {
public:
    SetTimeCommand(time_t seconds) {
        this->seconds  = seconds; 
        time_t nowtime = seconds;

        tm *nowtm = gmtime(&this->seconds);

        year   = nowtm->tm_year;
        month  = nowtm->tm_mon;
        day    = nowtm->tm_mday;
        hour   = nowtm->tm_hour;
        minute = nowtm->tm_min;
        second = nowtm->tm_sec;
    }

    char GetYearSince1900()  { return year; };
    char GetMonth()  { return month; };
    char GetDay()    { return day; };
    char GetHour()   { return hour; };
    char GetMinute() { return minute; };
    char GetSecond() { return second; };

    void Execute() {};
private:
    char year;
    char month;
    char day;
    char hour;
    char minute;
    char second;
    time_t seconds;
};
#endif
