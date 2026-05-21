#include <stdio.h>

static FILE *g_dev_null = NULL;

#define CLUT_META_TESTING
#define CLUT_STREAM_DEFAULT stdout
#define CLUT_STREAM_FAIL g_dev_null

#define CLUT_OUTPUT_COLOR
#define CLUT_IMPLEMENTATION
#include "clut.h"

void validate_pass(bool assert_condition, const char *macro_expression, const char *func_name, const char *file, int line) {
  if (assert_condition) {
    fprintf(stderr, "%s:%d:%s:", file, line, func_name);
    fprintf(stderr, CLUT_STR_FAIL ":False-positive FAILURE detected\n");
    fprintf(stderr, "  -> Expression: %s\n\n", macro_expression);
    Clut.failures++;
  }
}

void validate_fail(bool assert_condition, const char *macro_expression, const char *func_name, const char *file, int line) {
  if (!assert_condition) {
    fprintf(stderr, "%s:%d:%s:", file, line, func_name);
    fprintf(stderr, CLUT_STR_FAIL ":False-positive PASS detected\n");
    fprintf(stderr, "  -> Expression: %s\n\n", macro_expression);
    Clut.failures++;
  }
}

#define VALIDATE_PASS(assertion_expr)                                                                                                                                                                                                                              \
  do {                                                                                                                                                                                                                                                             \
    Clut.total_tests++;                                                                                                                                                                                                                                            \
    Clut.current_test_failed = false;                                                                                                                                                                                                                              \
    assertion_expr;                                                                                                                                                                                                                                                \
    validate_pass(Clut.current_test_failed, #assertion_expr, __func__, __FILE__, __LINE__);                                                                                                                                                                        \
  } while (0)

#define VALIDATE_FAIL(assertion_expr)                                                                                                                                                                                                                              \
  do {                                                                                                                                                                                                                                                             \
    Clut.total_tests++;                                                                                                                                                                                                                                            \
    Clut.current_test_failed = false;                                                                                                                                                                                                                              \
    assertion_expr;                                                                                                                                                                                                                                                \
    validate_fail(Clut.current_test_failed, #assertion_expr, __func__, __FILE__, __LINE__);                                                                                                                                                                        \
  } while (0)

void test_suite_boolean_and_basic(void) {
  int val = 1;
  VALIDATE_PASS(TEST_ASSERT(val == 1));
  VALIDATE_FAIL(TEST_ASSERT(val == 0));

  VALIDATE_PASS(TEST_ASSERT_TRUE(true));
  VALIDATE_FAIL(TEST_ASSERT_TRUE(false));

  VALIDATE_PASS(TEST_ASSERT_FALSE(false));
  VALIDATE_FAIL(TEST_ASSERT_FALSE(true));

  VALIDATE_PASS(TEST_ASSERT_UNLESS(false));
  VALIDATE_FAIL(TEST_ASSERT_UNLESS(true));
}

void test_suite_pointers_and_nullness(void) {
  int dummy = 42;
  int *ptr = &dummy;

  VALIDATE_PASS(TEST_ASSERT_NULL(NULL));
  VALIDATE_FAIL(TEST_ASSERT_NULL(ptr));

  VALIDATE_PASS(TEST_ASSERT_NOT_NULL(ptr));
  VALIDATE_FAIL(TEST_ASSERT_NOT_NULL(NULL));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_PTR(ptr, ptr));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_PTR(ptr, NULL));

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_PTR(ptr, NULL));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_PTR(ptr, ptr));
}

void test_suite_char_comparisons(void) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR('\n', '\n'));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_CHAR('A', 'B'));

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_CHAR('A', 'B'));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_CHAR('\n', '\n'));
}

void test_suite_integer_comparisons(void) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT(10, 10));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_INT(10, 20));

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_INT(10, 20));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_INT(10, 10));

  VALIDATE_PASS(TEST_ASSERT_GREATER_THAN_INT(5, 10));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_INT(10, 5));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_INT(10, 10));

  VALIDATE_PASS(TEST_ASSERT_LESS_THAN_INT(10, 5));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_INT(5, 10));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_INT(5, 5));

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_INT(5, 10));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_INT(10, 10));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_OR_EQUAL_INT(10, 5));

  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_INT(10, 5));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_INT(5, 5));
  VALIDATE_FAIL(TEST_ASSERT_LESS_OR_EQUAL_INT(5, 10));
}

