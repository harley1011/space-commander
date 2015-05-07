#ifndef GETTIME_COMMAND_H
#define GETTIME_COMMAND_H

#define GETTIME_CMD_SIZE 1
#define GETTIME_RTN_SIZE sizeof(time_t)

#include "icommand.h"
#include "infobytes.h"
#include <time.h>
#include <iostream>
#include <sstream>

using namespace std;



class InfoBytesGetTime : public InfoBytes
{
    public:
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
    void* Execute(size_t* pSize);    
    InfoBytes* ParseResult(char *result);
};
#endif
