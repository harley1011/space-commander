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
#include <sys/types.h>
#include <sys/wait.h>


//************************************************************
//************************************************************
//              Net2ComTestGroup
//************************************************************
//************************************************************
TEST_GROUP(Net2ComTestGroup){
    Net2Com* netman;
    Net2Com* commander;
    static const int BUFFER_SIZE = 50;
    static const int NULL_CHAR_LENGTH = 1;
    void setup(){
        netman = new Net2Com(Dnet_w_com_r, Dcom_w_net_r,  Inet_w_com_r, Icom_w_net_r);
        commander = new Net2Com(Dcom_w_net_r, Dnet_w_com_r, Icom_w_net_r, Inet_w_com_r);
    }
    void teardown(){
        if (netman != NULL){
            delete netman;
            netman = NULL;
        }

        if (commander != NULL){
            delete commander;
            commander = NULL;
        }
    }
};
TEST(Net2ComTestGroup, WriteToDataPipe_ReturnsCorrectNumberOfBytesWritten){
    char buffer[BUFFER_SIZE];
    const char* data = "SomeUsefullData";
    int result;
    pid_t pid;
    pid = fork();
    if (pid == 0){                                          // Child
        result = netman->WriteToDataPipe(data);
        #ifdef DEBUG
        printf("strlen(data) : %d, result : %d\n", strlen(data) + NULL_CHAR_LENGTH, result);
        #endif
        CHECK_EQUAL(strlen(data) + NULL_CHAR_LENGTH, result);

    }else{                                                  // Parent
        commander->ReadFromDataPipe(buffer, BUFFER_SIZE);
        #ifdef DEBUG
        printf("data : %s, buffer : %s\n", data, buffer);
        #endif
        STRCMP_EQUAL(data, buffer);
        exit(0);
    }
}
TEST(Net2ComTestGroup, ReadFromDataPipe_ReturnsCorrectString){
    char buffer[BUFFER_SIZE];
    const char* data = "myCommand";
    int result;
    pid_t pid = fork();
    if (pid == 0){
        result = commander->WriteToDataPipe(data);
        CHECK_EQUAL(strlen(data) + NULL_CHAR_LENGTH, result);
        exit(0);
    }else{
        netman->ReadFromDataPipe(buffer, BUFFER_SIZE);
        STRCMP_EQUAL(data, buffer);
    }
}
TEST(Net2ComTestGroup, WriteToInfoPipe_ReturnsCorrectNumberOfBytesWritten){
    char buffer[BUFFER_SIZE];
    const unsigned char byte = 252;
    int result;
    pid_t pid = fork();

    if (pid == 0){
        result = netman->WriteToInfoPipe(&byte, 1);
        CHECK_EQUAL(1, result);
    }else{
        commander->ReadFromInfoPipe(buffer, BUFFER_SIZE);
        CHECK_EQUAL(byte, (unsigned char)buffer[0]);
        exit(0);
    }
}
TEST(Net2ComTestGroup, ReadFromInfoPipe_ReturnsCorrectByte){
    char buffer[BUFFER_SIZE];
    const unsigned char byte = 252;
    int result;
    pid_t pid = fork();

    if (pid == 0){
        result = commander->WriteToInfoPipe(&byte, 1);
        CHECK_EQUAL(1, result);
        exit(0);
    }else{
        netman->ReadFromInfoPipe(buffer, BUFFER_SIZE);
        CHECK_EQUAL(byte, (unsigned char)buffer[0]);
    }
}

TEST(Net2ComTestGroup, ReadFromInfoPipe_persist_open_ReturnsCorrectByte){
    char buffer[BUFFER_SIZE];
    const unsigned char byte = 252;
    int result = 0;
    pid_t pid = fork();


    if (pid == 0){
        commander->OpenWritePipesPersistently();
        result = commander->WriteToInfoPipe(&byte, 1);
        CHECK_EQUAL(1, result);
        exit(0);
    }else{
        netman->OpenReadPipesPersistently();

        int bytes_read = 0;
        
        while (bytes_read == 0){
            sleep(1);
            bytes_read = netman->ReadFromInfoPipe(buffer, BUFFER_SIZE);
        }

        CHECK_EQUAL(bytes_read, 1);
        CHECK_EQUAL(byte, (unsigned char)buffer[0]);
    }

}

TEST(Net2ComTestGroup, ReadFromInfoPipe_TryOverFlowBuffer_ReadUntilBufferIsFull){
    const int TO_SMALL = 5;
    char buffer[TO_SMALL];
    const char* data = "myCommand";
    int result;
    pid_t pid = fork();
    if (pid == 0){
        result = commander->WriteToDataPipe(data);
        CHECK_EQUAL(strlen(data) + NULL_CHAR_LENGTH, result);
        exit(0);
    }else{
        int bytes_read = netman->ReadFromDataPipe(buffer, TO_SMALL);
        STRCMP_EQUAL("myCom", buffer);
        CHECK_EQUAL(TO_SMALL, bytes_read);
    }

}
