// using GoogleTests
#include "gtest/gtest.h"
#include <command-factory.h>
#include <Net2Com.h>
#include <fileIO.h>

const char* testfiles = "../stubs"; // sample log files, don't delete!
const char* logs = "../test-logs";

class commands_test : public ::testing::Test {
	protected:
	virtual void SetUp() {
    mkdir(logs, 0775);
    CopyDirRecursively(testfiles, logs);
  }

  virtual void TearDown() {
    DeleteDirectoryContent(logs);
    remove(logs);
  }
  char* set_time = "0";
  char* upload = "2";
  char* get_log = "3";
  char* reboot  = "4";
  char* decode = "6";
};

//
TEST_F(commands_test, getTimeCommand) {
  char* get_time = "1";
  ICommand* command = CommandFactory::CreateCommand(get_time);
  bool command_error = false;
  if(command == NULL)
    command_error = true;
  ASSERT_FALSE(command_error);
  char* result  = (char* )command->Execute();
  ASSERT_STRNE(result, NULL);
}


TEST_F(commands_test, getLogCommand) {
  ICommand* command = CommandFactory::CreateCommand(get_log);
  bool command_error = false;
  if(command == NULL)
    command_error = true;
  ASSERT_FALSE(command_error);
  char* result  = (char* )command->Execute();
  ASSERT_STRNE(result, NULL);
}
