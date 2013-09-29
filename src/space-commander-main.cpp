#include "Net2Com.h"
#include "command-factory.h"
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <cstring>

int main() {

    Net2Com* commander = new Net2Com(PIPE_TWO, PIPE_ONE, PIPE_FOUR, PIPE_THREE);
    char buffer[255];
    ICommand* command = NULL;

    while (true) {
        memset(buffer, 0, sizeof(char) * 255);
        commander->ReadFromDataPipe(buffer);
        printf("buffer = %s", buffer);
        command = CommandFactory::CreateCommand(buffer);
        
        if (command != NULL) {
            time_t* currentTime = (time_t*)command->Execute();
            struct tm* timeinfo;
            timeinfo = localtime(currentTime);
            printf("Current time = %s", asctime(timeinfo));
            free(currentTime);
        }
        delete command;
    }

    delete commander;
    return 0;
}
