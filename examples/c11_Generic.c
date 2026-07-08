#define IS_INT(X) _Generic((X), int: 1, default: 0)
#define IS_FLOAT(X) _Generic((X), float: 1, default: 0)

#define CLUT_IMPLEMENTATION
#include "../clut.h"

TEST(_Generic) {
  int my_int = 10;
  float my_float = 5.5f;

  TEST_ASSERT_TRUE(IS_INT(my_int));
  TEST_ASSERT_TRUE(IS_FLOAT(my_float));

  TEST_ASSERT_FALSE(IS_INT(my_float));
  TEST_ASSERT_FALSE(IS_FLOAT(my_int));
}

int main(void) {
  RUNNER_BEGIN();
  SUITE_BEGIN();

  RUN_TEST(_Generic);

  SUITE_END();
  return RUNNER_END();
}
