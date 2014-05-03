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
#include "commands.h"
#include "subsystems.h"

static char command_buf[11] = {'\0'};

TEST_GROUP(GetLogTestGroup)
{
    void setup(){
        mkdir(CS1_TGZ, S_IRWXU);

    }
    void teardown(){
        DeleteDirectoryContent(CS1_TGZ);
        rmdir(CS1_TGZ);
    }
};

void create_file(const char* path);
void create_file(const char* path, const char* msg)
{
    FILE* file = fopen(path, "w+");
    fprintf(file, "%s", msg);
    fclose(file);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : GetLogTestGroup
*
* NAME : GetNextFile_SUB_returnsOldestFilenameThatBelongsToSub 
* 
*-----------------------------------------------------------------------------*/
TEST(GetLogTestGroup, GetNextFile_SUB_returnsOldestFilenameThatBelongsToSub) 
{
    create_file(CS1_TGZ"/Watch-Puppy20140101.txt", "file a");
    usleep(1000000);
    create_file(CS1_TGZ"/Updater20140102.txt", "file b");
    usleep(1000000);
    create_file(CS1_TGZ"/Updater20140103.txt", "file c");
    usleep(5000);

    command_buf[0] = GETLOG_CMD;
    command_buf[1] = OPT_SUB;
    command_buf[2] = UPDATER;
    GetLogCommand *command = (GetLogCommand*)CommandFactory::CreateCommand(command_buf);

    char* oldest_file = command->GetNextFile();
    STRCMP_EQUAL("Updater20140102.txt", oldest_file);

    if (command != NULL){
        delete command;
        command = NULL;
    }

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : GetLogTestGroup
*
* NAME : GetNextFile_NOOPT_returnsOldestFilename 
* 
*-----------------------------------------------------------------------------*/
TEST(GetLogTestGroup, GetNextFile_NOOPT_returnsOldestFilename) 
{
    create_file(CS1_TGZ"/a.txt", "file a");
    usleep(1000000);
    create_file(CS1_TGZ"/b.txt", "file b");
    usleep(1000000);
    create_file(CS1_TGZ"/c.txt", "file c");
    usleep(5000);

    command_buf[0] = GETLOG_CMD;

    GetLogCommand *command = (GetLogCommand*)CommandFactory::CreateCommand(command_buf);

    char* oldest_file = command->GetNextFile();
    STRCMP_EQUAL("a.txt", oldest_file);

    if (command != NULL){
        delete command;
        command = NULL;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : GetLogTestGroup
*
* NAME : FindOldestFile_OPT_SUB_returnsCorrectFilename 
* 
*-----------------------------------------------------------------------------*/
TEST(GetLogTestGroup, FindOldestFile_OPT_SUB_returnsCorrectFilename) 
{
    create_file(CS1_TGZ"/SubA20140101.txt", "file a");
    usleep(1000000);
    create_file(CS1_TGZ"/SubA20140102.txt", "file b");
    usleep(1000000);
    create_file(CS1_TGZ"/SubC20140101.txt", "file c");
    usleep(5000);

    char* oldestFile = GetLogCommand::FindOldestFile(CS1_TGZ, "SubA");

    STRCMP_EQUAL("SubA20140101.txt", oldestFile);

    if (oldestFile != 0){
        free(oldestFile);
        oldestFile = 0;
    }
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetLogTestGroup  :: FindOldestFile_returnsTheCorrectFilename
* 
*-----------------------------------------------------------------------------*/
TEST(GetLogTestGroup, FindOldestFile_returnsTheCorrectFilename)
{
    create_file(CS1_TGZ"/a.txt", "file a");
    usleep(1000000);
    create_file(CS1_TGZ"/b.txt", "file b");
    usleep(1000000);
    create_file(CS1_TGZ"/c.txt", "file c");
    usleep(5000);

    char* oldestFile = GetLogCommand::FindOldestFile(CS1_TGZ, NULL);

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
TEST(GetLogTestGroup, Execute_OPT_NOOPT_returnsOldestTgz)
{
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
TEST(GetLogTestGroup, GetFileLastModifTimeT_returnsCorrectTimeT)
{
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

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : GetLogTestGroup
*
* NAME : GetPath_returnsCorrectPath
* 
*-----------------------------------------------------------------------------*/
TEST(GetLogTestGroup, GetPath_returnsCorrectPath) 
{
    const char* dir = "/my/directory";
    const char* file = "the_file.file";
    const char* expected = "/my/directory/the_file.file";

    char buffer[CS1_PATH_MAX];

    STRCMP_EQUAL(expected, GetLogCommand::GetPath(dir, file, buffer));
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : GetLogTestGroup
*
* NAME : prefixMatches_returnsTrue
* 
*-----------------------------------------------------------------------------*/
TEST(GetLogTestGroup, prefixMatches_returnsTrue) 
{
    const char* haystack = "StarMoonStorm";
    const char* needle1 = "";
    const char* needle2 = "Star";
    const char* needle3 = "NotThere";
    
    CHECK(GetLogCommand::prefixMatches(haystack, needle1));
    CHECK(GetLogCommand::prefixMatches(haystack, needle2));
    CHECK(GetLogCommand::prefixMatches(haystack, needle3) == 0);
}
