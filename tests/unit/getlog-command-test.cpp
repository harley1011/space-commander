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
TEST(GetLogTestGroup, justTryingStuff){
    char data[] = "30100";
    
    ICommand* command = CommandFactory::GetLogCommand(data);
    char* result = (char*)command->Execute();

}
