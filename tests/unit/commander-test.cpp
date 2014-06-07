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
* LAST MODIFIED : Sat 07 Jun 2014 07:10:30 PM EDT
*
******************************************************************************/
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"

#include "SpaceDecl.h"
#include "command-factory.h"
#include "icommand.h"
#include "fileIO.h"
#include "SpaceString.h"
#include "Net2Com.h"

static char command_buf[GETLOG_CMD_SIZE] = {'\0'};
#define SPACE_COMMANDER_BIN  "bin/space-commander" // use local bin, not the one unser CS1_APPS

TEST_GROUP(CommanderTestGroup)
{

    Net2Com* netman;

    void setup()
    {
        mkdir(CS1_TGZ, S_IRWXU);
        mkdir(CS1_LOGS, S_IRWXU);
        if(system("./"SPACE_COMMANDER_BIN" &") != 0){
            fprintf(stderr, "[ERROR] %s:%s:%d ", __FILE__, __func__, __LINE__);
            exit(EXIT_FAILURE);
        }

        netman = Net2Com::create_netman();
    }
    
    void teardown()
    {
#ifndef PRESERVE
        DeleteDirectoryContent(CS1_TGZ);
        rmdir(CS1_TGZ);

        DeleteDirectoryContent(CS1_LOGS);
        rmdir(CS1_LOGS);

        if (system("pidof space-commander | xargs  kill -15") != 0) {
            fprintf(stderr, "[ERROR] pidof space-commander | xargs -15 kill");
        }
#endif
        if (netman) {
            delete netman;
            netman = NULL;
        }
    }
};

TEST(CommanderTestGroup, DeleteLog_Success) 
{

    char* result = 0;
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
        fprintf(stderr, "[DEBUG] %s:%s:%d command_buf is : %s\n", __FILE__,  __func__, __LINE__, command_buf);
    #endif

    // use Netman Net2Com to send data to space-commander Net2Com
        FAIL("TODO joseph write this command to the pipe!");
//    netman->WriteToInfoPipe(sizeOfCmd);
 //   netman->WriteToDataPipe(cmd);
//    netman->WriteToInfoPipe(0xFF);
//  echo -n -e \\x01 > Inet-w-com-r
//  echo -n -e \\x21 > Dnet-w-com-r
//  echo -n -e \\xFF > Inet-w-com-r


    // Checks that the file has been deleted.
    CHECK_EQUAL(-1, access(filetest_path, F_OK));

    char status[2] = {'\0'};
    strncpy(status, result, 1);
    CHECK_EQUAL(0, atoi(status));
}
