#include "getlog-command.h"
#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>   
#include <ctype.h>

const char ACS          = 0;
const char COMMS        = 1;
const char HARDWARE     = 2;
const char MECH         = 3;
const char PAYLOAD      = 4;
const char POWER        = 5;
const char SOFTWARE     = 6;
const char WATCH_PUPPY  = 7;

const size_t GetLogCommand::MAX_LENGTH = 140;bool prefixMatches(const char prefix[], const char filename[]){
    if( strncmp(prefix, filename, strlen(prefix)) == 0) {
	    return true;
    }
    else{
	    return false;
    }
}

bool suffixMatches(const char suffix[], const char filename[]){
    //2nd argument of strncmp is all about starting the comparaison at the end of the string minus the length of the suffix
    if ( strncmp(suffix, ( filename + strlen(filename) - strlen(suffix) ), strlen(suffix)) == 0){
	    return true;	
    }
    else{
	    return false;
    }
}

//returning -1 means the log number could not be parsed in the log name (there aren't two periods surrounding the log number, the log number contains letters or symbols, etc) 
int getLogNumber(const char filename[]){
	char buffer[256];
	const char* firstPeriod;
	const char* lastPeriod;

	if( (firstPeriod = strchr(filename, '.')) != NULL){

        if( (lastPeriod = strrchr(filename, '.' )) != NULL){
			if( (firstPeriod + 1) == lastPeriod){
				//printf("%d\n%d\n", firstPeriod, lastPeriod); 
				return -1; //there is nothing between the periods
			}

			//the memcpy part here is just copying the characters between the first period and the last period
			int offsetStart = firstPeriod - filename + 1; //calculating offset to find the character after the first period
			memcpy(buffer, filename + offsetStart, lastPeriod - firstPeriod - 1 );
			buffer[lastPeriod - firstPeriod - 1] = '\0';
			
			//couldn't find a string to integer parser that did quite this part; gotta make sure there aren't any non-digits
			for(int i = 0; i < (lastPeriod - firstPeriod - 1) ; i++){
				if(! (isdigit(buffer[i])) ){
					return -1;
				}
			}

			int logNumber;
			if( sscanf(buffer, "%d", &logNumber) == 1){
				if(logNumber > 0){ //negative logs don't make sense
					return logNumber;
				}
			}
		}
	}

    return -1;
}


//function returns status code
//function uses third argument to return the filename of the oldest log, unless there's an error
//char[] filename shall be 256 bytes, the maximum filesize in ext2, ext3 and ext4
int GetOldestLog(const char prefix[], const char directory[], char filename[])
{
	//filenames on ext2, ext3 and ext4 are limited to 255 bytes (characters) plus null-terminator
	if(strlen(prefix) > 255) {
		return 2;
	}

	//just in case the prefix is weird
	if(strlen(prefix) == 0) {
		return 3;
	}


	//let's check if the path exists
	struct stat stat_dir = {0};
	if(stat(directory,&stat_dir) == 0) {
        if(!(S_ISDIR(stat_dir.st_mode))) {
			//path exists but it's not a directory
			return 4;
        }
	}
    else {
		//path does not exist at all in the filesystem
		return 5;
    }

	DIR *dir = opendir(directory);
	struct dirent *element;	
	int smallestLogNumber = -1;
	char smallestLogFilename[256];
	int currentLogNumber;

	if(dir) {
		
		while( ( element = readdir(dir))  != NULL){
			
			if(! (prefixMatches(prefix,element->d_name) )){
				printf("Prefix didn't match for: %s\n", element->d_name);
			}
			else if(! (suffixMatches("log", element->d_name)) ){
				printf("Suffix didn't match: %s\n", element->d_name);
			}
			else if( (currentLogNumber = getLogNumber(element->d_name)) != -1 ){
				printf("%s\n", element->d_name);
				if(smallestLogNumber == -1 || currentLogNumber < smallestLogNumber){
					smallestLogNumber = currentLogNumber;
					
					if( strlen(element->d_name) <= 256 ) { strcpy(smallestLogFilename, element->d_name); }
				}
			}
		}

		closedir(dir);
	}
	else{
		//couldn't open dir, check errno
		return 6;
	}

	if(smallestLogNumber == -1){
		//couldn't find any files matching the prefix that are a workable log file
		return 1;
	}
	else {
		//found it, all's good
		strcpy(filename, smallestLogFilename);
		return 0;
	}
}

const char* GetPrefix(char subsystem) {
    switch (subsystem) {
        case ACS:
            return "ACS";
        case COMMS:
            return "Comms";
        case HARDWARE:
            return "Hardware";
        case MECH:
            return "Mech";
        case PAYLOAD:
            return "Payload";
        case POWER:
            return "Power";
        case SOFTWARE:
            return "Software";
        case WATCH_PUPPY:
            return "Watch-Puppy";
        default:
            return "";
    }
}

char* GetLogCommand::GetLogFilename(char subsystem) {
    const char* prefix = GetPrefix(subsystem);
    char* result = (char* )malloc(sizeof(char) * 256);
    int status = GetOldestLog(prefix, this->GetLogFolder(), result);
    return result;
}

char* GetLogCommand::ReadLogFile(char* filename, size_t length) {
    if (length > GetLogCommand::MAX_LENGTH) { length = GetLogCommand::MAX_LENGTH; }

    // Find file size
    FILE* fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    unsigned long size = ftell(fp);
    rewind(fp);

    if (length > size) { length = size; }

    char* bufferRead  = (char* )malloc(sizeof(char) * length);
    char* bufferWrite = (char* )malloc(sizeof(char) * size - length);

    size_t bytesRead = 0;
    int retries      = 10;

    // Read requested length text
    while (bytesRead == 0 && retries > 0) {
        bytesRead = fread(bufferRead, 1, length, fp);
        retries -= 1;
    }

    // Read remaining text
    if (bytesRead > 0) {
        bytesRead = fread(bufferWrite, 1, size - length, fp);
    }

    fclose(fp);

    // Write remaining text
    if (bytesRead > 0) {
        fp = fopen(filename, "w");
        fwrite(bufferWrite, sizeof(char), size - length, fp);
        fclose(fp);
    }

    free(bufferWrite);

    return bufferRead;
}

void* GetLogCommand::Execute(){
    char* filename = GetLogFilename(this->GetSubSystem());
    char* filepath = (char*)malloc(sizeof(char) * 256);
    strcpy(filepath, this->GetLogFolder());
    strcat(filepath, "/");
    strcat(filepath, filename);
    char* data     = ReadLogFile(filepath, this->GetLength());

    free(filename);
    free(filepath);
    return (void*)data;
}
