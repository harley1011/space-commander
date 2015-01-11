#ifndef GETTIME_COMMAND_H
#define GETTIME_COMMAND_H

#define GETTIME_CMD_SIZE 2 + sizeof(time_t)
#define GETTIME_RTN_SIZE sizeof(time_t)

#include "icommand.h"
#include "iinfobytes.h"
#include <time.h>
#include <iostream>
#include <sstream>

using namespace std;



struct InfoBytesGetTime : public IInfoBytes
{
    char time_status;
    time_t time_set;

    string * ToString() {
        stringstream ss;
        ss << time_set;
        string* infoStatus = new string(ss.str());
        return infoStatus;
    }

};
class GetTimeCommand : public ICommand {
public:
    GetTimeCommand() {};
    void* Execute();    // Don't forget to free me. - Willy
    static void* ParseResult(const char *result);
};
#endif
