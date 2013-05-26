#ifndef SETTIME_COMMAND_H
#define SETTIME_COMMAND_H

#include "icommand.h"
#include <time.h>
#include <iostream>

using namespace std;

class SetTimeCommand : public ICommand {
public:
    SetTimeCommand(unsigned int seconds) {
        this->seconds = seconds; 
        timeval tv;
        tv.tv_sec = seconds;
        time_t nowtime = tv.tv_sec;
        tm *nowtm;
        nowtm = gmtime(&nowtime);
        year  = nowtm->tm_year;
        month = nowtm->tm_mon;
        day   = nowtm->tm_mday;

        char tmbuf[64], buf[64];
        strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
        cout << tmbuf;
    }

    SetTimeCommand(int y, char m, char d, char h, char min, char s) {
        year = y;
        month = m;
        day = d;
        hour = h;
        minute = min;
        second = s;
    };
    

    int GetYearSince1900()  { return year; };
    int GetMonth() { return month; };
    int GetDay()   { return day; };

    void Execute() {};
private:
    int year;
    char month;
    char day;
    char hour;
    char minute;
    char second;
    time_t seconds;
};
#endif
