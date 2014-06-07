#include "Net2Com.h"
#include "command-factory.h"
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <cstring>
#include <signal.h>
#include <unistd.h>
#include <inttypes.h>

const string LAST_COMMAND_FILENAME("last-command");
const int COMMAND_RESEND_INDEX = 0;
const char COMMAND_RESEND_CHAR = '!';
const int MAX_COMMAND_SIZE     = 255;

const char ERROR_CREATING_COMMAND  = '1';
const char ERROR_EXECUTING_COMMAND = '2';

const int SLEEP_TIME = 1;

pid_t get_watch_puppy_pid() {
    const int BUFFER_SIZE = 10;
    string filename = CS1_WATCH_PUPPY_PID; 
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

int main() 
{
    char info_buffer[255];
    char* buffer = NULL;
    char* previous_command_buffer = NULL;
    Net2Com* commander = 0; 
    ICommand* command  = NULL;
    unsigned char read = 0;
    int read_total     = 0;

    commander = new Net2Com(Dcom_w_net_r, Dnet_w_com_r, Icom_w_net_r, Inet_w_com_r);

    if (!commander) {
        fprintf(stderr, "[ERROR] %s:%s:%d Failed in Net2Com instanciation\n", 
                                                __FILE__, __func__, __LINE__);
        return EXIT_FAILURE; /* watch-puppy will take care of 
                              * restarting space-commander
                              */
    }

    printf("Commander waiting commands from ground...\n");

    while (true) {
        memset(info_buffer, 0, sizeof(char) * 255);
        int bytes = commander->ReadFromInfoPipe(info_buffer, 255);

        if (bytes > 0) {
            for(int i = 0; i != bytes; i++) {
                read = (unsigned char)info_buffer[i];

                printf("Read from info pipe = %d bytes\n", read);
                fflush(stdout);

                switch (read) {
                    case 252: break;
                    case 253:
                    case 254:
                    case 255: {

                        int data_bytes = 0;
                        while (data_bytes == 0) {
                            data_bytes = commander->ReadFromDataPipe(buffer, read_total);

                            if (data_bytes > 0) {
                                printf("Read %zu bytes from ground station: ", data_bytes);
                                fflush(stdout);
                                for(uint8_t z = 0; z < data_bytes; ++z){
                                    uint8_t c = buffer[z];
                                    printf("0x%02X ", c);
                                }
                                printf("\n");
                                fflush(stdout);

                                if (data_bytes != read_total) {
                                    printf("Something went wrong !!\n");
                                    fflush(stdout);
                                    read_total = 0;
                                    break;
                                }

                                FILE *fp_last_command = NULL;
                                unsigned int retry = 10000;

                                if(buffer[COMMAND_RESEND_INDEX] == COMMAND_RESEND_CHAR) {
                                    while(retry > 0 && fp_last_command == NULL){
                                        fp_last_command = fopen(LAST_COMMAND_FILENAME.c_str(), "r");
                                        retry -=1;
                                    }

                                    if (fp_last_command != NULL){
                                        previous_command_buffer = (char*) malloc(MAX_COMMAND_SIZE);
                                        fread(previous_command_buffer, sizeof(char), MAX_COMMAND_SIZE, fp_last_command);
                                        fclose(fp_last_command);
                                        command = CommandFactory::CreateCommand(previous_command_buffer);
                                        if (command != NULL) {
                                            printf("%s\n", "Executing command");
                                            fflush(stdout);

                                            char* result  = (char* )command->Execute();
                                            if (result != NULL) {
                                                printf("Command output = %s", result);
                                                fflush(stdout);

                                                commander->WriteToDataPipe(result);
                                                free(result);
                                                result = NULL;
                                            } else {
                                                commander->WriteToInfoPipe(ERROR_EXECUTING_COMMAND);
                                            }

                                            delete command;
                                            command = NULL;
                                        } else {
                                            commander->WriteToInfoPipe(ERROR_CREATING_COMMAND);
                                        }

                                        free(previous_command_buffer);
                                        previous_command_buffer = NULL;

                                    }
                                } else {

                                    while (retry > 0 && fp_last_command == NULL) {
                                        fp_last_command = fopen(LAST_COMMAND_FILENAME.c_str(), "w");
                                        retry -= 1;
                                    }

                                    if (fp_last_command != NULL) {
                                        fwrite(buffer, sizeof(char), data_bytes, fp_last_command);
                                        fclose(fp_last_command);
                                    }
                                }

                                free(buffer);
                                buffer = NULL;
                            }

                            sleep(SLEEP_TIME);
                            signal_watch_puppy();
                        } //end while

                        read_total = 0;
                        break;
                    }
                    default:
                        read_total += read;
                        buffer = (char* )realloc(buffer, read_total * sizeof(char));
                        memset(buffer, 0, sizeof(char) * read_total);
                        break;
                } // end switch
            } // end for

        } // end if


        sleep(SLEEP_TIME);
        signal_watch_puppy();
    }

    delete commander;
    return 0;
}
