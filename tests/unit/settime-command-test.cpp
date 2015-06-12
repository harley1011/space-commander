/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * AUTHORS : Space Concordia 2014, Harley
 *
 * TITLE : settime-command-test.cpp 
 *
 * DESCRIPTION : Tests the SetTimeCommand class
 *
 * NOTES : TODO -> mock the system calls to avoid the need to be root to run
 *                 the tests.
 *         TODO -> hardware tests are not unit tests and should move somewhere
 *                 else.
 *
 *----------------------------------------------------------------------------*/
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "SpaceDecl.h"
#include "SpaceString.h"
#include "common/commands.h"
#include "common/command-factory.h"
#include "common/icommand.h"
#include "common/settime-command.h"
#include "common/subsystems.h"
#include "fileIO.h"
#include "dirUtl.h"

static const char* RTC_DEV_PATH = "/dev/rtc1";
static bool checkRoot();
static char command_buf[SETTIME_CMD_SIZE] = {'\0'};

TEST_GROUP(SetTimeTestGroup) {
    void setup() {
        // clears the command buffer
        memset(command_buf, 0, SETTIME_CMD_SIZE);
        command_buf[CMD_ID] = SETTIME_CMD;
    }

    void teardown() {
        //*/
    }
};


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * GROUP : SetTimeTestGroup
 *
 * NAME : Check_Settime
 * 
 *-----------------------------------------------------------------------------*/
TEST(SetTimeTestGroup, Check_Settime) {   
    if (!checkRoot()) {
        return;
    }

    time_t rawtime = 0;
    time_t newtime = 0;
    size_t result_size = 0;
    time(&rawtime);

    SpaceString::getTimetInChar(command_buf + CMD_HEAD_SIZE, rawtime);
    command_buf[SETTIME_CMD_SIZE - 1] = 0xFF;   
    ICommand* command = CommandFactory::CreateCommand(command_buf);

    char* result = (char*)command->Execute(&result_size);
    CHECK(result_size == SETTIME_RTN_SIZE + CMD_RES_HEAD_SIZE);

    InfoBytesSetTime* settime_info = (InfoBytesSetTime*)command->ParseResult(result);
    CHECK(settime_info->time_status == CS1_SUCCESS);
    CHECK(settime_info->time_set == rawtime);

    time(&newtime);
    CHECK(newtime-rawtime < 1);        

    #ifdef CS1_DEBUG
    std::cerr << "[DEBUG] " << __FILE__ << "Raw Seconds elapsed " 
              << rawtime << " time is currently " << newtime 
              << " difference is " << newtime - rawtime << endl;
    #endif    

    if (command != NULL) {
        delete command;
        command = NULL;
    }

    if (result) {
        free(result);
        result = 0;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * GROUP : SetTimeTestGroup
 *
 * NAME : Endian_Checker
 * 
 *-----------------------------------------------------------------------------*/
TEST(SetTimeTestGroup, Endian_Checker) { 
    short x=0x0100;
    char  temp = (*(char *)&x);
    CHECK(temp == LE);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * GROUP : SetTimeTestGroup
 *
 * NAME : SetTime_Parseresult
 * 
 *-----------------------------------------------------------------------------*/
TEST(SetTimeTestGroup, ParseResult_success) {    
    time_t rawtime = 0;
    time(&rawtime);
    ICommand* command = new SetTimeCommand(rawtime);

    // expected InfoBytesSetTime object
    InfoBytesSetTime expected;
    expected.setTime(rawtime);
    expected.setStatus(CS1_SUCCESS);
 
    // Execute the command
    char* result = (char*)malloc(sizeof(char) * SETTIME_RTN_SIZE_TOTAL);
    result[CMD_ID] = SETTIME_CMD;
    result[CMD_STS] = CS1_SUCCESS;
    memcpy(result + 2,&rawtime, sizeof(time_t));

    // Parse the result buffer
    InfoBytesSetTime* settime_info = (InfoBytesSetTime*)command->ParseResult(result);

    // Assertions
    CHECK_EQUAL(expected.getTime(), settime_info->getTime());
    CHECK_EQUAL(expected.getStatus(), settime_info->getStatus());

    if (result) {
        free(result);
        result = 0;
    }

    if (command != NULL) {
        delete command;
        command = NULL;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * GROUP : SetTimeTestGroup
 *
 * NAME : Check_Settime_Rtc
 * 
 *-----------------------------------------------------------------------------*/
TEST(SetTimeTestGroup, Check_Settime_Rtc) {
    if (!checkRoot()) {
        return;
    }

    std::ifstream ifs;
    ifs.open(RTC_DEV_PATH,std::ifstream::in);

    if (!ifs.good()) {
        printf("[WARNING] couldn't open %s... SKIPPING TEST\n", RTC_DEV_PATH);
        return;
    }

    ifs.close();
    time_t rawtime;
    size_t resultBufferSize;
    time(&rawtime);
    
    SpaceString::getTimetInChar(command_buf+1,rawtime);
    command_buf[SETTIME_CMD_SIZE - 1] = 0x01; // 0x01 -> RTC_BYTE ON      TODO !!! remove this magic number !!!
     
    ICommand* command = CommandFactory::CreateCommand(command_buf);
    char* result = (char*)command->Execute(&resultBufferSize);
    CHECK_EQUAL(SETTIME_RTN_SIZE_TOTAL, resultBufferSize);
    InfoBytesSetTime* settime_info = (InfoBytesSetTime*)command->ParseResult(result);


    CHECK(settime_info->time_status == CS1_SUCCESS);

    CHECK(settime_info->time_set == rawtime);
    time_t newtime;
    time(&newtime);
    CHECK(newtime-rawtime < 1);        

    #ifdef CS1_DEBUG
        std::cerr << "[DEBUG] " << __FILE__ << "Raw Seconds elapsed " 
                  << rawtime << " time is currently " << newtime 
                  << " difference is " << newtime - rawtime << endl;
    #endif    

    if ( command != NULL) {
        delete command;
        command = NULL;
    }

    if (result) {
        free(result);
        result = 0;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * GROUP : SetTimeTestGroup
 *
 * NAME : Check_Settime_Fail
 * 
 *-----------------------------------------------------------------------------*/
TEST(SetTimeTestGroup, Check_Settime_Fail) {   
    if (!checkRoot()) {
        return;
    }

    time_t rawtime = -1;
    size_t result_size; 
    
    SpaceString::getTimetInChar(command_buf+1,rawtime);
    command_buf[SETTIME_CMD_SIZE - 1] = 0xFF;   
    ICommand* command = CommandFactory::CreateCommand(command_buf);
    char* result = (char*)command->Execute(&result_size);
    
    CHECK(result_size == SETTIME_RTN_SIZE + CMD_RES_HEAD_SIZE);    

    InfoBytesSetTime* settime_info = (InfoBytesSetTime*)command->ParseResult(result);

    CHECK(settime_info->time_status == CS1_FAILURE);

    if ( command != NULL) {
        delete command;
        command = NULL;
    }
     
    if (result) {
        free(result);
        result = 0;
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * NAME : checkRoot
 *
 * PURPOSE : checks if the tests are run with ROOT.
 *
 *--------------------------------------------------------------------------------*/
bool checkRoot() {
    const int ROOT_UID = 0;
    bool result = true;

    if (getuid() != ROOT_UID) {
        printf("[WARNING] test needs root... SKIPPING TEST\n");
        result = false;
    }

    return result;
}
