#define CLUT_IMPLEMENTATION

#include "clut.h"

int main() {
  TEST_ASSERT_FALSE(0 == 1);
  TEST_ASSERT_EQUALS_INT(0, 0);
  TEST_ASSERT_EQUALS_FLOAT_MESSAGE(0.3f, 0.2f + 0.1f, "Must be 0.3f");
}
