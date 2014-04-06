#include <string.h>
#include <sys/stat.h>
#include <unistd.h>     // rmdir()
#include "SpaceDecl.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "command-factory.h"
#include "icommand.h"

#define free(a) cpputest_free_location(a, __FILE__, __LINE__)

TEST_GROUP(DeleteLogTestGroup){
    void setup(){

    }
    void teardown(){

    }
};


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : DeleteLogTestGroup :: DeleteLog_Success
* 
* PURPOSE : Successfully delete a file. 
*
*-----------------------------------------------------------------------------*/
TEST(DeleteLogTestGroup, DeleteLog_Success){
    FILE* filetest = fopen(CS1_LOGS"/filetest.log", "w+");
    fprintf(filetest, "some text to test");
    fclose(filetest);

    char data[] = "7filetest.log";
    
    ICommand* command = CommandFactory::CreateCommand(data);
    char* result = (char*)command->Execute();

    printf("%s\n", result);

    char status[2] = {'\0'};
    strncpy(status, result, 1);

    CHECK_EQUAL(1, atoi(status));

    if (result != NULL){
        free(result);
        result = NULL;
    }

    if (command != NULL){
        delete command;
        command = NULL;
    }

}
