#include "Net2Com.h"
#include "command-factory.h"
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <cstring>
#include <signal.h>

const string LAST_COMMAND_FILENAME("last-command");
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
    FILE* fp_last_command = NULL;
    unsigned int retry = 10000;
    while (retry > 0 && fp_last_command == NULL) {
        fp_last_command = fopen(LAST_COMMAND_FILENAME.c_str(), "w"); 
        retry -= 1;
    }
            
    char info_buffer[255];
    char* buffer = NULL;
    Net2Com* commander = new Net2Com(Dcom_w_net_r, Dnet_w_com_r, Icom_w_net_r, Inet_w_com_r);
    ICommand* command = NULL;
    int read = 0;
    int read_total = 0;

    while (true) {
        memset(info_buffer, 0, sizeof(char) * 255);
        printf("Readng...");
        fflush(stdout);
        commander->ReadFromInfoPipe(info_buffer, 255);
        
        read = atoi(info_buffer);

        fflush(stdout);
        printf("Read = %d", read);

        if (read == 252) {
            read = 0;
            read_total = 0;
        }
        else if (read > 0 && read <= 251) {
            read_total += read;
            buffer = (char* )realloc(buffer, read_total * sizeof(char));
            memset(buffer, 0, sizeof(char) * read_total);

        }
        else if (read == 253 || read == 255) {
            commander->ReadFromDataPipe(buffer, read_total);
                        
            if (fp_last_command) {
                fwrite(buffer, sizeof(char), read_total, fp_last_command); 
                fclose(fp_last_command);
            }

            printf("buffer = %s", buffer);
            fflush(stdout);
            command = CommandFactory::CreateCommand(buffer);
            if (command != NULL) {                
                char* result  = (char* )command->Execute();
                if (result != NULL) {
                    printf("Result = %s", result);
                    fflush(stdout);
                    commander->WriteToDataPipe(result);

                    free(result);
                    result = NULL;
                }
                delete command;
                command = NULL;
            }

            delete buffer;
            buffer = NULL;
        }
        signal_watch_puppy();
    }

    delete commander;
    return 0;
}
