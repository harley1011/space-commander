#include <Net2Com.h>
#include <NamedPipe.h>
#include <cstdlib>
#include <cstdio>


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
// WriteToDataPipe 
//----------------------------------------------
int Net2Com::WriteToDataPipe(const char* str){
    int result = dataPipe_w->WriteToPipe(str);
    return result;
}
int Net2Com::WriteToDataPipe(unsigned char number){
    char byte[] = {number, '\0'};
    return WriteToDataPipe(byte);
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
    int result = infoPipe_w->WriteToPipe(str);
    return result;
}
int Net2Com::WriteToInfoPipe(unsigned char number){
    char byte[] = {number, '\0'};
    return WriteToInfoPipe(byte);
}
//----------------------------------------------
//  ReadFromInfoPipe
//----------------------------------------------
char* Net2Com::ReadFromInfoPipe(char* buffer){
    infoPipe_r->ReadFromPipe(buffer);
    return buffer;
}
