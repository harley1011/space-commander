//******************************************/
//  SPACE CONCORDIA 2013
//
//
//******************************************/
#include "CppUTest/TestHarness.h"
#include <Net2Com.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>


//************************************************************
//************************************************************
//              Net2ComTestGroup
//************************************************************
//************************************************************
TEST_GROUP(Net2ComTestGroup){
    Net2Com* netman;
    Net2Com* commander;
    void setup(){
        netman = new Net2Com(PIPE_ONE, PIPE_TWO, PIPE_THREE, PIPE_FOUR);
        commander = new Net2Com(PIPE_TWO, PIPE_ONE, PIPE_FOUR, PIPE_THREE);
    }
    void teardown(){
        delete netman;
        delete commander;
    }

};
TEST(Net2ComTestGroup, WriteToDataPipe_ReturnsCorrectNumberOfBytesWritten){
    int BUFFER_SIZE = 50;
    char buffer[BUFFER_SIZE];
    const char* data = "SomeUsefullData";
    int result;
    pid_t pid;
    pid = fork();
    if (pid == 0){                                          // Child
        commander->ReadFromDataPipe(buffer);
        //printf("data : %s, buffer : %s\n", data, buffer);
        STRCMP_EQUAL(data, buffer);
        exit(0);
    }else{                                                  // Parent
        result = netman->WriteToDataPipe(data);
        //printf("strlen(data) : %d, result : %d\n", strlen(data), result);
        CHECK_EQUAL(strlen(data), result);
    }
}
TEST(Net2ComTestGroup, ReadFromDataPipe_ReturnsCorrectString){
    FAIL("TODO");
}
TEST(Net2ComTestGroup, WriteToInfoPipe_ReturnsCorrectNumberOfBytesWritten){
    FAIL("TODO");
}
TEST(Net2ComTestGroup, ReadFromInfoPipe_ReturnsCorrectByte){
    FAIL("TODO");
}
