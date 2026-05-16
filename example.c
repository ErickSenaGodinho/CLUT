#define CLUT_IMPLEMENTATION
#define CLUT_OUTPUT_COLOR

#include "clut.h"

void test1() { TEST_ASSERT_FALSE(1 == 1); }
void test2() { TEST_ASSERT_EQUALS_INT(1, 0); }
void test3() { TEST_ASSERT_EQUALS_FLOAT_MESSAGE(0.3f, 0.2f + 0.1f, "Must be 0.3f"); }
void test4() {
  TEST_ASSERT_EQUALS_STRING("Teste", "Teste");
  TEST_ASSERT_NULL(NULL);
  TEST_ASSERT_NOT_NULL(NULL);
}
void test5() {
  TEST_ASSERT_GREATER_THAN_INT(1, 2);
  TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(2.2f, 2.2f);
  TEST_ASSERT_LESS_THAN_DOUBLE(2.2, 1.5);
  TEST_ASSERT_LESS_OR_EQUAL_INT(1, 1);
}

int main() {
  TEST_BEGIN();
  TEST_RUN(test1);
  TEST_RUN(test2);
  TEST_RUN(test3);
  TEST_RUN(test4);
  TEST_RUN(test5);
  return TEST_END();
}
