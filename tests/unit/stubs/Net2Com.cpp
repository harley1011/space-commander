#include <Net2Com.h>
#include <NamedPipe.h>
#include <cstdlib>
#include <cstdio>
//----------------------------------------------
//  Constructor
//----------------------------------------------
Net2Com::Net2Com(){
    const char* p1 = "pipe1"; 
    const char* p2 = "pipe2";
    const char* p3 = "pipe3";
    const char* p4 = "pipe4";
    Initialize(p1, p2, p3, p4);
}
Net2Com::Net2Com(const char* p1, const char* p2, const char* p3, const char* p4){
    Initialize(p1, p2, p3, p4);
}
//----------------------------------------------
//  Destructor
//----------------------------------------------
Net2Com::~Net2Com(){
   //*/ 
}
//----------------------------------------------
//  CreatePipesStub
//----------------------------------------------
bool Net2Com::Initialize(const char* p1, const char* p2, const char* p3, const char* p4){
   pipe1 = new NamedPipe(p1); 

   pipe2 = new NamedPipe(p2); 
   
   pipe3 = new NamedPipe(p3); 
   
   pipe4 = new NamedPipe(p4); 

   return true;
}
