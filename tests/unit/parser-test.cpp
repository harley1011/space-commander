#include "CppUTest/TestHarness.h"
#include <cstring>
#include "parser.h"

static const int BUFFER_SIZE = 10;
static char zeroOutBuffer[BUFFER_SIZE];

TEST_GROUP(Parser) {
    void setup() {
        memset(zeroOutBuffer, 0, sizeof(char) * BUFFER_SIZE);
    }

    void teardown() {}
};

TEST(Parser, ParseBytes_InvalidCommandType_ReturnsNull) {
    char* actual   = NULL;
    char* expected = NULL;
    char  input[1] = {-1}; //First element represent type

    actual = (char* )Parser::ParseBytes(input);
    POINTERS_EQUAL(expected, actual);
}
