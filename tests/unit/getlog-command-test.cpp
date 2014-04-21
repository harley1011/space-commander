/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* AUTHORS : Space Concordia 2014, Joseph
*
* TITLE : getlog-command-test.cpp 
*
* DESCRIPTION : Tests the GetLogCommand
*
*----------------------------------------------------------------------------*/
#include <time.h>
#include <unistd.h>
#include <dirent.h>     // DIR
#include <sys/stat.h>

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"

#include "SpaceDecl.h"
#include "command-factory.h"
#include "getlog-command.h"
#include "icommand.h"
#include "fileIO.h"


#define UNIT_CS1_TGZ "./unit_tgz"

TEST_GROUP(GetLogTestGroup){
    void setup(){
        mkdir(UNIT_CS1_TGZ, S_IRWXU);

    }
    void teardown(){
        DeleteDirectoryContent(UNIT_CS1_TGZ);
        rmdir(UNIT_CS1_TGZ);
    }
};

void create_file(const char* path);
void create_file(const char* path, const char* msg){
    FILE* file = fopen(path, "w+");
    fprintf(file, "%s", msg);
    fclose(file);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetLogTestGroup  :: FindOldestFile_returnsTheCorrectFilename
* 
*-----------------------------------------------------------------------------*/
TEST(GetLogTestGroup, FindOldestFile_returnsTheCorrectFilename){
    create_file(UNIT_CS1_TGZ"/a.txt", "file a");
    usleep(1000000);
    create_file(UNIT_CS1_TGZ"/b.txt", "file b");
    usleep(1000000);
    create_file(UNIT_CS1_TGZ"/c.txt", "file c");
    usleep(5000);

    char* oldestFile = GetLogCommand::FindOldestFile(UNIT_CS1_TGZ);

    STRCMP_EQUAL("a.txt", oldestFile);

    if (oldestFile != 0){
        free(oldestFile);
        oldestFile = 0;
    }
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetLogTestGroup  :: GetFileLastModifTimeT_returnsCorrectTimeT
* 
*-----------------------------------------------------------------------------*/
TEST(GetLogTestGroup, Execute_OPT_NOOPT_returnsOldestTgz){
    char data[11] = {'\0'};
    data[0] = 0x33;
    ICommand *command = CommandFactory::CreateCommand(data);

    command->Execute();
    FAIL("Refactoring of GetLogCommand in progress...TODO");

    if (command != NULL){
        delete command;
        command = NULL;
    }
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetLogTestGroup  :: GetFileLastModifTimeT_returnsCorrectTimeT
* 
*-----------------------------------------------------------------------------*/
TEST(GetLogTestGroup, GetFileLastModifTimeT_returnsCorrectTimeT){
    const char* path = "aFile.txt";
    FILE* file = fopen(path, "w+");

    if (file != NULL){
        fclose(file);
        time_t fileCreationDate = GetLogCommand::GetFileLastModifTimeT(path);
        struct tm* timeinfo = localtime(&fileCreationDate);

        time_t currentTime = time(NULL);
        struct tm* current_tm = localtime(&currentTime);

        CHECK_EQUAL(current_tm->tm_year, timeinfo->tm_year);
        CHECK_EQUAL(current_tm->tm_mon, timeinfo->tm_mon);
        CHECK_EQUAL(current_tm->tm_mday, timeinfo->tm_mday);
        CHECK_EQUAL(current_tm->tm_hour, timeinfo->tm_hour);
        CHECK_EQUAL(current_tm->tm_min, timeinfo->tm_min);

        remove(path);
    }else{
        FAIL("Couldn't open the file");
    }
}
