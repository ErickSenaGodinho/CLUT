#include <stdio.h>
#include <string.h>

#define CLUT_OUTPUT_COLOR
#define CLUT_IMPLEMENTATION
#include "../clut.h"

ClutSB test_message;
ClutSB output;

static void record_failure(const char *file, int line) {
  ClutLogRecord record;
  clut_log_record_capture(&record, file, line);
  clut_dispatch_fail_append(&record, &output, &test_message);
  clut_sb_clear(&test_message);
}

static bool clut_validate_pass(bool failed, const char *expr, const char *file, int line) {
  if (failed) {
    const char *clut_msg = clut_sb_cstr(&Clut.runner.test_message);
    clut_sb_append(&test_message, "FALSE-POSITIVE (expected PASS, got FAIL)\n  expr: ");
    clut_sb_append(&test_message, expr);
    clut_sb_append(&test_message, "\n  msg : ");
    clut_sb_append(&test_message, clut_msg);
    record_failure(file, line);
    clut_sb_clear(&Clut.runner.output);
    clut_sb_clear(&Clut.runner.test_message);
    return true;
  }
  return false;
}

static bool clut_validate_fail(bool failed, const char *expr, const char *expected_msg, const char *file, int line) {
  if (!failed) {
    clut_sb_append(&test_message, "FALSE-NEGATIVE (expected FAIL, got PASS)\n  expr: ");
    clut_sb_append(&test_message, expr);
    record_failure(file, line);
    return true;
  }
  if (expected_msg != NULL) {
    const char *actual_msg = clut_sb_cstr(&Clut.runner.test_message);
    if (strcmp(actual_msg, expected_msg) != 0) {
      clut_sb_append(&test_message, "WRONG MESSAGE\n  expr    : ");
      clut_sb_append(&test_message, expr);
      clut_sb_append(&test_message, "\n  expected: ");
      clut_sb_append(&test_message, expected_msg);
      clut_sb_append(&test_message, "\n  actual  : ");
      clut_sb_append(&test_message, actual_msg);
      record_failure(file, line);
      clut_sb_clear(&Clut.runner.output);
      clut_sb_clear(&Clut.runner.test_message);
      return true;
    }
  }
  clut_sb_clear(&Clut.runner.output);
  clut_sb_clear(&Clut.runner.test_message);
  return false;
}

static inline bool clut_preserve_begin(void) {
  bool prev = Clut.current.failed;
  if (prev) {
    Clut.current.failed = false;
  }
  return prev;
}

static inline void clut_preserve_end(bool prev_failed, bool validation_failed) { Clut.current.failed = prev_failed || validation_failed; }