void test_suite_uint_comparisons(void) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT(10, 10));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_UINT(10, 20));

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_UINT(10, 20));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_UINT(10, 10));

  VALIDATE_PASS(TEST_ASSERT_GREATER_THAN_UINT(5, 10));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_UINT(10, 5));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_UINT(10, 10));

  VALIDATE_PASS(TEST_ASSERT_LESS_THAN_UINT(10, 5));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_UINT(5, 10));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_UINT(5, 5));

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_UINT(5, 10));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_UINT(10, 10));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_OR_EQUAL_UINT(10, 5));

  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_UINT(10, 5));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_UINT(5, 5));
  VALIDATE_FAIL(TEST_ASSERT_LESS_OR_EQUAL_UINT(5, 10));
}

void test_suite_floating_point_numbers(void) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT(3.14f, 3.14f));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_FLOAT(3.14f, 2.71f));

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_FLOAT(3.14f, 2.71f));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_FLOAT(3.14f, 3.14f));

  VALIDATE_PASS(TEST_ASSERT_GREATER_THAN_FLOAT(1.0f, 2.0f));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_FLOAT(2.0f, 1.0f));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_FLOAT(2.0f, 2.0f));

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(1.0f, 2.0f));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(2.0f, 2.0f));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(2.0f, 1.0f));

  VALIDATE_PASS(TEST_ASSERT_LESS_THAN_FLOAT(2.0f, 1.0f));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_FLOAT(1.0f, 2.0f));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_FLOAT(1.0f, 1.0f));

  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(2.0f, 1.0f));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(1.0f, 1.0f));
  VALIDATE_FAIL(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(1.0f, 2.0f));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE(3.14159, 3.14159));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_DOUBLE(3.14159, 2.71828));

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_DOUBLE(3.14159, 2.71828));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_DOUBLE(3.14159, 3.14159));

  VALIDATE_PASS(TEST_ASSERT_GREATER_THAN_DOUBLE(1.0, 2.0));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_DOUBLE(2.0, 1.0));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_DOUBLE(2.0, 2.0));

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(1.0, 2.0));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(2.0, 2.0));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(2.0, 1.0));

  VALIDATE_PASS(TEST_ASSERT_LESS_THAN_DOUBLE(2.0, 1.0));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_DOUBLE(1.0, 2.0));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_DOUBLE(1.0, 1.0));

  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(2.0, 1.0));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(1.0, 1.0));
  VALIDATE_FAIL(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(1.0, 2.0));
}

void test_suite_floating_point_precision_accumulation(void) {
  float accumulated_float = 0.0f;
  for (int i = 0; i < 10; i++) {
    accumulated_float += 0.1f;
  }
  float expected_float = 1.0f;

  double accumulated_double = 0.0;
  for (int i = 0; i < 10; i++) {
    accumulated_double += 0.1;
  }
  double expected_double = 1.0;

  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT(expected_float, accumulated_float));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_FLOAT(expected_float, accumulated_float));

  VALIDATE_PASS(TEST_ASSERT_GREATER_THAN_FLOAT(accumulated_float, expected_float + 1.0f));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_FLOAT(expected_float + 1.0f, accumulated_float));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_FLOAT(accumulated_float, expected_float));

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(accumulated_float, expected_float + 1.0f));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(accumulated_float, expected_float));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(expected_float + 1.0f, accumulated_float));

  VALIDATE_PASS(TEST_ASSERT_LESS_THAN_FLOAT(expected_float + 1.0f, accumulated_float));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_FLOAT(accumulated_float, expected_float + 1.0f));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_FLOAT(expected_float, accumulated_float));

  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(expected_float + 1.0f, accumulated_float));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(expected_float, accumulated_float));
  VALIDATE_FAIL(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(accumulated_float, expected_float + 1.0f));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE(expected_double, accumulated_double));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_DOUBLE(expected_double, accumulated_double));

  VALIDATE_PASS(TEST_ASSERT_GREATER_THAN_DOUBLE(accumulated_double, expected_double + 1.0));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_DOUBLE(expected_double + 1.0, accumulated_double));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_DOUBLE(accumulated_double, expected_double));

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(accumulated_double, expected_double + 1.0));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(accumulated_double, expected_double));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(expected_double + 1.0, accumulated_double));

  VALIDATE_PASS(TEST_ASSERT_LESS_THAN_DOUBLE(expected_double + 1.0, accumulated_double));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_DOUBLE(accumulated_double, expected_double + 1.0));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_DOUBLE(expected_double, accumulated_double));

  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(expected_double + 1.0, accumulated_double));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(expected_double, accumulated_double));
  VALIDATE_FAIL(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(accumulated_double, expected_double + 1.0));
}

