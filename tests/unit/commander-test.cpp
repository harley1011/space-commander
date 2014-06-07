/******************************************************************************
*
* AUTHOR :
*
* FILE : commander-test.cpp
*
* PURPOSE :
*
* CREATION DATE : 06-06-2014
*
* LAST MODIFIED : Fri 06 Jun 2014 10:21:18 PM EDT
*
******************************************************************************/
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"

#include "SpaceDecl.h"
#include "command-factory.h"
#include "icommand.h"
#include "fileIO.h"
#include "SpaceString.h"

static char command_buf[GETLOG_CMD_SIZE] = {'\0'};

TEST_GROUP(CommanderTestGroup)
{
    void setup()
    {
        mkdir(CS1_TGZ, S_IRWXU);
        mkdir(CS1_LOGS, S_IRWXU);
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

TEST(CommanderTestGroup, DeleteLog_Success) 
{

}
