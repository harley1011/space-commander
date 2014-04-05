#include <time.h>
#include <stdio.h>
#include "Date.h"


Date::Date(int year, int month, int day){
    this->year = year;
    this->month = month;
    this->day = day;

    this->BuildString();
}

Date::Date(time_t raw_time){
    struct tm* timeinfo = localtime(&raw_time);

    this->year = timeinfo->tm_year + 1900;
    this->month = timeinfo->tm_mon + 1;
    this->day = timeinfo->tm_mday;

    this->BuildString();
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetTimeT
*
* PURPOSE : returns the time_t corresponding to the Date
*
*------------------------------------------------------------------------------*/
time_t Date::GetTimeT(void){
    struct tm timeinfo = {0};

    timeinfo.tm_year = this->year - 1900;
    timeinfo.tm_mon  = this->month - 1;
    timeinfo.tm_mday = this->day;

    return mktime(&timeinfo);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetString
*
* PURPOSE : returns a pointer to date_str. Do NOT use this pointer after
*           destruction of the object. Make a copy!
*
*------------------------------------------------------------------------------*/
const char* Date::GetString(void){
    return date_str;
}



//-----------------
// Pivate functions
//-----------------



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : BuildString
*
* PURPOSE : Builds the date_str
*
*------------------------------------------------------------------------------*/
void Date::BuildString(void){
    snprintf(date_str, 9, "%04d%02d%02d", this->year, this->month, this->day);
}