#define VALIDATE_PASS(assertion_expr)                                                                                                                                                                                                                              \
  do {                                                                                                                                                                                                                                                             \
    bool __prev_failed = clut_preserve_begin();                                                                                                                                                                                                                    \
    assertion_expr;                                                                                                                                                                                                                                                \
    bool __validation_failed = clut_validate_pass(Clut.current.failed, #assertion_expr, __FILE__, __LINE__);                                                                                                                                                       \
    clut_preserve_end(__prev_failed, __validation_failed);                                                                                                                                                                                                         \
  } while (0)

#define VALIDATE_FAIL(assertion_expr)                                                                                                                                                                                                                              \
  do {                                                                                                                                                                                                                                                             \
    bool __prev_failed = clut_preserve_begin();                                                                                                                                                                                                                    \
    assertion_expr;                                                                                                                                                                                                                                                \
    bool __validation_failed = clut_validate_fail(Clut.current.failed, #assertion_expr, NULL, __FILE__, __LINE__);                                                                                                                                                 \
    clut_preserve_end(__prev_failed, __validation_failed);                                                                                                                                                                                                         \
  } while (0)

#define VALIDATE_FAIL_MSG(assertion_expr, expected_msg)                                                                                                                                                                                                            \
  do {                                                                                                                                                                                                                                                             \
    bool __prev_failed = clut_preserve_begin();                                                                                                                                                                                                                    \
    assertion_expr;                                                                                                                                                                                                                                                \
    bool __validation_failed = clut_validate_fail(Clut.current.failed, #assertion_expr, (expected_msg), __FILE__, __LINE__);                                                                                                                                       \
    clut_preserve_end(__prev_failed, __validation_failed);                                                                                                                                                                                                         \
  } while (0)

#define VALIDATE_CUSTOM_MESSAGE(assertion_expr, expected_msg)                                                                                                                                                                                                      \
  do {                                                                                                                                                                                                                                                             \
    bool __prev_failed = clut_preserve_begin();                                                                                                                                                                                                                    \
    assertion_expr;                                                                                                                                                                                                                                                \
    bool __validation_failed = clut_validate_fail(Clut.current.failed, #assertion_expr, (expected_msg), __FILE__, __LINE__);                                                                                                                                       \
    clut_preserve_end(__prev_failed, __validation_failed);                                                                                                                                                                                                         \
  } while (0)

TEST(suite_boolean_and_basic) {
  int val = 1;
  VALIDATE_PASS(TEST_ASSERT(val == 1));
  VALIDATE_FAIL_MSG(TEST_ASSERT(val == 0), "Expression Is False -> val == 0");

  VALIDATE_PASS(TEST_ASSERT_TRUE(true));
  VALIDATE_FAIL_MSG(TEST_ASSERT_TRUE(false), "Expected True But Was False -> false");

  VALIDATE_PASS(TEST_ASSERT_FALSE(false));
  VALIDATE_FAIL_MSG(TEST_ASSERT_FALSE(true), "Expected False But Was True -> true");

  VALIDATE_PASS(TEST_ASSERT_UNLESS(false));
  VALIDATE_FAIL_MSG(TEST_ASSERT_UNLESS(true), "Expression Is True -> true");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_MESSAGE(false, "custom assert"), "custom assert");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_TRUE_MESSAGE(false, "custom true"), "custom true");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_FALSE_MESSAGE(true, "custom false"), "custom false");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_UNLESS_MESSAGE(true, "custom unless"), "custom unless");
}

TEST(suite_pointers_and_nullness) {
  int dummy = 42;
  int *ptr = &dummy;

  VALIDATE_PASS(TEST_ASSERT_NULL(NULL));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NULL(ptr), "Expected NULL");

  VALIDATE_PASS(TEST_ASSERT_NOT_NULL(ptr));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_NULL(NULL), "Expected Non-NULL");

  VALIDATE_PASS(TEST_ASSERT_EQUAL_PTR(ptr, ptr));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_PTR(ptr, NULL));

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_PTR(ptr, NULL));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_PTR(ptr, ptr));

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NULL_MESSAGE(ptr, "custom null"), "custom null");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_NULL_MESSAGE(NULL, "custom not null"), "custom not null");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_EQUAL_PTR_MESSAGE(ptr, ptr, "custom ptr"), "custom ptr");
}

TEST(suite_char_comparisons) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR('\n', '\n'));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_CHAR('A', 'B'), "Expected 'B' (0x42) Received 'A' (0x41)");

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_CHAR('A', 'B'));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_EQUAL_CHAR('\n', '\n'), "Expected '\n' (0x0A) to not be equal to '\n' (0x0A)");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_CHAR_MESSAGE('A', 'B', "custom eq char"), "custom eq char");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_EQUAL_CHAR_MESSAGE('\n', '\n', "custom neq char"), "custom neq char");
}

