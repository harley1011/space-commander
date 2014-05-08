#include "CppUTest/TestHarness.h"
#include <cstring>
#include <string>
#include "command-factory.h"
#include <iostream>
#include <cstdlib>
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"

using namespace std;

static const int BUFFER_SIZE = 10;
static char zeroOutBuffer[BUFFER_SIZE];

TEST_GROUP(CommandFactory) {
    void setup() {
        memset(zeroOutBuffer, 0, sizeof(char) * BUFFER_SIZE);
    }

    void teardown() {}
};

TEST(CommandFactory, ParseGetLog_MechSubSytem_Returns0) {
    char input[2] = {2, 0}; //First element represent type

    GetLogCommand* actual = (GetLogCommand*) CommandFactory::CreateCommand(input);
    GetLogCommand  expected(0);

    CHECK_EQUAL(expected.GetSubSystem(), actual->GetSubSystem());
    delete actual;
}

TEST(CommandFactory, ParseUpdate_MechSubSytem_Returns0) {
    char input[BUFFER_SIZE];
    input[0] = 1;
    input[1] = 4;
    input[2] = 't';
    input[3] = 'e';
    input[4] = 's';
    input[5] = 't';

    UpdateCommand* actual = (UpdateCommand* )CommandFactory::CreateCommand(input);
    string expected = "test";
    CHECK_EQUAL(expected, *actual->GetPath());
    delete actual;
}

TEST(CommandFactory, ParseSetTime_MaxDate_Returns0) {
    char input[5];
   
    input[0] = 0;

    /* http://www.epochconverter.com/  */
    /* Human time (GMT): Fri, 07 Jul 2017 15:00:00 GMT */
    unsigned int seconds = 1499439600;
    input[1] = (seconds >> 24) & 0xFF;
    input[2] = (seconds >> 16) & 0xFF;
    input[3] = (seconds >> 8)  & 0xFF;
    input[4] = seconds & 0xFF;

    SetTimeCommand* actual = (SetTimeCommand*) CommandFactory::CreateCommand(input);
    int  expectedYear   = 117;
    int  expectedMonth  = 6; // Month starts at 0
    int  expectedDay    = 7;
    int  expectedHour   = 15;
    int  expectedMinute = 0;
    int  expectedSecond = 0;

    CHECK_EQUAL(expectedYear,   actual->GetYearSince1900());
    CHECK_EQUAL(expectedMonth,  actual->GetMonth());
    CHECK_EQUAL(expectedDay,    actual->GetDay());
    CHECK_EQUAL(expectedHour,   actual->GetHour());
    CHECK_EQUAL(expectedMinute, actual->GetMinute());
    CHECK_EQUAL(expectedSecond, actual->GetSecond());
    delete actual;
}
