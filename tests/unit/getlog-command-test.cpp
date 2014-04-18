/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* AUTHORS : Space Concordia 2014, Joseph
*
* TITLE : getlog-command-test.cpp 
*
* DESCRIPTION : Tests the GetLogCommand
*
*----------------------------------------------------------------------------*/
#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"

#include "SpaceDecl.h"
#include "command-factory.h"
#include "icommand.h"


TEST_GROUP(GetLogTestGroup){
    void setup(){

    }
    void teardown(){

    }
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetLogTestGroup  :: 
* 
* PURPOSE : 
*
*-----------------------------------------------------------------------------*/
/*
TEST(GetLogTestGroup, justTryingStuff){
    FAIL("TODO");
    char data[] = "30100";
    
    ICommand* command = CommandFactory::CreateCommand(data);
   char* result = (char*)command->Execute();
}
*/