TEST(suite_integer_comparisons) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT(10, 10));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_INT(10, 20), "Expected 20 Received 10");

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_INT(10, 20));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_EQUAL_INT(10, 10), "Expected 10 to not be equal to 10");

  VALIDATE_PASS(TEST_ASSERT_GREATER_THAN_INT(5, 10));
  VALIDATE_FAIL_MSG(TEST_ASSERT_GREATER_THAN_INT(10, 5), "Expected 5 to be greater than 10");
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_INT(10, 10));

  VALIDATE_PASS(TEST_ASSERT_LESS_THAN_INT(10, 5));
  VALIDATE_FAIL_MSG(TEST_ASSERT_LESS_THAN_INT(5, 10), "Expected 10 to be less than 5");
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_INT(5, 5));

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_INT(5, 10));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_INT(10, 10));
  VALIDATE_FAIL_MSG(TEST_ASSERT_GREATER_OR_EQUAL_INT(10, 5), "Expected 5 to be greater or equal than 10");

  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_INT(10, 5));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_INT(5, 5));
  VALIDATE_FAIL_MSG(TEST_ASSERT_LESS_OR_EQUAL_INT(5, 10), "Expected 10 to be less or equal than 5");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_INT_MESSAGE(10, 20, "custom eq int"), "custom eq int");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_EQUAL_INT_MESSAGE(10, 10, "custom neq int"), "custom neq int");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_GREATER_THAN_INT_MESSAGE(10, 5, "custom gt int"), "custom gt int");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_LESS_THAN_INT_MESSAGE(5, 10, "custom lt int"), "custom lt int");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(10, 5, "custom gte int"), "custom gte int");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_LESS_OR_EQUAL_INT_MESSAGE(5, 10, "custom lte int"), "custom lte int");
}

TEST(suite_uint_comparisons) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT(10, 10));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_UINT(10, 20), "Expected 20 Received 10");

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_UINT(10, 20));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_EQUAL_UINT(10, 10), "Expected 10 to not be equal to 10");

  VALIDATE_PASS(TEST_ASSERT_GREATER_THAN_UINT(5, 10));
  VALIDATE_FAIL_MSG(TEST_ASSERT_GREATER_THAN_UINT(10, 5), "Expected 5 to be greater than 10");
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_UINT(10, 10));

  VALIDATE_PASS(TEST_ASSERT_LESS_THAN_UINT(10, 5));
  VALIDATE_FAIL_MSG(TEST_ASSERT_LESS_THAN_UINT(5, 10), "Expected 10 to be less than 5");
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_UINT(5, 5));

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_UINT(5, 10));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_UINT(10, 10));
  VALIDATE_FAIL_MSG(TEST_ASSERT_GREATER_OR_EQUAL_UINT(10, 5), "Expected 5 to be greater or equal than 10");

  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_UINT(10, 5));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_UINT(5, 5));
  VALIDATE_FAIL_MSG(TEST_ASSERT_LESS_OR_EQUAL_UINT(5, 10), "Expected 10 to be less or equal than 5");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_UINT_MESSAGE(10, 20, "custom eq uint"), "custom eq uint");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_EQUAL_UINT_MESSAGE(10, 10, "custom neq uint"), "custom neq uint");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(10, 5, "custom gt uint"), "custom gt uint");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_LESS_THAN_UINT_MESSAGE(5, 10, "custom lt uint"), "custom lt uint");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_GREATER_OR_EQUAL_UINT_MESSAGE(10, 5, "custom gte uint"), "custom gte uint");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_LESS_OR_EQUAL_UINT_MESSAGE(5, 10, "custom lte uint"), "custom lte uint");
}

