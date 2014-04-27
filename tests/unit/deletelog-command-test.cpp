#include <string.h>
#include <sys/stat.h>
#include <unistd.h>     // rmdir()

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"

#include "SpaceDecl.h"
#include "command-factory.h"
#include "icommand.h"
#include "fileIO.h"

TEST_GROUP(DeleteLogTestGroup){
    void setup(){
        mkdir(CS1_TGZ, S_IRWXU);
        mkdir(CS1_LOGS, S_IRWXU);
    }
    void teardown(){
        DeleteDirectoryContent(CS1_TGZ);
        rmdir(CS1_TGZ);

        DeleteDirectoryContent(CS1_LOGS);
        rmdir(CS1_LOGS);
    }
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : DeleteLogTestGroup :: SendDeleteCommandUsingThePipes_FileIsDeleted
* 
* PURPOSE : 
*
*-----------------------------------------------------------------------------*/
TEST(DeleteLogTestGroup, SendDeleteCommandUsingThePipes_FileIsDeleted){
    FAIL("TODO : SendDeleteCommandUsingThePipes_FileIsDeleted");    
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : DeleteLogTestGroup :: Execute_FileIsDeleted
* 
* PURPOSE : Successfully delete a file. 
*
*-----------------------------------------------------------------------------*/
TEST(DeleteLogTestGroup, Execute_FileIsDeleted){
    const char* filetest_path = CS1_LOGS"/filetest.log";
    FILE* filetest = fopen(filetest_path, "w+");
    fprintf(filetest, "some text to test");
    fclose(filetest);

    char data[] = "7filetest.log";
    
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
TEST(DeleteLogTestGroup, FindType_ReturnsLOG){
    char data[] = "7filetest.log";      // Command number followed by the filename

    DeleteLogCommand* command = new DeleteLogCommand(&data[1]);

    int result = command->FindType();

    CHECK_EQUAL(0, result);

    if (command != NULL){
        delete command;
        command = NULL;
    }
}
