#include "CppUTest/TestHarness.h"
#include <cstring>
#include <string>
#include "parser.h"
#include <iostream>

using namespace std;

static const int BUFFER_SIZE = 10;
static char zeroOutBuffer[BUFFER_SIZE];

TEST_GROUP(Parser) {
    void setup() {
        memset(zeroOutBuffer, 0, sizeof(char) * BUFFER_SIZE);
    }

    void teardown() {}
};

TEST(Parser, ParseBytes_InvalidCommandType_ReturnsNull) {
    CommandParam* actual   = NULL;
    CommandParam* expected = NULL;
    char input[1]          = {-1}; //First element represent type

    actual = Parser::ParseBytes(input);
    POINTERS_EQUAL(expected, actual);
}

TEST(Parser, ParseBytes_ValidCommandType_ReturnCommandParamObject) {
    char input[2]          = {2, 2};
    CommandParam* actual   = NULL;
    CommandParam expected;

    expected.type      = 2;
    expected.fields    = new void*[1];
    expected.fields[0] = (void* )2;

    actual = (CommandParam*) Parser::ParseBytes(input);
    CHECK_EQUAL(expected.type,      actual->type);
    CHECK_EQUAL(expected.fields[0], actual->fields[0]);
    delete actual;
}

TEST(Parser, ParseBytes_CommandWithStringField_ReturnsString) {
    char input[BUFFER_SIZE];
    input[0] = 1;
    input[1] = 4;   //Size of the string, no need for \0
    input[2] = 't';
    input[3] = 'e';
    input[4] = 's';
    input[5] = 't';

    CommandParam* actual  = Parser::ParseBytes(input);
    string* actualString  = static_cast<string* >(actual->fields[0]);
    string expectedString = "test";

    CHECK_EQUAL(expectedString, *actualString);
  

    //TODO: change void** fields to vector<void *>, delete each element in the vector
    delete actualString; 
    delete actual;
}