TEST(suite_floating_point_numbers) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT(3.14f, 3.14f));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_FLOAT(3.14f, 2.71f), "Expected 2.71000 Received 3.14000");

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_FLOAT(3.14f, 2.71f));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_EQUAL_FLOAT(3.14f, 3.14f), "Expected 3.14000 to not be equal to 3.14000");

  VALIDATE_PASS(TEST_ASSERT_GREATER_THAN_FLOAT(1.0f, 2.0f));
  VALIDATE_FAIL_MSG(TEST_ASSERT_GREATER_THAN_FLOAT(2.0f, 1.0f), "Expected 1.00000 to be greater than 2.00000");
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_FLOAT(2.0f, 2.0f));

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(1.0f, 2.0f));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(2.0f, 2.0f));
  VALIDATE_FAIL_MSG(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(2.0f, 1.0f), "Expected 1.00000 to be greater or equal than 2.00000");

  VALIDATE_PASS(TEST_ASSERT_LESS_THAN_FLOAT(2.0f, 1.0f));
  VALIDATE_FAIL_MSG(TEST_ASSERT_LESS_THAN_FLOAT(1.0f, 2.0f), "Expected 2.00000 to be less than 1.00000");
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_FLOAT(1.0f, 1.0f));

  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(2.0f, 1.0f));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(1.0f, 1.0f));
  VALIDATE_FAIL_MSG(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(1.0f, 2.0f), "Expected 2.00000 to be less or equal than 1.00000");

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(-1.0f, -1.0f));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(-1.0f, -1.0f));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(-2.0f, -1.0f));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(-1.0f, -2.0f));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(-1.0f, -2.0f));
  VALIDATE_FAIL(TEST_ASSERT_LESS_OR_EQUAL_FLOAT(-2.0f, -1.0f));

  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE(3.14159, 3.14159));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_DOUBLE(3.14159, 2.71828), "Expected 2.718280000 Received 3.141590000");

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_DOUBLE(3.14159, 2.71828));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_EQUAL_DOUBLE(3.14159, 3.14159), "Expected 3.141590000 to not be equal to 3.141590000");

  VALIDATE_PASS(TEST_ASSERT_GREATER_THAN_DOUBLE(1.0, 2.0));
  VALIDATE_FAIL_MSG(TEST_ASSERT_GREATER_THAN_DOUBLE(2.0, 1.0), "Expected 1.000000000 to be greater than 2.000000000");
  VALIDATE_FAIL(TEST_ASSERT_GREATER_THAN_DOUBLE(2.0, 2.0));

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(1.0, 2.0));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(2.0, 2.0));
  VALIDATE_FAIL_MSG(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(2.0, 1.0), "Expected 1.000000000 to be greater or equal than 2.000000000");

  VALIDATE_PASS(TEST_ASSERT_LESS_THAN_DOUBLE(2.0, 1.0));
  VALIDATE_FAIL_MSG(TEST_ASSERT_LESS_THAN_DOUBLE(1.0, 2.0), "Expected 2.000000000 to be less than 1.000000000");
  VALIDATE_FAIL(TEST_ASSERT_LESS_THAN_DOUBLE(1.0, 1.0));

  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(2.0, 1.0));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(1.0, 1.0));
  VALIDATE_FAIL_MSG(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(1.0, 2.0), "Expected 2.000000000 to be less or equal than 1.000000000");

  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(-1.0, -1.0));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(-1.0, -1.0));
  VALIDATE_PASS(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(-2.0, -1.0));
  VALIDATE_FAIL(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(-1.0, -2.0));
  VALIDATE_PASS(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(-1.0, -2.0));
  VALIDATE_FAIL(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(-2.0, -1.0));

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.14f, 2.71f, "custom eq float"), "custom eq float");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_EQUAL_FLOAT_MESSAGE(3.14f, 3.14f, "custom neq float"), "custom neq float");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_GREATER_THAN_FLOAT_MESSAGE(2.0f, 1.0f, "custom gt float"), "custom gt float");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_GREATER_OR_EQUAL_FLOAT_MESSAGE(2.0f, 1.0f, "custom gte float"), "custom gte float");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_LESS_THAN_FLOAT_MESSAGE(1.0f, 2.0f, "custom lt float"), "custom lt float");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_LESS_OR_EQUAL_FLOAT_MESSAGE(1.0f, 2.0f, "custom lte float"), "custom lte float");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(3.14159, 2.71828, "custom eq double"), "custom eq double");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_EQUAL_DOUBLE_MESSAGE(3.14159, 3.14159, "custom neq double"), "custom neq double");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_GREATER_THAN_DOUBLE_MESSAGE(2.0, 1.0, "custom gt double"), "custom gt double");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE_MESSAGE(2.0, 1.0, "custom gte double"), "custom gte double");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_LESS_THAN_DOUBLE_MESSAGE(1.0, 2.0, "custom lt double"), "custom lt double");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_LESS_OR_EQUAL_DOUBLE_MESSAGE(1.0, 2.0, "custom lte double"), "custom lte double");
}

