#define CLUT_IMPLEMENTATION
#include "../clut.h"

TEST(add) {
  TEST_ASSERT_EQUAL_INT(5, 2 + 3);  // Passed
  TEST_ASSERT_EQUAL_INT(0, -1 + 1); // Passed
}

TEST(sub) {
  TEST_ASSERT_EQUAL_INT(1, 3 - 2);  // Passed
  TEST_ASSERT_EQUAL_INT(0, -1 - 1); // Failed -> 0 != -2
}

int main(void) {
  RUNNER_BEGIN();
  SUITE_BEGIN();

  RUN_TEST(add); // Passed: all assertions passed
  RUN_TEST(sub); // Failed: one or more assertions failed

  SUITE_END();
  return RUNNER_END();
}
