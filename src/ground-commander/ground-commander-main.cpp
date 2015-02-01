#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <cstring>
#include <signal.h>
#include <sstream>
#include <unistd.h>
#include <inttypes.h>
#include <fstream>

#include "space-commander/Net2Com.h"
#include "common/command-factory.h"
#include "shakespeare.h"
#include "common/subsystems.h"
#include "SpaceDecl.h"

const char MAGIC_BYTE = 0;

const string LAST_COMMAND_FILENAME("last-command");
const int COMMAND_RESEND_INDEX = 0;
const char COMMAND_RESEND_CHAR = '!';
const int MAX_COMMAND_SIZE     = 255;
const int MAX_BUFFER_SIZE      = 255;

const char ERROR_CREATING_COMMAND  = '1';
const char ERROR_EXECUTING_COMMAND = '2';

// Declarations
static char log_buffer[255] = {0};
static char info_buffer[255] = {'\0'};
static Net2Com* commander = 0; 
static string stored_command;

const char* LOGNAME = cs1_systems[CS1_COMMANDER];
const char CMD_INPUT_FILE[] = "/home/todo"; 
const char CMD_TEMP_FILE[] = "/home/groundCommanderTemp"; //###MAKE SURE TO HAVE WRITE PERMISSIONS###
string* GetGarbage(char* result_buffer);
void perform(int bytes);
void read_command();
void delete_command();
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * NAME : main 
 *
 * DESCRIPTION : space-commander main 
 *
 *-----------------------------------------------------------------------------*/
int main() 
{
    commander = new Net2Com(Dcom_w_net_r, Dnet_w_com_r, Icom_w_net_r, Inet_w_com_r);

    while (true)
    {
        memset(info_buffer, 0, sizeof(char) * 255);
        
        //int bytes = commander->ReadFromInfoPipe(info_buffer, 255);
        int bytes = 1;
        if (bytes > 0) {
            //get result
 //           perform(bytes);
        }

        read_command();
        delete_command();
        sleep(COMMANER_SLEEP_TIME);
    }        
    
    if (commander) {
        delete commander;
        commander = 0;
    }
    return 0;
}

void read_command(){
    ifstream infile(CMD_INPUT_FILE);

    if(infile.good()){
        getline(infile, stored_command);
        cout << stored_command << endl;
    }

    //TODO: write to pipes
    infile.close();
}

void delete_command(){
    string read_command;
    ifstream in(CMD_INPUT_FILE);
    int temp;
    
    if( !in.is_open()){
        cout << "Command input file failed to open" << endl;
        return;
    }
    
    ofstream out(CMD_TEMP_FILE);

    if( !out.is_open()){
        cout << "Temp file failed to open. Check directory permmissions" << endl;
        return;
    }

    while( getline(in,read_command) ){
        if(read_command.compare(stored_command) != 0){
            out << read_command << "\n";
        }
    }

    in.close();
    out.close();    
    remove(CMD_INPUT_FILE);
    rename(CMD_TEMP_FILE,CMD_INPUT_FILE);
}


void perform(int bytes){
    char* buffer = NULL; //TODO This buffer does not scare me !
    int read_total = 0;
    unsigned char read = 0;

    for(int i = 0; i != bytes; i++){
        read = (unsigned char)info_buffer[i];
        switch (read) 
        {
            case 252: 
                break;
            case 253:
            case 254:
            case 255: 
            {
                int data_bytes = 0;
                while (data_bytes == 0) {
                    data_bytes = commander->ReadFromDataPipe(buffer, read_total);

                    if (data_bytes > 0) {
                        if (data_bytes != read_total) {
                            Shakespeare::log(Shakespeare::ERROR, LOGNAME, "Something went wrong !!");
                            read_total = 0;
                            break;
                        }

                        string* obtainedSpaceGarbage = GetGarbage(buffer);
                        if (obtainedSpaceGarbage != NULL) { // success
                            if (buffer[MAGIC_BYTE] == GETLOG_CMD) {
                                // TODO: log to proper system (get log)
                            } else {
                                Shakespeare::log(Shakespeare::NOTICE, "GROUND_COMMANDER", obtainedSpaceGarbage->c_str());
                            }

                            delete obtainedSpaceGarbage;
                            obtainedSpaceGarbage = NULL;
                            delete_command();
                        }

                        free(buffer);
                        buffer = NULL;
                    }
                    
                    sleep(COMMANER_SLEEP_TIME); //We have to sleep because we got data from the info pipe. Wait for the data pipe to be ready
                    
                }

                read_total = 0;
                break;

            }
            default:
                read_total += read;
                buffer = (char* )realloc(buffer, read_total * sizeof(char));
                memset(buffer, 0, sizeof(char) * read_total);
            break;
        }
    }
    

    
    return; 
}

string* GetGarbage(char* result_buffer){

            IInfoBytes* result2;
            switch(result_buffer[MAGIC_BYTE]){
                
                case UPDATE_CMD: 
                    result2 = (IInfoBytes* )UpdateCommand::ParseResult(result_buffer);
                    break;
                case GETTIME_CMD: 
                    result2 = (IInfoBytes* )GetTimeCommand::ParseResult(result_buffer);
                    break;
                case SETTIME_CMD:
                    result2 = (IInfoBytes* )SetTimeCommand::ParseResult(result_buffer);
                    break;
//                case GETLOG_CMD:
//                    result2 = (IInfoBytes* )GetLogCommand::ParseResult(result_buffer);
//                    break;
//                case DELETELOG_CMD:
//                    result2 = (IInfoBytes* )DeleteLogCommand::ParseResult(result_buffer);
//                    break;
                case REBOOT_CMD:
                    result2 = (IInfoBytes* )RebootCommand::ParseResult(result_buffer);
                    break;
                case DECODE_CMD:
                    result2 = (IInfoBytes* )DecodeCommand::ParseResult(result_buffer);
                    break;
                default:
                    cout << "Goodbye world!" << endl;
            }

            string *garbage = result2->ToString();       
            cout << *garbage << endl;
            return garbage;
}
//TODO:
//- Receive command
//- Check if it's OK or failure
//- If it's OK then life is good
//- If It's not OK then log the failure
//- log status of all commands in a file
