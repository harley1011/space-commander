#include "CppUTest/TestHarness.h"
#include <string.h>

static const int BUFFER_SIZE = 10;
static char zeroOutBuffer[BUFFER_SIZE];

TEST_GROUP(Parser) {
    void setup() {
        memset(zeroOutBuffer, 0, sizeof(char) * BUFFER_SIZE);
    }

    void teardown() {}
};

TEST(Parser, ParseBytes_InvalidArrayOfBytes_ReturnsNull) {
    FAIL("Do me!");
}
