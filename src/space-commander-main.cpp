#include "Net2Com.h"
#include "command-factory.h"
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <cstring>
#include <signal.h>


pid_t get_watch_puppy_pid() {
    const int BUFFER_SIZE = 10;
    string filename = "/home/pids/watch-puppy.pid";
    char buffer[BUFFER_SIZE] = {0};
    FILE* fp = fopen(filename.c_str(), "r");

    if (fp != NULL) {
        fread(buffer, BUFFER_SIZE, sizeof(char), fp);
        fclose(fp);
        return atoi(buffer);
    }
    else {
        return 0;
    }
}

void signal_watch_puppy() {
    pid_t pid = get_watch_puppy_pid();
    if (pid > 0) {
        kill(pid, SIGUSR2);
    }
}

int main() {
    Net2Com* commander = new Net2Com(PIPE_TWO, PIPE_ONE, PIPE_FOUR, PIPE_THREE);
    char buffer[255];
    ICommand* command = NULL;

    while (true) {
        memset(buffer, 0, sizeof(char) * 255);
        commander->ReadFromDataPipe(buffer, 255);
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

        signal_watch_puppy();
    }

    delete commander;
    return 0;
}