TEST(suite_floating_point_zero) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT(0.0f, 0.0f));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE(0.0, 0.0));

  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_EQUAL_FLOAT(0.0f, 0.0f), "Expected 0.00000 to not be equal to 0.00000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_EQUAL_DOUBLE(0.0, 0.0), "Expected 0.000000000 to not be equal to 0.000000000");

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
  for (int i = 0; i < 10; i++)
    accumulated_float += 0.1f;
  float expected_float = 1.0f;

  double accumulated_double = 0.0;
  for (int i = 0; i < 10; i++)
    accumulated_double += 0.1;
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
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING("clut", "different"), "Expected \"different\" to be equal to \"clut\"");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING("clut", NULL), "Expected (null) to be equal to \"clut\"");

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_STRING("clut", "different"));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_EQUAL_STRING("clut", "clut"), "Expected \"clut\" to not be equal to \"clut\"");

  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_LEN("clut_framework", "clut_testing", 5));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_LEN("clut_framework", "clut_testing", 10), "Expected \"clut_framework\" to be equal to \"clut_testing\" until length 10");

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_STRING_LEN("clut_framework", "clut_testing", 10));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_EQUAL_STRING_LEN("clut_framework", "clut_testing", 5), "Expected \"clut_framework\" to not be equal to \"clut_testing\" until length 5");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_STRING_MESSAGE("clut", "different", "custom eq string"), "custom eq string");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_EQUAL_STRING_MESSAGE("clut", "clut", "custom neq string"), "custom neq string");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE("clut_framework", "clut_testing", 10, "custom eq string len"), "custom eq string len");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_EQUAL_STRING_LEN_MESSAGE("clut_framework", "clut_testing", 5, "custom neq string len"), "custom neq string len");
}

TEST(suite_within_comparisons) {
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR(50, 5, 52));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR(50, 5, 55));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR(50, 5, 45));
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_CHAR(50, 5, 56), "Expected '2' (0x32) within +/- '\x05' (0x05) but actual diff was '\x06' (0x06)");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_CHAR(50, 5, 44), "Expected '2' (0x32) within +/- '\x05' (0x05) but actual diff was '\x06' (0x06)");

  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT(100, 10, 105));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT(100, 10, 110));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT(100, 10, 90));
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_INT(100, 10, 111), "Expected 100 within +/- 10 but actual diff was 11");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_INT(100, 10, 89), "Expected 100 within +/- 10 but actual diff was 11");

  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT(1000, 50, 980));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT(1000, 50, 1050));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT(1000, 50, 950));
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_UINT(1000, 50, 1051), "Expected 1000 within +/- 50 but actual diff was 51");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_UINT(1000, 50, 949), "Expected 1000 within +/- 50 but actual diff was 51");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_CHAR_MESSAGE(50, 5, 56, "custom within char"), "custom within char");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_INT_MESSAGE(100, 10, 111, "custom within int"), "custom within int");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_UINT_MESSAGE(1000, 50, 1051, "custom within uint"), "custom within uint");

  float accumulated_float = 0.0f;
  for (int i = 0; i < 10; i++)
    accumulated_float += 0.1f;
  float expected_float = 1.0f;

  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT(expected_float, 0.00001f, accumulated_float));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_FLOAT(expected_float, 0.0000001f, accumulated_float + 0.1f));

  double accumulated_double = 0.0;
  for (int i = 0; i < 10; i++)
    accumulated_double += 0.1;
  double expected_double = 1.0;

  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE(expected_double, 0.00001, accumulated_double));
  VALIDATE_FAIL(TEST_ASSERT_WITHIN_DOUBLE(expected_double, 0.0000001, accumulated_double + 0.1));

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_FLOAT_MESSAGE(expected_float, 0.0000001f, accumulated_float + 0.1f, "custom within float"), "custom within float");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_DOUBLE_MESSAGE(expected_double, 0.0000001, accumulated_double + 0.1, "custom within double"), "custom within double");
}

