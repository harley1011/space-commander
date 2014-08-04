#include <string.h>
#include <sys/stat.h>
#include <unistd.h>     // rmdir()
#include <sys/types.h>

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"

#include "SpaceDecl.h"
#include "command-factory.h"
#include "icommand.h"
#include "fileIO.h"
#include "SpaceString.h"

#define PADDING 10
static char command_buf[GETLOG_CMD_SIZE + PADDING] = {'\0'};

TEST_GROUP(DeleteLogTestGroup)
{
    void setup()
    {
        mkdir(CS1_TGZ, S_IRWXU);
        mkdir(CS1_LOGS, S_IRWXU);

        memset(command_buf, '\0', GETLOG_CMD_SIZE);
    }
    
    void teardown()
    {
#ifndef PRESERVE
        DeleteDirectoryContent(CS1_TGZ);
        rmdir(CS1_TGZ);

        DeleteDirectoryContent(CS1_LOGS);
        rmdir(CS1_LOGS);
#endif
    }
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : DeleteLogTestGroup 
*
* NAME : DeleteLog_UsingInode_fileIsDeleted
* 
*-----------------------------------------------------------------------------*/
TEST(DeleteLogTestGroup, DeleteLog_UsingInode_fileIsDeleted)
{
    // Creates the file
    char inode_str[5] = {'\0'};
    const char* filetest_path = CS1_TGZ"/filetest.tgz";
    FILE* filetest = fopen(filetest_path, "w+");
    fprintf(filetest, "some text to test");
    fclose(filetest);

    ino_t inode = GetLogCommand::GetInoT(filetest_path); 
    SpaceString::get4Char(inode_str, inode);


    ino_t inode_check = SpaceString::getUInt(inode_str);
    CHECK_EQUAL((unsigned int)inode, (unsigned int)inode_check);

    snprintf(command_buf, 3, "7I");  // 7 is the command number, I means inode
    memcpy(command_buf + 2, inode_str, 4);

    #ifdef CS1_DEBUG
        fprintf(stderr, "[DEBUG] %s:%s:%d filetest_path is : %s\n", __FILE__, __func__, __LINE__, filetest_path);
        fprintf(stderr, "[DEBUG] %s:%s:%d inode is : %d\n", __FILE__, __func__, __LINE__, (unsigned int)inode);
    #endif

    ICommand* command = CommandFactory::CreateCommand(command_buf);
    char* result = (char*)command->Execute();

    CHECK_EQUAL(-1, access(filetest_path, F_OK));

    char status[2] = {'\0'};
    strncpy(status, result, 1);
    CHECK_EQUAL(0, atoi(status));

    if (result) {
        free(result);
        result = 0;
    }

    if (command != NULL){
        delete command;
        command = NULL;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : DeleteLogTestGroup 
*
* NAME  : Execute_FileIsDeleted
* 
*-----------------------------------------------------------------------------*/
TEST(DeleteLogTestGroup, Execute_FileIsDeleted)
{
    const char* filetest_path = CS1_LOGS"/filetest.log";
    FILE* filetest = fopen(filetest_path, "w+");
    fprintf(filetest, "some text to test");
    fclose(filetest);

    char data[] = "7_filetest.log";
    
    ICommand* command = CommandFactory::CreateCommand(data);
    char* result = (char*)command->Execute();

    char status[2] = {'\0'};
    strncpy(status, result, 1);

    CHECK_EQUAL(-1, access(filetest_path, F_OK));
    CHECK_EQUAL(0, atoi(status));

    if (result != NULL){
        free(result);
        result = NULL;
    }

    if (command != NULL) {
        delete command;
        command = NULL;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : DeleteLogTestGroup :: FindType_ReturnsTGZ
* 
* PURPOSE : Successfully determines the type of the file 
*
*-----------------------------------------------------------------------------*/
TEST(DeleteLogTestGroup, FindType_ReturnsTGZ)
{
    char data[] = "7filetest.tgz";      // Command number followed by the filename

    DeleteLogCommand* command = new DeleteLogCommand(&data[1]);

    int result = command->FindType();

    CHECK_EQUAL(0x1, result);

    if (command != NULL){
        delete command;
        command = NULL;
    }
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : DeleteLogTestGroup :: FindType_ReturnsLOG
* 
* PURPOSE : Successfully determines the type of the file 
*
*-----------------------------------------------------------------------------*/
TEST(DeleteLogTestGroup, FindType_ReturnsLOG)
{
    char data[] = "7filetest.log";      // Command number followed by the filename

    DeleteLogCommand* command = new DeleteLogCommand(&data[1]);

    int result = command->FindType();

    CHECK_EQUAL(0, result);

    if (command != NULL){
        delete command;
        command = NULL;
    }
}
