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