TEST(suite_memory) {
  typedef struct {
    int x;
    int y;
    char c;
  } DummyStruct;

  DummyStruct s1 = {0}, s2 = {0}, s3 = {0}, s4 = {0};
  s1 = (DummyStruct){10, 20, 'A'};
  s2 = (DummyStruct){10, 20, 'A'};
  s3 = (DummyStruct){10, 99, 'A'};
  s4 = (DummyStruct){10, 20, 'Z'};

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
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_MEMORY(buf1, NULL, sizeof(buf1)), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_MEMORY(NULL, buf2, sizeof(buf2)), "One of the pointers is NULL");
  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(&s1, &s2, 0));

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_MEMORY_MESSAGE(buf1, NULL, sizeof(buf1), "custom memory"), "custom memory");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_MEMORY_ARRAY_MESSAGE(buf1, buf3, sizeof(DummyStruct), 3, "custom memory array"), "custom memory array");
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

  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_end, 8), "Array mismatch at Element [7] Expected 't' (0x74) but was 'X' (0x58)");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_start, 8), "Array mismatch at Element [0] Expected 'C' (0x43) but was 'X' (0x58)");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_mid, 8), "Array mismatch at Element [4] Expected 'T' (0x54) but was 'X' (0x58)");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, NULL, 8), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_CHAR_ARRAY(NULL, act_ok, 8), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE(exp, act_diff_end, 8, "custom char array"), "custom char array");
}

TEST(suite_int_array) {
  int arr1[] = {-10, 20, -30, 40, -50};
  int arr2[] = {-10, 20, -30, 40, -50};
  int arr3[] = {-10, -99, -30, 40, -50};
  int arr4[] = {-10, 20, -30, 40, 99};
  int arr5[] = {99, 20, -30, 40, -50};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr2, 5));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr1, 5));

  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr3, 5), "Array mismatch at Element [1] Expected 20 but was -99");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr4, 5), "Array mismatch at Element [4] Expected -50 but was 99");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr5, 5), "Array mismatch at Element [0] Expected -10 but was 99");

  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr3, 1));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr4, 4));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(&arr1[2], &arr2[2], 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_INT_ARRAY(&arr1[1], &arr3[1], 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr3, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(NULL, NULL, 0));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, NULL, 5), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_INT_ARRAY(NULL, arr2, 5), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(arr1, arr3, 5, "custom int array"), "custom int array");
}

TEST(suite_uint_array) {
  size_t arr1[] = {10, 20, 30, 40, 50};
  size_t arr2[] = {10, 20, 30, 40, 50};
  size_t arr3[] = {10, 99, 30, 40, 50};
  size_t arr4[] = {10, 20, 30, 40, 99};
  size_t arr5[] = {99, 20, 30, 40, 50};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr2, 5));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr1, 5));

  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr3, 5), "Array mismatch at Element [1] Expected 20 but was 99");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr4, 5), "Array mismatch at Element [4] Expected 50 but was 99");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr5, 5), "Array mismatch at Element [0] Expected 10 but was 99");

  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr3, 1));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr4, 4));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(&arr1[2], &arr2[2], 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_UINT_ARRAY(&arr1[1], &arr3[1], 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr3, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(NULL, NULL, 0));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, NULL, 5), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_UINT_ARRAY(NULL, arr2, 5), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_UINT_ARRAY_MESSAGE(arr1, arr3, 5, "custom uint array"), "custom uint array");
}

TEST(suite_float_array) {
  float sum = 0.0f;
  for (int i = 0; i < 10; i++)
    sum += 0.1f;

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

  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_start, 3), "Array mismatch at Element [0] Expected 1.00000 but was 1.10000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_mid, 3), "Array mismatch at Element [1] Expected 5.50000 but was 5.50100");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_end, 3), "Array mismatch at Element [2] Expected 10.12300 but was 10.12400");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_FLOAT_ARRAY(NULL, act_ok, 3), "One of the pointers is NULL");

  float zero_exp[] = {0.0f, 0.0f};
  float zero_act[] = {0.0f, 0.0f};
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(zero_exp, zero_act, 2));

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(exp, act_diff_start, 3, "custom float array"), "custom float array");
}

