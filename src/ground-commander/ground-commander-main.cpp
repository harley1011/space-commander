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

// TODO these should go in a header file shared by ground and satellite commanders
const string LAST_COMMAND_FILENAME("last-command");
const int COMMAND_RESEND_INDEX = 0;
const char COMMAND_RESEND_CHAR = '!';
const int MAX_COMMAND_SIZE     = 255;
const int MAX_BUFFER_SIZE      = 255;

const char ERROR_CREATING_COMMAND  = '1';
const char ERROR_EXECUTING_COMMAND = '2';

// Declarations
static char info_buffer[255] = {'\0'};
static Net2Com* commander = 0; 
static string stored_command;
string* GetResultData(char* result_buffer);
void perform(int bytes);
int read_command();
void delete_command();

const char* LOGNAME = "GROUND_COMMANDER"; // usually cs1_systems[CS1_COMMANDER];

// TODO
const char CMD_INPUT_FILE[] = "/home/pipes/command-input"; // ? TODO ostensibly the input pipe
const char CMD_TEMP_FILE[] = "/home/groundCommanderTemp"; //###MAKE SURE TO HAVE WRITE PERMISSIONS###


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * NAME : main 
 *
 * DESCRIPTION : ground-commander main 
 * - the ground-commander main should read Dnet_w_com_r and Inet_w_com_r pipes
 *   for incoming data (result buffers) from the satellite
 * - it should also read the Command Input File (CMD_INPUT_FILE) for commands
 *   to be sent to the satellite (command buffers)
 *
 *-----------------------------------------------------------------------------*/
int main() 
{
    // TODO for mock satellite simulation, the ground and satellite commanders need to be
    // reading from a different set of named pipes
    commander = new Net2Com(Dcom_w_net_r, Dnet_w_com_r, Icom_w_net_r, Inet_w_com_r);

    while (true)
    {
        memset(info_buffer, 0, sizeof(char) * 255);
        
        int bytes = commander->ReadFromInfoPipe(info_buffer, 255);
        if (bytes > 0) {
            //get result buffers
            perform(bytes);
        }

        // if no result buffers, proceed to check for commands to send
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

/**
 * read_command will parse a file containing commands, and write them to the
 * info and data pipes as necessary to deliver them to the netman, and 
 * subsequently to the satellite
 */
int read_command()
{
    ifstream infile(CMD_INPUT_FILE);

    // the command input file contains command buffers that are ready to be passed
    // through the pipes to the satellite commander
    if ( infile.good() )
    {
        getline(infile, stored_command);
        cout << stored_command << endl;
    }

    //TODO: write to pipes
    
    //int info_bytes_written = commander->WriteToInfoPipe(ERROR_EXECUTING_COMMAND);

    int data_bytes_written = commander->WriteToDataPipe(stored_command);
    // TODO implement passing size // int data_bytes_written = commander->WriteToDataPipe(result, size);

    if (info_bytes_written > 0) {

    }

    infile.close();
}

/**
 * delete_command will remove the command from the command input file
 * if writing to the pipes was successful
 */
void delete_command(){
    string read_command;
    ifstream in(CMD_INPUT_FILE);
    
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


void perform(int bytes)
{
    char* buffer = NULL; //TODO This buffer does not scare me !
    int read_total = 0;
    unsigned char read = 0;

    for(int i = 0; i != bytes; i++)
    {
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

                        string* obtainedSpaceGarbage = GetResultData(buffer);
                        if (obtainedSpaceGarbage != NULL) { // success
                            if (buffer[(uint8_t)MAGIC_BYTE] == GETLOG_CMD) {
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

string* GetResultData(char* result_buffer)
{
            InfoBytes* result2;
            ICommand* command = CommandFactory::CreateCommand(result_buffer);
            result2 = (InfoBytes* )command->ParseResult(result_buffer);

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
