/*=============================================================================
*
*   AUTHOR      : Space Concordia 2014 
*
*   PURPOSE     : The Date is used to convert YYYYMMDD to time_t and
*                 time_t to YYYYMMDD
*
*============================================================================*/
#ifndef _DATE_H_
#define _DATE_H_

class Date {
    private :
        int year;
        int month;
        int day;
        char date_str[9];
        time_t time_t_format;

    public :
        Date();
        Date(int year, int month, int day);
        Date(time_t raw_time);

        time_t GetTimeT(void);
        const char* GetString(void); // When the object is destroyed, the memory location is deallocated, make a copy!


    private :
        void MakeTimeT(void);
        void BuildString(void);
};

#endif
