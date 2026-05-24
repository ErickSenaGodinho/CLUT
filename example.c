#define CLUT_IMPLEMENTATION
#define CLUT_OUTPUT_COLOR

#include "clut.h"

int CTX_COUNTER;

int add(int a, int b) { return a + b; }
float divide(float a, float b) { return b != 0.0f ? a / b : 0.0f; }
char *say_hello() { return "Hello"; }
void *return_null() { return NULL; }

void test_add() {
  TEST_ASSERT_EQUAL_INT(5, add(2, 3));  // Passed
  TEST_ASSERT_EQUAL_INT(0, add(-1, 1)); // Passed
}

void test_divide() {
  TEST_ASSERT_EQUAL_FLOAT(0.5f, divide(1.0f, 2.0f)); // Passed
  TEST_ASSERT_EQUAL_FLOAT(0.0f, divide(5.0f, 0.0f)); // Passed
}

void test_strings() {
  TEST_ASSERT_EQUAL_STRING("Hello", say_hello());            // Passed
  TEST_ASSERT_NOT_EQUAL_STRING_LEN("Hella", say_hello(), 5); // Passed
  TEST_ASSERT_NOT_NULL(say_hello());                         // Passed
}

void test_pointers() {
  int x = 10;
  int *p1 = &x;
  int *p2 = &x;

  TEST_ASSERT_NULL(return_null()); // Passed
  TEST_ASSERT_EQUAL_PTR(p1, p2);   // Passed
}

void test_comparisons() {
  float x = 0;
  for (int i = 0; i < 10; i++)
    x += 0.1f;

  TEST_ASSERT_LESS_OR_EQUAL_FLOAT(1.0f, x);        // Passed
  TEST_ASSERT_GREATER_THAN_INT(5, 10);             // Passed
  TEST_ASSERT_LESS_OR_EQUAL_INT(5, 5);             // Passed
  TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(3.0f, 3.14f); // Passed
}

void test_context() {
  TEST_ASSERT_EQUAL_INT(12, CTX_COUNTER);        // Passed
  TEST_ASSERT_GREATER_THAN_INT(20, CTX_COUNTER); // Failed: expected 12 > 20
  TEST_ASSERT_EQUAL_INT(10, CTX_COUNTER);        // Not validated due to previous failure
}

void before_all() { CTX_COUNTER = 5; }
void before_each() { CTX_COUNTER += 2; }
void after_each() { CTX_COUNTER -= 1; }
void after_all() { CTX_COUNTER = 0; }

int main() {
  CLUT_BEFORE_ALL(before_all);
  CLUT_BEFORE_EACH(before_each);
  CLUT_AFTER_EACH(after_each);
  CLUT_AFTER_ALL(after_all);

  TEST_BEGIN();

  TEST_RUN(test_add);         // Passed: all assertions passed
  TEST_RUN(test_divide);      // Passed: all assertions passed
  TEST_RUN(test_strings);     // Passed: all assertions passed
  TEST_RUN(test_pointers);    // Passed: all assertions passed
  TEST_RUN(test_comparisons); // Passed: all assertions passed
  TEST_RUN(test_context);     // Failed: one assertion failed

  return TEST_END();
}
