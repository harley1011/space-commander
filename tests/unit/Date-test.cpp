
#include "CppUTest/TestHarness.h"
#include "Date.h"


TEST_GROUP(DateTestGroup){
    void setup(){
    }
    void teardown(){
    }
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetTimeT_returns_time_t
* 
* PURPOSE : Converts YYYYMMDD to time_t back and forth.
*
*-----------------------------------------------------------------------------*/
TEST(DateTestGroup, GetTimeT_returns_time_t){
    Date date(2014, 1, 1);

    time_t rawtime = date.GetTimeT();

    Date date_raw(rawtime);

    STRCMP_EQUAL(date.GetString(), date_raw.GetString()); 

}
