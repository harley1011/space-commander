#ifndef SETTIME_COMMAND_H
#define SETTIME_COMMAND_H

#include "icommand.h"
#include <time.h>
#include <iostream>

using namespace std;

class SetTimeCommand : public ICommand {
public:
    SetTimeCommand(char year, char month, char day, char hour, char minute, char second) {
        this.year    = year;
        this.month   = month;
        this.day     = day;
        this.hour    = hour;
        this.minute  = minute;
        this.second  = second;
        this.seconds = year * month * day * hour * minute * second;
    }

    char GetYearSince1900()  { return year; };
    char GetMonth()     { return month; };
    char GetDay()       { return day; };
    char GetHour()      { return hour; };
    char GetMinute()    { return minute; };
    char GetSecond()    { return second; };
    time_t GetSeconds() { return seconds; };
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
