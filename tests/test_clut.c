#include <stdio.h>

static FILE *g_dev_null = NULL;

#define CLUT_STREAM_DEFAULT g_dev_null
#define CLUT_STREAM_FAIL g_dev_null

#define CLUT_OUTPUT_COLOR
#define CLUT_IMPLEMENTATION
#include "../clut.h"

void validate_pass(bool assert_condition, const char *macro_expression, const char *func_name, const char *file, int line) {
  if (assert_condition) {
    fprintf(stderr, CLUT_STR_FAIL);
    fprintf(stderr, "%s", CLUT_STR_BEGIN_RED_TEXT);
    fprintf(stderr, "%s\n", func_name);
    fprintf(stderr, "%s:%d:", file, line);
    fprintf(stderr, ":False-positive:FAILURE detected\n");
    fprintf(stderr, "  -> Expression: %s\n\n", macro_expression);
    fprintf(stderr, "%s", CLUT_STR_END_COLOR_TEXT);
  }
}

void validate_fail(bool assert_condition, const char *macro_expression, const char *func_name, const char *file, int line) {
  if (!assert_condition) {
    fprintf(stderr, CLUT_STR_FAIL);
    fprintf(stderr, "%s", CLUT_STR_BEGIN_RED_TEXT);
    fprintf(stderr, "%s\n", func_name);
    fprintf(stderr, "%s:%d:", file, line);
    fprintf(stderr, ":False-positive:PASS detected\n");
    fprintf(stderr, "  -> Expression: %s\n\n", macro_expression);
    fprintf(stderr, "%s", CLUT_STR_END_COLOR_TEXT);
  } else {
    Clut.current.failed = false;
    Clut.current.header_printed = false;
  }
}

#define VALIDATE_PASS(assertion_expr)                                                                                                                                                                                                                              \
  do {                                                                                                                                                                                                                                                             \
    assertion_expr;                                                                                                                                                                                                                                                \
    validate_pass(Clut.current.failed, #assertion_expr, __func__, __FILE__, __LINE__);                                                                                                                                                                             \
  } while (0)

#define VALIDATE_FAIL(assertion_expr)                                                                                                                                                                                                                              \
  do {                                                                                                                                                                                                                                                             \
    assertion_expr;                                                                                                                                                                                                                                                \
    validate_fail(Clut.current.failed, #assertion_expr, __func__, __FILE__, __LINE__);                                                                                                                                                                             \
  } while (0)

TEST(suite_boolean_and_basic) {
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

TEST(suite_pointers_and_nullness) {
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

TEST(suite_char_comparisons) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR('\n', '\n'));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_CHAR('A', 'B'));

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_CHAR('A', 'B'));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_CHAR('\n', '\n'));
}

TEST(suite_integer_comparisons) {
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

TEST(suite_uint_comparisons) {
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

TEST(suite_floating_point_numbers) {
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

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(-1.0f, -1.0f));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(-1.0f, -1.0f));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(-2.0f, -1.0f));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(-1.0f, -2.0f));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(-1.0f, -2.0f));
  VALIDATE_FAIL(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(-2.0f, -1.0f));

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

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(-1.0, -1.0));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(-1.0, -1.0));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(-2.0, -1.0));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(-1.0, -2.0));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(-1.0, -2.0));
  VALIDATE_FAIL(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(-2.0, -1.0));
}

TEST(suite_floating_point_zero) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT(0.0f, 0.0f));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE(0.0, 0.0));

  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_FLOAT(0.0f, 0.0f));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_DOUBLE(0.0, 0.0));

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(0.0f, 0.0f));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(0.0f, 0.0f));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(0.0, 0.0));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(0.0, 0.0));

  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_FLOAT(0.0f, 0.0f));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_FLOAT(0.0f, 0.0f));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_DOUBLE(0.0, 0.0));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_DOUBLE(0.0, 0.0));
}

TEST(suite_floating_point_precision_accumulation) {
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

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(accumulated_double, expected_double + 1.0));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(accumulated_double, expected_double));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(expected_double + 1.0, accumulated_double));

  VALIDATE_PASS(TEST_ASSERT_LESS_THAN_DOUBLE(expected_double + 1.0, accumulated_double));
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_DOUBLE(accumulated_double, expected_double + 1.0));

  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(expected_double + 1.0, accumulated_double));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(expected_double, accumulated_double));
  VALIDATE_FAIL(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(accumulated_double, expected_double + 1.0));
}

TEST(suite_strings) {
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

TEST(suite_within_comparisons) {
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
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_FLOAT(expected_float, 0.0000001f, accumulated_float + 0.1f));

  double accumulated_double = 0.0;
  for (int i = 0; i < 10; i++) {
    accumulated_double += 0.1;
  }
  double expected_double = 1.0;

  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE(expected_double, 0.00001, accumulated_double));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_DOUBLE(expected_double, 0.0000001, accumulated_double + 0.1));
}

