#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <cstring>
#include <signal.h>
#include <unistd.h>
#include <inttypes.h>

#include "Net2Com.h"
#include "command-factory.h"
#include "shakespeare.h"
#include "SpaceDecl.h"

const string LAST_COMMAND_FILENAME("last-command");
const int COMMAND_RESEND_INDEX = 0;
const char COMMAND_RESEND_CHAR = '!';
const int MAX_COMMAND_SIZE     = 255;

const char ERROR_CREATING_COMMAND  = '1';
const char ERROR_EXECUTING_COMMAND = '2';

// Declarations
void out_of_memory_handler();

pid_t get_watch_puppy_pid() 
{
    const int BUFFER_SIZE = 10;
    string filename = CS1_WATCH_PUPPY_PID; 
    char buffer[BUFFER_SIZE] = {0};
    FILE* fp = fopen(filename.c_str(), "r");

    if (fp != NULL) {
        fread(buffer, BUFFER_SIZE, sizeof(char), fp);
        fclose(fp);
        return atoi(buffer);
    } else {
        return 0;
    }
}

void signal_watch_puppy() 
{
    pid_t pid = get_watch_puppy_pid();

    if (pid > 0) {
        kill(pid, SIGUSR2);
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * NAME : main 
 *
 * DESCRIPTION : space-commander main 
 *
 *-----------------------------------------------------------------------------*/
int main() 
{
    set_new_handler(&out_of_memory_handler);

    char info_buffer[255] = {'\0'};
    char previous_command_buffer[MAX_COMMAND_SIZE] = {'\0'};
    char* buffer = NULL;    // TODO  This buffer scared me ! 
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

    Shakespeare::log_3(Shakespeare::NOTICE, "Commander", "Waiting commands from ground...");

    while (true) {
        memset(info_buffer, 0, sizeof(char) * 255);
        int bytes = commander->ReadFromInfoPipe(info_buffer, 255);

        if (bytes > 0) {
            for(int i = 0; i != bytes; i++) {
                read = (unsigned char)info_buffer[i];

                fprintf(stderr, "Read from info pipe = %d bytes\n", read);
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
                                fprintf(stderr, "Read %d bytes from ground station: ", data_bytes);
                                fflush(stdout);
                                for(uint8_t z = 0; z < data_bytes; ++z){
                                    uint8_t c = buffer[z];
                                    fprintf(stderr, "0x%02X ", c);
                                }
                                fprintf(stderr, "\n");
                                fflush(stdout);

                                if (data_bytes != read_total) {
                                    fprintf(stderr, "Something went wrong !!\n");
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

                                    if (fp_last_command != NULL) {
                                        fread(previous_command_buffer, sizeof(char), MAX_COMMAND_SIZE, fp_last_command);
                                        fclose(fp_last_command);
                                        command = CommandFactory::CreateCommand(previous_command_buffer);
                                        if (command != NULL) {
                                            Shakespeare::log_3(Shakespeare::NOTICE, "Commander", "Executing command");

                                            char* result  = (char* )command->Execute();
                                            if (result != NULL) {
                                                fprintf(stderr, "Command output = %s\n", result);
                                                fflush(stdout);

                                                commander->WriteToDataPipe(result);
                                                free(result); // TODO allocate result buffer with new in all icommand subclasses and use delete
                                                result = NULL;
                                            } else {
                                                commander->WriteToInfoPipe(ERROR_EXECUTING_COMMAND);
                                            }

                                            delete command;
                                            command = NULL;
                                        } else {
                                            commander->WriteToInfoPipe(ERROR_CREATING_COMMAND);
                                        }

                                        memset(previous_command_buffer, '\0', MAX_COMMAND_SIZE);
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

                            sleep(COMMANER_SLEEP_TIME);
                            signal_watch_puppy();
                        } //end while

                        read_total = 0;
                        break;
                    }
                    default:
                        read_total += read;
                        buffer = (char* )realloc(buffer, read_total * sizeof(char)); // todo get rid of this realloc! use new instead
                        memset(buffer, 0, sizeof(char) * read_total);
                        break;
                } // end switch
            } // end for

        } // end if


        sleep(COMMANER_SLEEP_TIME);
        signal_watch_puppy();
    }

    if (commander) {
        delete commander;
        commander = 0;
    }

    return 0;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * NAME : out_of_memory_handler 
 *
 * DESCRIPTION : This function is called when memory allocation with new fails.
 *
 *-----------------------------------------------------------------------------*/
void out_of_memory_handler()
{
    std::cerr << "[ERROR] new failed\n";
    throw bad_alloc();
}
