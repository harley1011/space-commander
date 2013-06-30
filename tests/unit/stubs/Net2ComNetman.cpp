#include <Net2ComNetman.h>
#include <cstdlib>
#include <cstdio>
//----------------------------------------------
//  Constructor/Destructor
//----------------------------------------------
Net2ComNetman::Net2ComNetman() : Net2Com(){
    CreatePipes();
}
Net2ComNetman::Net2ComNetman(const char* p1, const char* p2, const char* p3, const char* p4) : Net2Com(p1, p2, p3, p4){
    CreatePipes();
}
Net2ComNetman::~Net2ComNetman(){
    delete pipe1;
    delete pipe2;
    delete pipe3;
    delete pipe4;
}
//----------------------------------------------
//  CreatePipes
//----------------------------------------------
bool Net2ComNetman::CreatePipes(){
   
   if (pipe1->CreatePipe() == false){
        exit(-1);
   }

   if (pipe2->CreatePipe() == false){
        exit(-1);
   }

   if (pipe3->CreatePipe() == false){
        exit(-1);
   }

   if (pipe4->CreatePipe() == false){
        exit(-1);
   }

   return true;
}
//----------------------------------------------
// WriteToDataPipe 
//----------------------------------------------
int Net2ComNetman::WriteToDataPipe(const char* str){
    int result = pipe1->WriteToPipe(str);
    return result;
}
//----------------------------------------------
// ReadFromDataPipe 
//----------------------------------------------
char* Net2ComNetman::ReadFromDataPipe(char* buffer){
    pipe2->ReadFromPipe(buffer);
    return buffer;
}
//----------------------------------------------
// WriteToInfoComPipe 
//----------------------------------------------
int Net2ComNetman::WriteToInfoComPipe(const char* str){
    int result = pipe3->WriteToPipe(str);
    return result;
}
//----------------------------------------------
//  ReadFromInfoComPipe
//----------------------------------------------
char* Net2ComNetman::ReadFromInfoComPipe(char* buffer){
    pipe4->ReadFromPipe(buffer);
    return buffer;
}
