//TODO:
//- TEST TEST TEST
//- Receive command
//- Check if it's OK or failure
//- If it's OK then life is good
//- If It's not OK then log the failure
//- log status of all commands in a file

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
int perform(int bytes);
int read_command();
int delete_command();

const char* GC_LOGNAME = cs1_systems[CS1_GND_COMMANDER]; 
char gc_log_buffer[CS1_MAX_LOG_ENTRY] = {0};

// The file where commands are added to be sent to the satellite
const char CMD_INPUT_FILE[] = "/home/pipes/command-input";  

// TODO What is this file for?
const char CMD_TEMP_FILE[] = "/home/groundCommanderTemp"; //###MAKE SURE TO HAVE WRITE PERMISSIONS###

#define NETMAN_INPUT_PIPE "/home/pipes/gnd-input"
#define NETMAN_OUTPUT_PIPE "/home/pipes/gnd-output"
static NamedPipe nm_output(NETMAN_OUTPUT_PIPE);
static NamedPipe nm_input(NETMAN_OUTPUT_PIPE);

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
    if(!nm_output.Exist()) {
        Shakespeare::log(Shakespeare::NOTICE,GC_LOGNAME,"Creating "NETMAN_OUTPUT_PIPE);
        nm_output.CreatePipe(); 
    };
    if(!nm_input.Exist()) {
        Shakespeare::log(Shakespeare::NOTICE,GC_LOGNAME,"Creating "NETMAN_INPUT_PIPE);
        nm_input.CreatePipe();
    };
#ifdef GROUND_MOCK_SAT
    fprintf(stderr,"Mock configuration\n");
#endif

    fprintf(stderr,"Follow log files in /home/logs for output\n");

    // TODO for mock satellite simulation, the ground and satellite commanders need to be
    // reading from a different set of named pipes
    
    // TODO this is causing segfault
    // commander = new Net2Com(GDcom_w_net_r, GDnet_w_com_r, GIcom_w_net_r, GInet_w_com_r);

    Shakespeare::log(Shakespeare::NOTICE, GC_LOGNAME, "Waiting for commands to send or satellite data");
    while (true)
    {
        memset(info_buffer, 0, sizeof(char) * 255);
        
        //int bytes = commander->ReadFromInfoPipe(info_buffer, 255);
        int bytes = nm_output.ReadFromPipe(info_buffer, CS1_MAX_FRAME_SIZE);
        if (bytes > 0) {
            //get result buffers
            perform(bytes);
        }

        // if no result buffers, proceed to check for commands to send
        read_command();
        sleep(COMMANER_SLEEP_TIME);
    }        
    
    if (commander) {
        delete commander;
        commander = 0;
    }
    return CS1_SUCCESS;
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
        snprintf(gc_log_buffer,CS1_MAX_LOG_ENTRY,"Read from command input file: %s", stored_command.c_str());
        Shakespeare::log(Shakespeare::NOTICE,GC_LOGNAME,gc_log_buffer);
    }

    int data_bytes_written = nm_input.WriteToPipe( stored_command.c_str(), CS1_MAX_FRAME_SIZE );
    // TODO: write to normal pipes
    //int data_bytes_written = commander->WriteToDataPipe( stored_command.c_str() );
    // TODO implement passing size // int data_bytes_written = commander->WriteToDataPipe(result, size);
    
    if (data_bytes_written > 0) 
    {
        delete_command();
    }
    else 
    {
        return CS1_FAILURE;
    }

    infile.close();
    
    return data_bytes_written;
}

/**
 * delete_command will remove the command from the command input file
 * if writing to the pipes was successful
 */
int delete_command()
{
    string read_command;
    ifstream cmd_input_file(CMD_INPUT_FILE);
    
    if( !cmd_input_file.is_open()) {
        Shakespeare::log(Shakespeare::ERROR,GC_LOGNAME,"Command input file failed to open");
        return CS1_FAILURE;
    }
    
    ofstream out(CMD_TEMP_FILE);

    if( !out.is_open()) {
        Shakespeare::log(Shakespeare::ERROR,GC_LOGNAME,"Temp file failed to open. Check directory permmissions");
        return CS1_FAILURE;
    }

    while( getline(cmd_input_file,read_command) ){
        if(read_command.compare(stored_command) != 0){
            out << read_command << "\n";
        }
    }

    cmd_input_file.close();
    out.close();
    remove(CMD_INPUT_FILE);
    rename(CMD_TEMP_FILE,CMD_INPUT_FILE);

    // TODO validate number of bytes removed
    return CS1_SUCCESS;
}

/**
 * The perform function will parse incoming bytes from the Dnet_w_com_r pipe 
 * and attempt to detect result buffers. 
 **/
int perform(int bytes)
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
                            Shakespeare::log(Shakespeare::ERROR, GC_LOGNAME, "Something went wrong !!");
                            read_total = 0;
                            break;
                        }

                        string* obtainedSpaceData = GetResultData(buffer);
                        // TODO this is returning not null 
                        if (obtainedSpaceData != NULL) { // success
                            if (buffer[(uint8_t)MAGIC_BYTE] == GETLOG_CMD) {
                                // TODO: log to proper system (get log)
                            } else {
                                Shakespeare::log(Shakespeare::NOTICE, "GROUND_COMMANDER", obtainedSpaceData->c_str());
                            }

                            delete obtainedSpaceData;
                            obtainedSpaceData = NULL;
                            if (delete_command() != 0) { 
                                return CS1_FAILURE;
                            } // TODO - are we deleting a command from the input file because we received its result?
                            // this is assuming the result buffer corresponds to the last executed command. Not necessarily true.
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
    
    return CS1_SUCCESS; 
}

/** 
 * GetResultData will take the incoming result buffers, create commands using the factory,
 * and call the command ParseResult function to populate the InfoBytes object for 
 * further processing
 * 
 * TODO verify this method is working correctly. TEST
 **/
string* GetResultData(char* result_buffer)
{
            InfoBytes* result2;
            ICommand* command = CommandFactory::CreateCommand(result_buffer);
            result2 = (InfoBytes* )command->ParseResult(result_buffer);

            string *result_data = result2->ToString();       
            cout << *result_data << endl;
            return result_data;
}

