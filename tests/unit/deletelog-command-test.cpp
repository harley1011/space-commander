#include <string.h>
#include <sys/stat.h>
#include <unistd.h>     // rmdir()

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"

#include "SpaceDecl.h"
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
* NAME : DeleteLogTestGroup :: Execute_FileIsDeleted
* 
* PURPOSE : Successfully delete a file. 
*
*-----------------------------------------------------------------------------*/
TEST(DeleteLogTestGroup, Execute_FileIsDeleted){
    FILE* filetest = fopen(CS1_LOGS"/filetest.log", "w+");
    fprintf(filetest, "some text to test");
    fclose(filetest);

    char data[] = "7filetest.log";
    
    ICommand* command = CommandFactory::CreateCommand(data);
    char* result = (char*)command->Execute();

    printf("%s\n", result);

    char status[2] = {'\0'};
    strncpy(status, result, 1);

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

    #ifdef DEBUG
    printf("result : %d\n", result);
    #endif
    
    CHECK_EQUAL(0, result);

    if (command != NULL){
        delete command;
        command = NULL;
    }
}