TEST(suite_memory) {
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

  DummyStruct buf1[3], buf2[3], buf3[3];
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

TEST(suite_char_array) {
  char exp[] = "ClutTest";
  char act_ok[] = "ClutTest";
  char act_diff_end[] = "ClutTesX";
  char act_diff_start[] = "XlutTest";
  char act_diff_mid[] = "ClutXest";

  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_ok, 8));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, exp, 8));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_end, 7));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_mid, 4));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_start, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(NULL, NULL, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_end, 0));

  VALIDATE_FAIL(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_end, 8));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_start, 8));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_mid, 8));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, NULL, 8));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_CHAR_ARRAY(NULL, act_ok, 8));
}

TEST(suite_int_array) {
  int arr1[] = {-10, 20, -30, 40, -50};
  int arr2[] = {-10, 20, -30, 40, -50};
  int arr3[] = {-10, -99, -30, 40, -50};
  int arr4[] = {-10, 20, -30, 40, 99};
  int arr5[] = {99, 20, -30, 40, -50};

  size_t len5 = 5;

  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr2, len5));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr1, len5));

  VALIDATE_FAIL(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr3, len5));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr4, len5));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr5, len5));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr3, 1));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr4, 4));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(&arr1[2], &arr2[2], 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_INT_ARRAY(&arr1[1], &arr3[1], 3));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr3, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(NULL, NULL, 0));

  VALIDATE_FAIL(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, NULL, len5));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_INT_ARRAY(NULL, arr2, len5));
}

TEST(suite_uint_array) {
  size_t arr1[] = {10, 20, 30, 40, 50};
  size_t arr2[] = {10, 20, 30, 40, 50};
  size_t arr3[] = {10, 99, 30, 40, 50};
  size_t arr4[] = {10, 20, 30, 40, 99};
  size_t arr5[] = {99, 20, 30, 40, 50};

  size_t len5 = 5;

  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr2, len5));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr1, len5));

  VALIDATE_FAIL(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr3, len5));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr4, len5));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr5, len5));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr3, 1));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr4, 4));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(&arr1[2], &arr2[2], 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_UINT_ARRAY(&arr1[1], &arr3[1], 3));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr3, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(NULL, NULL, 0));

  VALIDATE_FAIL(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, NULL, len5));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_UINT_ARRAY(NULL, arr2, len5));
}

TEST(suite_float_array) {
  float sum = 0.0f;
  for (int i = 0; i < 10; i++) {
    sum += 0.1f;
  }

  float exp[] = {1.0f, 5.5f, 10.123f};
  float act_ok[] = {1.0f, 5.5f, 10.123f};
  float act_accumulated[] = {sum, 5.5f, 10.123f};
  float act_diff_start[] = {1.1f, 5.5f, 10.123f};
  float act_diff_mid[] = {1.0f, 5.501f, 10.123f};
  float act_diff_end[] = {1.0f, 5.5f, 10.124f};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_accumulated, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_mid, 1));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_end, 2));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(NULL, NULL, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_start, 0));

  VALIDATE_FAIL(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_start, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_mid, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_end, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, NULL, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_FLOAT_ARRAY(NULL, act_ok, 3));

  float zero_exp[] = {0.0f, 0.0f};
  float zero_act[] = {0.0f, 0.0f};
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(zero_exp, zero_act, 2));
}

TEST(suite_double_array) {
  double sum = 0.0;
  for (int i = 0; i < 10000; i++) {
    sum += 0.1;
  }

  double exp[] = {1000.0, 0.123456789, 99.99};
  double act_ok[] = {1000.0, 0.123456789, 99.99};
  double act_accumulated[] = {sum, 0.123456789, 99.99};
  double act_diff_start[] = {1000.00001, 0.123456789, 99.99};
  double act_diff_mid[] = {1000.0, 0.123456780, 99.99};
  double act_diff_end[] = {1000.0, 0.123456789, 100.0};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_accumulated, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_mid, 1));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_end, 2));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(NULL, NULL, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_start, 0));

  VALIDATE_FAIL(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_start, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_mid, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_end, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, NULL, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(NULL, act_ok, 3));

  double zero_exp[] = {0.0, 0.0};
  double zero_act[] = {0.0, 0.0};
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(zero_exp, zero_act, 2));
}

