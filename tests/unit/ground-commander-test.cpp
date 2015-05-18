/******************************************************************************
*
* AUTHOR : Shawn
*
* FILE : ground-commander-test.cpp
*
* PURPOSE : Test the ground-commander using the Net2Com interface.
*
* CREATION DATE : 2015-05-18
*
******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  
#include <sys/wait.h>

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"

#include "SpaceDecl.h"
#include "common/command-factory.h"
#include "common/icommand.h"
#include "fileIO.h"
#include "SpaceString.h"
#include "space-commander/Net2Com.h"
#include "dirUtl.h"
#include "UTestUtls.h"

#define RESULT_BUF_SIZE 50 // TODO this should be globally defined
#define CMD_BUF_SIZE 25 // TODO this should be globally defined

static char command_buf[CMD_BUF_SIZE] = {'\0'};
#define SPACE_COMMANDER_BIN  "bin/space-commander/space-commander" // use local bin, not the one unser CS1_APPS
#define GROUND_COMMANDER_BIN "bin/ground-commander/ground-commander" // use local bin, not the one unser CS1_APPS

#define UTEST_SIZE_OF_TEST_FILES 6

TEST_GROUP(GroundCommanderTestGroup)
{
    Net2Com* netman;

    void setup()
    {
        mkdir(CS1_TGZ, S_IRWXU);
        mkdir(CS1_LOGS, S_IRWXU);

        pid_t pid = fork();

        if (pid == 0) {
            if(execl("./"SPACE_COMMANDER_BIN, SPACE_COMMANDER_BIN, NULL) == -1){
                fprintf(stderr, "[ERROR] %s:%s:%d ", __FILE__, __func__, __LINE__);
                exit(EXIT_FAILURE);
            }
        }

        // Make sure the commander is running
        while (system("ps aux | grep bin/space-commander/space-commander 1>/dev/null") != 0){
           usleep(1000); 
        }

        netman = Net2Com::create_netman();
        memset(command_buf, '\0', CMD_BUF_SIZE);
    }
    
    void teardown()
    {
#ifndef PRESERVE
        DeleteDirectoryContent(CS1_TGZ);
        rmdir(CS1_TGZ);

        DeleteDirectoryContent(CS1_LOGS);
        rmdir(CS1_LOGS);
#endif

        if (system("pidof space-commander | xargs  kill -15") != 0) {
            fprintf(stderr, "[ERROR] pidof space-commander | xargs -15 kill");
        }

        DeleteDirectoryContent(CS1_PIPES);

        if (netman) {
            delete netman;
            netman = NULL;
        }
    }
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * GROUP :  GroundCommanderTestGroup
 * NAME :   ReadCommand_Success 
 * DESC :   This test will make sure commands are read successfully from the 
 *          Command Input File, and that the commands are validated
 *
 *-----------------------------------------------------------------------------*/
TEST(GroundCommanderTestGroup, Read_Command_Success) 
{
    // - write a command buffer to the Command Input File
    // - run read_command
    // - check if data pipe was written to, and validate the contents of the 
    //   command that was written
    // - check that the command was removed from the Command Input File
    // - verfiy data_bytes_written return result
}

TEST(GroundCommanderTestGroup, Delete_Command_Success) 
{
    // - write a command buffer to the Command Input File
    // - run delete_command
    // - check if the right number of bytes was removed from the command 
    //   input file
}

TEST(GroundCommanderTestGroup, GetResultData_Success) 
{
    // - provide sample result buffers for all available commands
    // - write place each command in the Dnet-w-com-r pipe
    // - read the result buffer
    // - run ParseResult and validate all sample data
}

TEST(GroundCommanderTestGroup, Perform_Success) 
{
    // - test all switch cases
    // - test all conditional statements
    // - test null pointers
}

// Other tests
//
// Integration test:
//
// For each command (once API is available):
// call GetCmdStr to create a command buffer, put it in the Command Input Pipe, let it be parsed, 
// send it through the mock_sat netman system, run the command, return and test the result buffer 
// (ParseResult)
//
