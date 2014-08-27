#ifndef SETTIME_COMMAND_H
#define SETTIME_COMMAND_H
#define SETTIME_CMD_SIZE 9
#include "icommand.h"
#include <time.h>
#include <iostream>
#include <cstdio>

using namespace std;

struct InfoBytesSetTime
{
    char timeStatus;
    time_t timeSet;

};
class SetTimeCommand : public ICommand {
public:
    SetTimeCommand(time_t time ) {
        this->seconds = time;
    }
    SetTimeCommand(char year, char month, char day, char hour, char minute, char second) {
        this->year    = year;
        this->month   = month;
        this->day     = day;
        this->hour    = hour;
        this->minute  = minute;
        this->second  = second;

        // Converts to tv_sec to avoid dealing with leap years.
        time_t rawtime = 0;
        struct tm* timeinfo;
        timeinfo = localtime (&rawtime);

        timeinfo->tm_year = year + 69;
        timeinfo->tm_mon  = month - 1;
        timeinfo->tm_mday = day;
        timeinfo->tm_hour = hour;
        timeinfo->tm_min  = minute;
        timeinfo->tm_sec  = second;

        this->seconds = mktime(timeinfo);
    }
    
    char GetYearSince1900()  { return year; };
    char GetMonth()     { return month; };
    char GetDay()       { return day; };
    char GetHour()      { return hour; };
    char GetMinute()    { return minute; };
    char GetSecond()    { return second; };
    time_t GetSeconds() { return seconds; };
    void* Execute();
    void* ParseResult(const char *result, const char *filename);
        
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
