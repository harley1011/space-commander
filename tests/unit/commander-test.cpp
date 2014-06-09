/******************************************************************************
*
* AUTHOR :
*
* FILE : commander-test.cpp
*
* PURPOSE :
*
* CREATION DATE : 06-06-2014
*
* LAST MODIFIED : Sun 08 Jun 2014 11:46:44 PM EDT
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
#include "command-factory.h"
#include "icommand.h"
#include "fileIO.h"
#include "SpaceString.h"
#include "Net2Com.h"

#define RESULT_BUF_SIZE 50
#define CMD_BUF_SIZE 25
static char command_buf[CMD_BUF_SIZE] = {'\0'};
#define SPACE_COMMANDER_BIN  "bin/space-commander" // use local bin, not the one unser CS1_APPS

TEST_GROUP(CommanderTestGroup)
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
        while (system("ps aux | grep bin/space-commander 1>/dev/null") != 0){
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

TEST(CommanderTestGroup, DeleteLog_Success) 
{
    char result[RESULT_BUF_SIZE] = {0};
    // Creates the file
    char inode_str[4] = {'\0'};
    const char* filetest_path = CS1_TGZ"/filetest.tgz";
    FILE* filetest = fopen(filetest_path, "w+");
    fprintf(filetest, "some text to test");
    fclose(filetest);

    ino_t inode = GetLogCommand::GetInoT(filetest_path); 
    SpaceString::get4Char(inode_str, inode);

    // Check that the inode is good.
    ino_t inode_check = SpaceString::getUInt(inode_str);
    CHECK_EQUAL((unsigned int)inode, (unsigned int)inode_check);

    snprintf(command_buf, 3, "7I");  // 7 is the command number, I means inode
    memcpy(command_buf + 2, inode_str, 4);

    #ifdef DEBUG
        fprintf(stderr, "[DEBUG] %s:%s:%d filetest_path is : %s\n", __FILE__, __func__, __LINE__, filetest_path);
        fprintf(stderr, "[DEBUG] %s:%s:%d inode is : %d\n", __FILE__, __func__, __LINE__, (unsigned int)inode);
    #endif

    // use Netman Net2Com to send data to space-commander Net2Com
    netman->WriteToInfoPipe((unsigned char)CMD_BUF_SIZE);
    netman->WriteToDataPipe(command_buf, CMD_BUF_SIZE);
    netman->WriteToInfoPipe((unsigned char)0xFF);
    netman->WriteToInfoPipe((unsigned char)0x01);
    netman->WriteToDataPipe((unsigned char)0x21);
    netman->WriteToInfoPipe((unsigned char)0xFF);


    while (netman->ReadFromDataPipe(result, RESULT_BUF_SIZE) == 0) {
        // Give enough time to the commander to proceed!
        usleep(1000);
    }

    // Checks that the file has been deleted.
    CHECK_EQUAL(-1, access(filetest_path, F_OK));

    char status[2] = {'\0'};
    strncpy(status, result, 1);
    CHECK_EQUAL(0, atoi(status));
}