TEST(suite_string_array) {
  const char *exp[] = {"clut", "test", "framework"};
  const char *act_ok[] = {"clut", "test", "framework"};
  const char *act_diff_start[] = {"XXXX", "test", "framework"};
  const char *act_diff_mid[] = {"clut", "XXXX", "framework"};
  const char *act_diff_end[] = {"clut", "test", "XXXXXXXXX"};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_mid, 1));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_end, 2));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_start, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(NULL, NULL, 0));

  VALIDATE_FAIL(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_start, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_mid, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_end, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, NULL, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_STRING_ARRAY(NULL, act_ok, 3));

  const char *exp_null[] = {"clut", NULL, "framework"};
  const char *act_null[] = {"clut", NULL, "framework"};
  const char *act_no_null[] = {"clut", "test", "framework"};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp_null, act_null, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_STRING_ARRAY(exp_null, act_no_null, 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_STRING_ARRAY(act_no_null, exp_null, 3));
}

TEST(suite_within_char_array) {
  char exp[] = {'2', 'd', 20};
  char act_ok[] = {'4', 'a', 23};
  char act_fail[] = {'2', 'd', 26};

  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 5, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 5, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 5, act_fail, 2));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 0, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR_ARRAY(NULL, 5, NULL, 0));

  VALIDATE_FAIL(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 5, act_fail, 3));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 0, act_ok, 3));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 5, NULL, 3));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_CHAR_ARRAY(NULL, 5, act_ok, 3));
}

TEST(suite_within_int_array) {
  int exp[] = {100, -50, 0, 200};
  int act_ok[] = {105, -45, 10, 195};
  int act_fail[] = {105, -45, 11, 195};

  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 10, act_ok, 4));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 10, exp, 4));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 10, act_fail, 2));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 0, exp, 4));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT_ARRAY(NULL, 10, NULL, 0));

  VALIDATE_FAIL(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 10, act_fail, 4));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 0, act_ok, 4));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 10, NULL, 4));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_INT_ARRAY(NULL, 10, act_ok, 4));
}

TEST(suite_within_uint_array) {
  size_t exp[] = {1000, 500, 250};
  size_t act_ok[] = {1040, 550, 250};
  size_t act_fail[] = {1040, 551, 250};

  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 50, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 50, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 50, act_fail, 1));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 0, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT_ARRAY(NULL, 50, NULL, 0));

  VALIDATE_FAIL(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 50, act_fail, 3));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 0, act_ok, 3));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 50, NULL, 3));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_UINT_ARRAY(NULL, 50, act_ok, 3));
}

TEST(suite_within_float_array) {
  float exp[] = {1.0f, 5.5f, 10.0f};
  float act_ok[] = {1.05f, 5.45f, 10.09f};
  float act_fail[] = {1.05f, 5.45f, 10.11f};

  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.1f, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.1f, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.1f, act_fail, 2));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.0f, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT_ARRAY(NULL, 0.1f, NULL, 0));

  VALIDATE_FAIL(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.1f, act_fail, 3));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.0f, act_ok, 3));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.1f, NULL, 3));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_FLOAT_ARRAY(NULL, 0.1f, act_ok, 3));
}

TEST(suite_within_double_array) {
  double exp[] = {1.0, 5.5, 10.0};
  double act_ok[] = {1.05, 5.45, 10.09};
  double act_fail[] = {1.05, 5.45, 10.11};

  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.1, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.1, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.1, act_fail, 2));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.0, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(NULL, 0.1, NULL, 0));

  VALIDATE_FAIL(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.1, act_fail, 3));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.0, act_ok, 3));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.1, NULL, 3));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(NULL, 0.1, act_ok, 3));
}

void setup_stream_file() {
#ifdef _WIN32
  g_dev_null = fopen("nul", "w");
#else
  g_dev_null = fopen("/dev/null", "w");
#endif
}

void close_stream_file() {
  if (g_dev_null)
    fclose(g_dev_null);
}

int main(void) {
  CLUT_BEFORE_ALL(setup_stream_file);
  CLUT_AFTER_ALL(close_stream_file);

  TEST_BEGIN();

  TEST_RUN(suite_boolean_and_basic);
  TEST_RUN(suite_pointers_and_nullness);
  TEST_RUN(suite_char_comparisons);
  TEST_RUN(suite_integer_comparisons);
  TEST_RUN(suite_uint_comparisons);
  TEST_RUN(suite_floating_point_numbers);
  TEST_RUN(suite_floating_point_zero);
  TEST_RUN(suite_floating_point_precision_accumulation);
  TEST_RUN(suite_strings);
  TEST_RUN(suite_within_comparisons);
  TEST_RUN(suite_memory);
  TEST_RUN(suite_char_array);
  TEST_RUN(suite_int_array);
  TEST_RUN(suite_uint_array);
  TEST_RUN(suite_float_array);
  TEST_RUN(suite_double_array);
  TEST_RUN(suite_string_array);
  TEST_RUN(suite_within_char_array);
  TEST_RUN(suite_within_int_array);
  TEST_RUN(suite_within_uint_array);
  TEST_RUN(suite_within_float_array);
  TEST_RUN(suite_within_double_array);

  return TEST_END();
}
