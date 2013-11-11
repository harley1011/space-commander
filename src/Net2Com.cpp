#include <Net2Com.h>
#include <NamedPipe.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

const char* Net2Com::pipe_str[] = {"pipe1", "pipe2", "pipe3", "pipe4"};
//----------------------------------------------
//  Constructor
//----------------------------------------------
Net2Com::Net2Com(PIPE_NUM dataw, PIPE_NUM datar, PIPE_NUM infow, PIPE_NUM infor){
    Initialize();
    CreatePipes(); 
    
    dataPipe_w = pipe[dataw];
    dataPipe_r = pipe[datar];
    infoPipe_w = pipe[infow];
    infoPipe_r = pipe[infor];
}
//----------------------------------------------
//  Destructor
//----------------------------------------------
Net2Com::~Net2Com(){
    for (int i=0; i<NUMBER_OF_PIPES; i++){
        delete pipe[i];
    }
}
//----------------------------------------------
//  Initialize
//----------------------------------------------
bool Net2Com::Initialize(){
    for (int i=0; i<NUMBER_OF_PIPES; i++){
        pipe[i] = new NamedPipe(pipe_str[i]); 
    }

    return true;
}
//----------------------------------------------
//  CreatePipes
//----------------------------------------------
bool Net2Com::CreatePipes(){
    for (int i=0; i<NUMBER_OF_PIPES; i++){
        if (pipe[i]->Exist() == false){
            pipe[i]->CreatePipe();   
        }
    }

    return true;
}

//----------------------------------------------
//  OpenReadPipesPersistently 
//----------------------------------------------
void Net2Com::OpenReadPipesPersistently(){
    infoPipe_r->persist_open('r');
    dataPipe_r->persist_open('r');
}
//----------------------------------------------
//  OpenWritePipesPersistently 
//----------------------------------------------
void Net2Com::OpenWritePipesPersistently(){
    infoPipe_w->persist_open('w');
    dataPipe_w->persist_open('w');
}
//----------------------------------------------
// WriteToDataPipe 
//----------------------------------------------
int Net2Com::WriteToDataPipe(const char* str){
    int result = dataPipe_w->WriteToPipe(str, strlen(str) + NULL_CHAR_LENGTH);
    return result;
}
int Net2Com::WriteToDataPipe(const void* data, int size){
    int result = dataPipe_w->WriteToPipe(data, size);
    return result;
}
int Net2Com::WriteToDataPipe(unsigned char number){
    unsigned char byte = number;
    return dataPipe_w->WriteToPipe(&byte, sizeof(unsigned char));
}
//----------------------------------------------
// ReadFromDataPipe 
//----------------------------------------------
char* Net2Com::ReadFromDataPipe(char* buffer){
    dataPipe_r->ReadFromPipe(buffer);
    return buffer;
}
//----------------------------------------------
// WriteToInfoPipe 
//----------------------------------------------
int Net2Com::WriteToInfoPipe(const char* str){
    int result = infoPipe_w->WriteToPipe(str, strlen(str) + NULL_CHAR_LENGTH);
    return result;
}
int Net2Com::WriteToInfoPipe(const void* data, int size){
    int result = infoPipe_w->WriteToPipe(data, size);
    return result;
}
int Net2Com::WriteToInfoPipe(unsigned char number){
    unsigned char byte = number;
    return infoPipe_w->WriteToPipe(&byte, sizeof(unsigned char));
}
//----------------------------------------------
//  ReadFromInfoPipe
//----------------------------------------------
char* Net2Com::ReadFromInfoPipe(char* buffer){
    infoPipe_r->ReadFromPipe(buffer);
    return buffer;
}