void test_suite_strings(void) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING("clut", "clut"));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_STRING("clut", "different"));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_STRING("clut", NULL));

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_STRING("clut", "different"));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_STRING("clut", "clut"));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_LEN("clut_framework", "clut_testing", 5));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_STRING_LEN("clut_framework", "clut_testing", 10));

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_STRING_LEN("clut_framework", "clut_testing", 10));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_STRING_LEN("clut_framework", "clut_testing", 5));
}

void test_suite_within_comparisons(void) {
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR(50, 5, 52));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR(50, 5, 55));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR(50, 5, 45));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_CHAR(50, 5, 56));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_CHAR(50, 5, 44));

  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT(100, 10, 105));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT(100, 10, 110));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT(100, 10, 90));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_INT(100, 10, 111));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_INT(100, 10, 89));

  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT(1000, 50, 980));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT(1000, 50, 1050));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT(1000, 50, 950));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_UINT(1000, 50, 1051));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_UINT(1000, 50, 949));

  float accumulated_float = 0.0f;
  for (int i = 0; i < 10; i++) {
    accumulated_float += 0.1f;
  }
  float expected_float = 1.0f;

  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT(expected_float, 0.00001f, accumulated_float));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT(expected_float, 0.0f, accumulated_float));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_FLOAT(expected_float, 0.0000001f, accumulated_float + 0.1f));

  double accumulated_double = 0.0;
  for (int i = 0; i < 10; i++) {
    accumulated_double += 0.1;
  }
  double expected_double = 1.0;

  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE(expected_double, 0.00001, accumulated_double));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE(expected_double, 0.0, accumulated_double));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_DOUBLE(expected_double, 0.0000001, accumulated_double + 0.1));
}

void test_suite_memory(void) {
  typedef struct {
    int x;
    int y;
    char c;
  } DummyStruct;

  DummyStruct s1 = {0};
  DummyStruct s2 = {0};
  DummyStruct s3 = {0};
  DummyStruct s4 = {0};

  s1.x = 10;
  s1.y = 20;
  s1.c = 'A';

  s2.x = 10;
  s2.y = 20;
  s2.c = 'A';

  s3.x = 10;
  s3.y = 99;
  s3.c = 'A';

  s4.x = 10;
  s4.y = 20;
  s4.c = 'Z';

  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(&s1, &s2, sizeof(DummyStruct)));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(&s1, &s1, sizeof(DummyStruct)));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_MEMORY(&s1, &s3, sizeof(DummyStruct)));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_MEMORY(&s1, &s4, sizeof(DummyStruct)));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(NULL, NULL, 0));

  DummyStruct buf1[3];
  DummyStruct buf2[3];
  DummyStruct buf3[3];

  memset(buf1, 0, sizeof(buf1));
  memset(buf2, 0, sizeof(buf2));
  memset(buf3, 0, sizeof(buf3));

  buf1[0] = (DummyStruct){10, 20, 'A'};
  buf1[1] = (DummyStruct){30, 40, 'B'};
  buf1[2] = (DummyStruct){50, 60, 'C'};

  buf2[0] = (DummyStruct){10, 20, 'A'};
  buf2[1] = (DummyStruct){30, 40, 'B'};
  buf2[2] = (DummyStruct){50, 60, 'C'};

  buf3[0] = (DummyStruct){10, 20, 'A'};
  buf3[1] = (DummyStruct){30, 99, 'B'};
  buf3[2] = (DummyStruct){50, 60, 'C'};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(buf1, buf2, sizeof(buf1)));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_MEMORY(buf1, buf3, sizeof(buf1)));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(buf1, buf3, sizeof(DummyStruct)));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(&buf1[1], &buf2[1], sizeof(DummyStruct)));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(NULL, NULL, 0));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_MEMORY(buf1, NULL, sizeof(buf1)));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_MEMORY(NULL, buf2, sizeof(buf2)));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(&s1, &s2, 0));
}

int main(void) {
#ifdef _WIN32
  g_dev_null = fopen("nul", "w");
#else
  g_dev_null = fopen("/dev/null", "w");
#endif

  TEST_BEGIN();

  TEST_RUN(test_suite_boolean_and_basic);
  TEST_RUN(test_suite_pointers_and_nullness);
  TEST_RUN(test_suite_char_comparisons);
  TEST_RUN(test_suite_integer_comparisons);
  TEST_RUN(test_suite_uint_comparisons);
  TEST_RUN(test_suite_floating_point_numbers);
  TEST_RUN(test_suite_floating_point_precision_accumulation);
  TEST_RUN(test_suite_strings);
  TEST_RUN(test_suite_within_comparisons);
  TEST_RUN(test_suite_memory);

  int exit_code = TEST_END();

  if (g_dev_null)
    fclose(g_dev_null);
  return exit_code;
}