TEST(suite_double_array) {
  double sum = 0.0;
  for (int i = 0; i < 10000; i++)
    sum += 0.1;

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

  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_start, 3), "Array mismatch at Element [0] Expected 1000.000000000 but was 1000.000010000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_mid, 3), "Array mismatch at Element [1] Expected 0.123456789 but was 0.123456780");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_end, 3), "Array mismatch at Element [2] Expected 99.990000000 but was 100.000000000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(NULL, act_ok, 3), "One of the pointers is NULL");

  double zero_exp[] = {0.0, 0.0};
  double zero_act[] = {0.0, 0.0};
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(zero_exp, zero_act, 2));

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_DOUBLE_ARRAY_MESSAGE(exp, act_diff_start, 3, "custom double array"), "custom double array");
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

  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_start, 3), "Array mismatch at Element [0] Expected \"clut\" but was \"XXXX\"");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_mid, 3), "Array mismatch at Element [1] Expected \"test\" but was \"XXXX\"");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_end, 3), "Array mismatch at Element [2] Expected \"framework\" but was \"XXXXXXXXX\"");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(NULL, act_ok, 3), "One of the pointers is NULL");

  const char *exp_null[] = {"clut", NULL, "framework"};
  const char *act_null[] = {"clut", NULL, "framework"};
  const char *act_no_null[] = {"clut", "test", "framework"};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp_null, act_null, 3));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(exp_null, act_no_null, 3), "Array mismatch at Element [1] Expected (null) but was \"test\"");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(act_no_null, exp_null, 3), "Array mismatch at Element [1] Expected \"test\" but was (null)");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_STRING_ARRAY_MESSAGE(exp, act_diff_start, 3, "custom string array"), "custom string array");
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

  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 5, act_fail, 3), "Array mismatch at Element [2] Expected '\x14' (0x14) within +/- '\x05' (0x05) but actual diff was '\x06' (0x06)");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 0, act_ok, 3), "Array mismatch at Element [0] Expected '2' (0x32) within +/- '\x00' (0x00) but actual diff was '\x02' (0x02)");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 5, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_CHAR_ARRAY(NULL, 5, act_ok, 3), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_CHAR_ARRAY_MESSAGE(exp, 5, act_fail, 3, "custom within char array"), "custom within char array");
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

  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 10, act_fail, 4), "Array mismatch at Element [2] Expected 0 within +/- 10 but actual diff was 11");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 0, act_ok, 4), "Array mismatch at Element [0] Expected 100 within +/- 0 but actual diff was 5");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 10, NULL, 4), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_INT_ARRAY(NULL, 10, act_ok, 4), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_INT_ARRAY_MESSAGE(exp, 10, act_fail, 4, "custom within int array"), "custom within int array");
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

  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 50, act_fail, 3), "Array mismatch at Element [1] Expected 500 within +/- 50 but actual diff was 51");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 0, act_ok, 3), "Array mismatch at Element [0] Expected 1000 within +/- 0 but actual diff was 40");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 50, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_UINT_ARRAY(NULL, 50, act_ok, 3), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_UINT_ARRAY_MESSAGE(exp, 50, act_fail, 3, "custom within uint array"), "custom within uint array");
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

  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.1f, act_fail, 3), "Array mismatch at Element [2] Expected 10.00000 within +/- 0.10000 but actual diff was 0.11000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.0f, act_ok, 3), "Array mismatch at Element [0] Expected 1.00000 within +/- 0.00000 but actual diff was 0.05000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.1f, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_FLOAT_ARRAY(NULL, 0.1f, act_ok, 3), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_FLOAT_ARRAY_MESSAGE(exp, 0.1f, act_fail, 3, "custom within float array"), "custom within float array");
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

  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.1, act_fail, 3), "Array mismatch at Element [2] Expected 10.000000000 within +/- 0.100000000 but actual diff was 0.110000000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.0, act_ok, 3), "Array mismatch at Element [0] Expected 1.000000000 within +/- 0.000000000 but actual diff was 0.050000000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.1, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(NULL, 0.1, act_ok, 3), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_DOUBLE_ARRAY_MESSAGE(exp, 0.1, act_fail, 3, "custom within double array"), "custom within double array");
}

void before_all() {
  clut_sb_init(&output);
  clut_sb_init(&test_message);
}

void after_each() {
  if (Clut.current.failed) {
    clut_dispatch_fail_flush(&output);
  }
}

void after_all() {
  clut_sb_free(&output);
  clut_sb_free(&test_message);
}

int main(void) {
  CLUT_BEFORE_ALL(before_all);
  CLUT_AFTER_EACH(after_each);
  CLUT_AFTER_ALL(after_all);

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
