#ifndef GETTIME_COMMAND_H
#define GETTIME_COMMAND_H

#include "icommand.h"
#include <time.h>
#include <iostream>

using namespace std;

class GetTimeCommand : public ICommand {
public:
    GetTimeCommand() {};
    void* Execute();    // Don't forget to free me. - Willy
};
#endif
