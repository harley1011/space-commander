#include <Net2Com.h>
#include <NamedPipe.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#ifdef PC
const char* Net2Com::pipe_str[] = {"Dnet-w-com-r", "Dcom-w-net-r", "Inet-w-com-r", "Icom-w-net-r"};
#else   
const char* Net2Com::pipe_str[] = {"/tmp/Net2Com/pipes/Dnet-w-com-r", "/tmp/Net2Com/pipes/Dcom-w-net-r", "/tmp/Net2Com/pipes/Inet-w-com-r", "/tmp/Net2Com/pipes/Icom-w-net-r"}; // Q6 paths
#endif
//----------------------------------------------
//  Constructor
//----------------------------------------------
Net2Com::Net2Com(pipe_num_t dataw, pipe_num_t datar, pipe_num_t infow, pipe_num_t infor){
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
int Net2Com::ReadFromDataPipe(char* buffer, int buf_size){
    return dataPipe_r->ReadFromPipe(buffer, buf_size);
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
int Net2Com::ReadFromInfoPipe(char* buffer, int buf_size){
    return infoPipe_r->ReadFromPipe(buffer, buf_size);
}
