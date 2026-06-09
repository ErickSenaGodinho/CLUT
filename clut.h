#ifndef INCLUDE_CLUT_H
#define INCLUDE_CLUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef CLUT_OUTPUT_COLOR
#define CLUT_STR_BEGIN_RED_TEXT "\033[31m"
#define CLUT_STR_BEGIN_BLUE_TEXT "\033[34m"
#define CLUT_STR_END_COLOR_TEXT "\033[0m"
#define CLUT_RED_TEXT(text) CLUT_STR_BEGIN_RED_TEXT text CLUT_STR_END_COLOR_TEXT
#define CLUT_GREEN_TEXT(text) "\033[32m" text CLUT_STR_END_COLOR_TEXT
#define CLUT_BLUE_TEXT(text) CLUT_STR_BEGIN_BLUE_TEXT text CLUT_STR_END_COLOR_TEXT
#define CLUT_STR_FAIL CLUT_RED_TEXT("[ FAIL ] ")
#define CLUT_STR_PASSED CLUT_GREEN_TEXT("[ PASS ] ")
#else
#define CLUT_STR_BEGIN_RED_TEXT ""
#define CLUT_STR_BEGIN_BLUE_TEXT ""
#define CLUT_STR_END_COLOR_TEXT ""
#define CLUT_RED_TEXT(text) text
#define CLUT_GREEN_TEXT(text) text
#define CLUT_BLUE_TEXT(text) text
#define CLUT_STR_FAIL "[ FAIL ] "
#define CLUT_STR_PASSED "[ PASS ] "
#endif

#ifndef CLUT_STREAM_DEFAULT
#define CLUT_STREAM_DEFAULT stdout
#endif

#ifndef CLUT_STREAM_FAIL
#define CLUT_STREAM_FAIL stderr
#endif

#ifndef CLUT_FLOAT_EPSILON
#define CLUT_FLOAT_EPSILON 1e-5f
#endif

#ifndef CLUT_DOUBLE_EPSILON
#define CLUT_DOUBLE_EPSILON 1e-9
#endif

#define CLUT_STR_EXPECTED "Expected "
#define CLUT_STR_RECEIVED " Received "
#define CLUT_STR_EQUAL " to be equal to "
#define CLUT_STR_NOT_EQUAL " to not be equal to "
#define CLUT_STR_GREATER_THAN " to be greater than "
#define CLUT_STR_LESS_THAN " to be less than "
#define CLUT_STR_GREATER_OR_EQUAL " to be greater or equal than "
#define CLUT_STR_LESS_OR_EQUAL " to be less or equal than "
#define CLUT_STR_UNTIL_LEN " until length "
#define CLUT_STR_WITHIN_DELTA " within +/- "
#define CLUT_STR_BUT_DIFF " but actual diff was "
#define CLUT_STR_MEMORY_NULL "One of the pointers is NULL"
#define CLUT_STR_MISMATCH_AT "Memory mismatch at byte offset "
#define CLUT_STR_ARRAY_MISMATCH_AT "Array mismatch at Element "
#define CLUT_STR_ARRAY_INDEX_START "["
#define CLUT_STR_ARRAY_INDEX_END "] "
#define CLUT_STR_BYTE_OFFSET ", Byte Offset "
#define CLUT_STR_WAS " but was "

typedef void (*ClutHookFn)();
typedef void (*ClutTestFn)();

typedef struct {
  size_t total_tests;
  size_t passed;
  size_t failures;

  long start_time;

  FILE *stream;
} ClutRunner;

typedef struct {
  ClutHookFn before_all;
  ClutHookFn before_each;
  ClutHookFn after_each;
  ClutHookFn after_all;
} ClutHooks;

typedef struct {
  const char *name;
  long start_time;
  bool failed;
  bool header_printed;
  int param_index;
} ClutTestState;

typedef struct {
  ClutRunner runner;
  ClutHooks hooks;
  ClutTestState current;
} ClutData;

#define TEST(name)                                                                                                                                                                                                                                                 \
  void name(void);                                                                                                                                                                                                                                                 \
  void run_##name(void) { name(); }                                                                                                                                                                                                                                \
  void name(void)

#define PARAM_TEST(name, type, ...)                                                                                                                                                                                                                                \
  type clut_##name##_input_arr[] = __VA_ARGS__;                                                                                                                                                                                                                    \
  void name(type input);                                                                                                                                                                                                                                           \
  void run_##name(void) {                                                                                                                                                                                                                                          \
    size_t n = sizeof(clut_##name##_input_arr) / sizeof(clut_##name##_input_arr[0]);                                                                                                                                                                               \
    bool failed = false;                                                                                                                                                                                                                                           \
    for (size_t i = 0; i < n; ++i) {                                                                                                                                                                                                                               \
      Clut.current.param_index = (int)i;                                                                                                                                                                                                                           \
      name(clut_##name##_input_arr[i]);                                                                                                                                                                                                                            \
      if (Clut.current.failed) {                                                                                                                                                                                                                                   \
        failed = true;                                                                                                                                                                                                                                             \
        Clut.current.failed = false;                                                                                                                                                                                                                               \
      }                                                                                                                                                                                                                                                            \
    }                                                                                                                                                                                                                                                              \
    Clut.current.failed = failed;                                                                                                                                                                                                                                  \
  }                                                                                                                                                                                                                                                                \
  void name(type input)

#define CLUT_BEFORE_ALL(hook_fn) Clut.hooks.before_all = (hook_fn)
#define CLUT_BEFORE_EACH(hook_fn) Clut.hooks.before_each = (hook_fn)
#define CLUT_AFTER_ALL(hook_fn) Clut.hooks.after_all = (hook_fn)
#define CLUT_AFTER_EACH(hook_fn) Clut.hooks.after_each = (hook_fn)

#define TEST_BEGIN() ClutTestBegin()
#define TEST_RUN(test_fn) ClutTestRun((run_##test_fn), #test_fn)
#define TEST_END() ClutTestEnd()

/* Assertions */

#define TEST_ASSERT(condition) ClutTestAssert((condition), __FILE__, __LINE__, "Expression Is False -> " #condition)
#define TEST_ASSERT_TRUE(condition) ClutTestAssert((condition), __FILE__, __LINE__, "Expected True But Was False -> " #condition)
#define TEST_ASSERT_FALSE(condition) ClutTestAssert(!(condition), __FILE__, __LINE__, "Expected False But Was True -> " #condition)
#define TEST_ASSERT_UNLESS(condition) ClutTestAssert(!(condition), __FILE__, __LINE__, "Expression Is True -> " #condition)
#define TEST_ASSERT_NULL(pointer) ClutTestAssert(((pointer) == NULL), __FILE__, __LINE__, "Expected NULL")
#define TEST_ASSERT_NOT_NULL(pointer) ClutTestAssert(((pointer) != NULL), __FILE__, __LINE__, "Expected Non-NULL")

#define TEST_ASSERT_EQUAL_CHAR(expected, actual) ClutTestAssertEqualChar((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT(expected, actual) ClutTestAssertEqualInt((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT(expected, actual) ClutTestAssertEqualUint((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_FLOAT(expected, actual) ClutTestAssertEqualFloat((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_DOUBLE(expected, actual) ClutTestAssertEqualDouble((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING(expected, actual) ClutTestAssertEqualString((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len) ClutTestAssertEqualStringLen((expected), (actual), (len), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_PTR(expected, actual) ClutTestAssertEqualPtr((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_NOT_EQUAL_CHAR(expected, actual) ClutTestAssertNotEqualChar((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_INT(expected, actual) ClutTestAssertNotEqualInt((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_UINT(expected, actual) ClutTestAssertNotEqualUint((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_FLOAT(expected, actual) ClutTestAssertNotEqualFloat((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_DOUBLE(expected, actual) ClutTestAssertNotEqualDouble((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_STRING(expected, actual) ClutTestAssertNotEqualString((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_STRING_LEN(expected, actual, len) ClutTestAssertNotEqualStringLen((expected), (actual), (len), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_PTR(expected, actual) ClutTestAssertNotEqualPtr((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_INT(expected, actual) ClutTestAssertIntGreaterThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT(expected, actual) ClutTestAssertIntLessThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT(expected, actual) ClutTestAssertIntGreaterOrEqual((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT(expected, actual) ClutTestAssertIntLessOrEqual((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_UINT(expected, actual) ClutTestAssertUintGreaterThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT(expected, actual) ClutTestAssertUintLessThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT(expected, actual) ClutTestAssertUintGreaterOrEqual((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT(expected, actual) ClutTestAssertUintLessOrEqual((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_FLOAT(expected, actual) ClutTestAssertFloatGreaterThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(expected, actual) ClutTestAssertFloatGreaterOrEqual((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_FLOAT(expected, actual) ClutTestAssertFloatLessThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_FLOAT(expected, actual) ClutTestAssertFloatLessOrEqual((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_DOUBLE(expected, actual) ClutTestAssertDoubleGreaterThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(expected, actual) ClutTestAssertDoubleGreaterOrEqual((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_DOUBLE(expected, actual) ClutTestAssertDoubleLessThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(expected, actual) ClutTestAssertDoubleLessOrEqual((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_WITHIN_CHAR(expected, delta, actual) ClutTestAssertWithinChar((expected), (delta), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_INT(expected, delta, actual) ClutTestAssertWithinInt((expected), (delta), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_UINT(expected, delta, actual) ClutTestAssertWithinUint((expected), (delta), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_FLOAT(expected, delta, actual) ClutTestAssertWithinFloat((expected), (delta), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_DOUBLE(expected, delta, actual) ClutTestAssertWithinDouble((expected), (delta), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_EQUAL_MEMORY(expected, actual, len) ClutTestAssertEqualMemory((expected), (actual), (len), 1, __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements) ClutTestAssertEqualMemory((expected), (actual), (len), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, actual, num_elements) ClutTestAssertEqualCharArray((expected), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements) ClutTestAssertEqualIntArray((expected), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements) ClutTestAssertEqualUintArray((expected), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements) ClutTestAssertEqualFloatArray((expected), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, num_elements) ClutTestAssertEqualDoubleArray((expected), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements) ClutTestAssertEqualStringArray((expected), (actual), (num_elements), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_WITHIN_CHAR_ARRAY(expected, delta, actual, num_elements) ClutTestAssertWithinCharArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_INT_ARRAY(expected, delta, actual, num_elements) ClutTestAssertWithinIntArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_UINT_ARRAY(expected, delta, actual, num_elements) ClutTestAssertWithinUintArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_FLOAT_ARRAY(expected, delta, actual, num_elements) ClutTestAssertWithinFloatArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_DOUBLE_ARRAY(expected, delta, actual, num_elements) ClutTestAssertWithinDoubleArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, NULL)

/* Messages */

#define TEST_ASSERT_MESSAGE(condition, mgs) ClutTestAssert((condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_TRUE_MESSAGE(condition, msg) ClutTestAssert((condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_FALSE_MESSAGE(condition, msg) ClutTestAssert(!(condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_UNLESS_MESSAGE(condition, mgs) ClutTestAssert(!(condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NULL_MESSAGE(pointer, msg) ClutTestAssert(((pointer) == NULL), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_NULL_MESSAGE(pointer, msg) ClutTestAssert(((pointer) != NULL), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_EQUAL_CHAR_MESSAGE(expected, actual, msg) ClutTestAssertEqualChar((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, msg) ClutTestAssertEqualInt((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_UINT_MESSAGE(expected, actual, msg) ClutTestAssertEqualUint((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected, actual, msg) ClutTestAssertEqualFloat((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(expected, actual, msg) ClutTestAssertEqualDouble((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, msg) ClutTestAssertEqualString((const char *)(expected), (const char *)(actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(expected, actual, len, msg) ClutTestAssertEqualStringLen((expected), (actual), (len), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_NOT_EQUAL_CHAR_MESSAGE(expected, actual, msg) ClutTestAssertNotEqualChar((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_INT_MESSAGE(expected, actual, msg) ClutTestAssertEqualInt((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_UINT_MESSAGE(expected, actual, msg) ClutTestAssertNotEqualUint((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_FLOAT_MESSAGE(expected, actual, msg) ClutTestAssertNotEqualFloat((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_DOUBLE_MESSAGE(expected, actual, msg) ClutTestAssertNotEqualDouble((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_STRING_MESSAGE(expected, actual, msg) ClutTestAssertNotEqualString((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_STRING_LEN_MESSAGE(expected, actual, len, msg) ClutTestAssertNotEqualStringLen((expected), (actual), (len), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_PTR_MESSAGE(expected, actual, msg) ClutTestAssertNotEqualPtr((expected), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_INT_MESSAGE(expected, actual, msg) ClutTestAssertIntGreaterThan((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_INT_MESSAGE(expected, actual, msg) ClutTestAssertIntLessThan((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(expected, actual, msg) ClutTestAssertIntGreaterOrEqual((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_INT_MESSAGE(expected, actual, msg) ClutTestAssertIntLessOrEqual((expected), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(expected, actual, msg) ClutTestAssertUintGreaterThan((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_UINT_MESSAGE(expected, actual, msg) ClutTestAssertUintLessThan((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT_MESSAGE(expected, actual, msg) ClutTestAssertUintGreaterOrEqual((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT_MESSAGE(expected, actual, msg) ClutTestAssertUintLessOrEqual((expected), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_FLOAT_MESSAGE(expected, actual, msg) ClutTestAssertFloatGreaterThan((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_FLOAT_MESSAGE(expected, actual, msg) ClutTestAssertFloatGreaterOrEqual((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_FLOAT_MESSAGE(expected, actual, msg) ClutTestAssertFloatLessThan((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_FLOAT_MESSAGE(expected, actual, msg) ClutTestAssertFloatLessOrEqual((expected), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_DOUBLE_MESSAGE(expected, actual, msg) ClutTestAssertDoubleGreaterThan((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE_MESSAGE(expected, actual, msg) ClutTestAssertDoubleGreaterOrEqual((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_DOUBLE_MESSAGE(expected, actual, msg) ClutTestAssertDoubleLessThan((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_DOUBLE_MESSAGE(expected, actual, msg) ClutTestAssertDoubleLessOrEqual((expected), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_WITHIN_CHAR_MESSAGE(expected, delta, actual, msg) ClutTestAssertWithinChar((expected), (delta), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_INT_MESSAGE(expected, delta, actual, msg) ClutTestAssertWithinInt((expected), (delta), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_UINT_MESSAGE(expected, delta, actual, msg) ClutTestAssertWithinUint((expected), (delta), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_FLOAT_MESSAGE(expected, delta, actual, msg) ClutTestAssertWithinFloat((expected), (delta), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_DOUBLE_MESSAGE(expected, delta, actual, msg) ClutTestAssertWithinDouble((expected), (delta), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_EQUAL_MEMORY_MESSAGE(expected, actual, len, num_elements, msg) ClutTestAssertEqualMemory((expected), (actual), (len), 1, __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_MEMORY_ARRAY_MESSAGE(expected, actual, len, num_elements, msg) ClutTestAssertEqualMemory((expected), (actual), (len), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE(expected, actual, num_elements, msg) ClutTestAssertEqualCharArray((expected), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, num_elements, msg) ClutTestAssertEqualIntArray((expected), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_UINT_ARRAY_MESSAGE(expected, actual, num_elements, msg) ClutTestAssertEqualUintArray((expected), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(expected, actual, num_elements, msg) ClutTestAssertEqualFloatArray((expected), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_DOUBLE_ARRAY_MESSAGE(expected, actual, num_elements, msg) ClutTestAssertEqualDoubleArray((expected), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_STRING_ARRAY_MESSAGE(expected, actual, num_elements, msg) ClutTestAssertEqualStringArray((expected), (actual), (num_elements), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_WITHIN_CHAR_ARRAY_MESSAGE(expected, delta, actual, num_elements, msg) ClutTestAssertWithinCharArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_INT_ARRAY_MESSAGE(expected, delta, actual, num_elements, msg) ClutTestAssertWithinIntArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_UINT_ARRAY_MESSAGE(expected, delta, actual, num_elements, msg) ClutTestAssertWithinUintArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_FLOAT_ARRAY_MESSAGE(expected, delta, actual, num_elements, msg) ClutTestAssertWithinFloatArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_DOUBLE_ARRAY_MESSAGE(expected, delta, actual, num_elements, msg) ClutTestAssertWithinDoubleArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, (msg))

void ClutReset();
void ClutTestReset();
void ClutTestBegin();
void ClutTestRun(ClutTestFn test_fn, const char *test_name);
int ClutTestEnd();

void ClutTestAssert(bool condition, const char *file, const int line, const char *msg);
void ClutTestAssertEqualChar(char expected, char actual, const char *file, const int line, const char *msg);
void ClutTestAssertEqualInt(int expected, int actual, const char *file, const int line, const char *msg);
void ClutTestAssertEqualUint(size_t expected, size_t actual, const char *file, const int line, const char *msg);
void ClutTestAssertEqualFloat(float expected, float actual, const char *file, const int line, const char *msg);
void ClutTestAssertEqualDouble(double expected, double actual, const char *file, const int line, const char *msg);
void ClutTestAssertEqualString(const char *expected, const char *actual, const char *file, const int line, const char *msg);
void ClutTestAssertEqualStringLen(const char *expected, const char *actual, size_t len, const char *file, const int line, const char *msg);
void ClutTestAssertEqualPtr(void *expected, void *actual, const char *file, const int line, const char *msg);

void ClutTestAssertNotEqualChar(char expected, char actual, const char *file, const int line, const char *msg);
void ClutTestAssertNotEqualInt(int expected, int actual, const char *file, const int line, const char *msg);
void ClutTestAssertNotEqualUint(size_t expected, size_t actual, const char *file, const int line, const char *msg);
void ClutTestAssertNotEqualFloat(float expected, float actual, const char *file, const int line, const char *msg);
void ClutTestAssertNotEqualDouble(double expected, double actual, const char *file, const int line, const char *msg);
void ClutTestAssertNotEqualString(const char *expected, const char *actual, const char *file, const int line, const char *msg);
void ClutTestAssertNotEqualStringLen(const char *expected, const char *actual, size_t len, const char *file, const int line, const char *msg);
void ClutTestAssertNotEqualPtr(void *expected, void *actual, const char *file, const int line, const char *msg);

void ClutTestAssertIntGreaterThan(int expected, int actual, const char *file, int line, const char *msg);
void ClutTestAssertIntGreaterOrEqual(int expected, int actual, const char *file, int line, const char *msg);
void ClutTestAssertIntLessThan(int expected, int actual, const char *file, int line, const char *msg);
void ClutTestAssertIntLessOrEqual(int expected, int actual, const char *file, int line, const char *msg);

void ClutTestAssertUintGreaterThan(size_t expected, size_t actual, const char *file, int line, const char *msg);
void ClutTestAssertUintGreaterOrEqual(size_t expected, size_t actual, const char *file, int line, const char *msg);
void ClutTestAssertUintLessThan(size_t expected, size_t actual, const char *file, int line, const char *msg);
void ClutTestAssertUintLessOrEqual(size_t expected, size_t actual, const char *file, int line, const char *msg);

void ClutTestAssertFloatGreaterThan(float expected, float actual, const char *file, int line, const char *msg);
void ClutTestAssertFloatGreaterOrEqual(float expected, float actual, const char *file, int line, const char *msg);
void ClutTestAssertFloatLessThan(float expected, float actual, const char *file, int line, const char *msg);
void ClutTestAssertFloatLessOrEqual(float expected, float actual, const char *file, int line, const char *msg);

void ClutTestAssertDoubleGreaterThan(double expected, double actual, const char *file, int line, const char *msg);
void ClutTestAssertDoubleGreaterOrEqual(double expected, double actual, const char *file, int line, const char *msg);
void ClutTestAssertDoubleLessThan(double expected, double actual, const char *file, int line, const char *msg);
void ClutTestAssertDoubleLessOrEqual(double expected, double actual, const char *file, int line, const char *msg);

void ClutTestAssertWithinChar(char expected, char delta, char actual, const char *file, const int line, const char *msg);
void ClutTestAssertWithinInt(int expected, int delta, int actual, const char *file, const int line, const char *msg);
void ClutTestAssertWithinUint(size_t expected, size_t delta, size_t actual, const char *file, const int line, const char *msg);
void ClutTestAssertWithinFloat(float expected, float delta, float actual, const char *file, const int line, const char *msg);
void ClutTestAssertWithinDouble(double expected, double delta, double actual, const char *file, const int line, const char *msg);

void ClutTestAssertEqualMemory(const void *expected, const void *actual, size_t len, size_t num_elements, const char *file, const int line, const char *msg);
void ClutTestAssertEqualCharArray(const char *expected, const char *actual, size_t num_elements, const char *file, const int line, const char *msg);
void ClutTestAssertEqualIntArray(const int *expected, const int *actual, size_t num_elements, const char *file, const int line, const char *msg);
void ClutTestAssertEqualUintArray(const size_t *expected, const size_t *actual, size_t num_elements, const char *file, const int line, const char *msg);
void ClutTestAssertEqualFloatArray(const float *expected, const float *actual, size_t num_elements, const char *file, const int line, const char *msg);
void ClutTestAssertEqualDoubleArray(const double *expected, const double *actual, size_t num_elements, const char *file, const int line, const char *msg);
void ClutTestAssertEqualStringArray(const char **expected, const char **actual, size_t num_elements, const char *file, const int line, const char *msg);

void ClutTestAssertWithinCharArray(const char *expected, char delta, const char *actual, size_t num_elements, const char *file, const int line, const char *msg);
void ClutTestAssertWithinIntArray(const int *expected, int delta, const int *actual, size_t num_elements, const char *file, const int line, const char *msg);
void ClutTestAssertWithinUintArray(const size_t *expected, size_t delta, const size_t *actual, size_t num_elements, const char *file, const int line, const char *msg);
void ClutTestAssertWithinFloatArray(const float *expected, float delta, const float *actual, size_t num_elements, const char *file, const int line, const char *msg);
void ClutTestAssertWithinDoubleArray(const double *expected, double delta, const double *actual, size_t num_elements, const char *file, const int line, const char *msg);

#ifdef CLUT_IMPLEMENTATION

#include <stdarg.h>
#include <string.h>
#include <time.h>

#define RETURN_IF_FAILED                                                                                                                                                                                                                                           \
  do {                                                                                                                                                                                                                                                             \
    if (Clut.current.failed)                                                                                                                                                                                                                                       \
      return;                                                                                                                                                                                                                                                      \
  } while (0)

#define CLUT_START_FAILURE_LOG(file, line, msg)                                                                                                                                                                                                                    \
  do {                                                                                                                                                                                                                                                             \
    double end_time = ClutElapsedSeconds(Clut.current.start_time);                                                                                                                                                                                                 \
    ClutFail();                                                                                                                                                                                                                                                    \
    if (!Clut.current.header_printed) {                                                                                                                                                                                                                            \
      Clut.current.header_printed = true;                                                                                                                                                                                                                          \
      ClutPrint(CLUT_STR_FAIL);                                                                                                                                                                                                                                    \
      ClutPrint(CLUT_STR_BEGIN_RED_TEXT);                                                                                                                                                                                                                          \
      ClutPrintTestNameWithTime(end_time);                                                                                                                                                                                                                         \
    }                                                                                                                                                                                                                                                              \
    ClutPrint(CLUT_STR_BEGIN_RED_TEXT);                                                                                                                                                                                                                            \
    ClutPrintTestLocation(file, line);                                                                                                                                                                                                                             \
    if (msg) {                                                                                                                                                                                                                                                     \
      ClutPrint(msg);                                                                                                                                                                                                                                              \
    } else {

#define CLUT_END_FAILURE_LOG()                                                                                                                                                                                                                                     \
  }                                                                                                                                                                                                                                                                \
  ClutPrint(CLUT_STR_END_COLOR_TEXT);                                                                                                                                                                                                                              \
  ClutPrintChar('\n');                                                                                                                                                                                                                                             \
  }                                                                                                                                                                                                                                                                \
  while (0)

#define CHECK_MEMORY_PRECONDITIONS                                                                                                                                                                                                                                 \
  do {                                                                                                                                                                                                                                                             \
    if (num_elements == 0)                                                                                                                                                                                                                                         \
      return;                                                                                                                                                                                                                                                      \
                                                                                                                                                                                                                                                                   \
    if (expected == actual)                                                                                                                                                                                                                                        \
      return;                                                                                                                                                                                                                                                      \
                                                                                                                                                                                                                                                                   \
    if (expected == NULL || actual == NULL) {                                                                                                                                                                                                                      \
      CLUT_START_FAILURE_LOG(file, line, msg);                                                                                                                                                                                                                     \
      ClutPrint(CLUT_STR_MEMORY_NULL);                                                                                                                                                                                                                             \
      CLUT_END_FAILURE_LOG();                                                                                                                                                                                                                                      \
      return;                                                                                                                                                                                                                                                      \
    }                                                                                                                                                                                                                                                              \
  } while (0)

static inline float clut_fabsf(float x) { return x < 0.0f ? -x : x; }
static inline double clut_fabs(double x) { return x < 0.0 ? -x : x; }

static inline bool clut_float_equal(float expected, float actual) {
  if (expected == 0.0f)
    return actual == 0.0f;
  return clut_fabsf(expected - actual) < clut_fabsf(expected) * CLUT_FLOAT_EPSILON;
}

static inline bool clut_double_equal(double expected, double actual) {
  if (expected == 0.0)
    return actual == 0.0;
  return clut_fabs(expected - actual) < clut_fabs(expected) * CLUT_DOUBLE_EPSILON;
}

static inline double ClutElapsedSeconds(long start) { return ((double)(clock() - start)) / CLOCKS_PER_SEC; }

ClutData Clut = {};

void ClutPrint(const char *str) {
  if (str == NULL) {
    str = "(null)";
  }
  fprintf(Clut.runner.stream, "%s", str);
}
void ClutPrintChar(const char c) { fprintf(Clut.runner.stream, "%c", c); }
void ClutPrintInt(int number) { fprintf(Clut.runner.stream, "%d", number); }
void ClutPrintUint(size_t number) { fprintf(Clut.runner.stream, "%zu", number); }
void ClutPrintFloat(float number) { fprintf(Clut.runner.stream, "%.5f", number); }
void ClutPrintDouble(double number) { fprintf(Clut.runner.stream, "%.9f", number); }
void ClutPrintString(const char *str) { fprintf(Clut.runner.stream, "\"%s\"", str); }
void ClutPrintPtr(void *ptr) { fprintf(Clut.runner.stream, "%p", ptr); }
void ClutPrintHex(int value) { fprintf(Clut.runner.stream, "0x%02X", value); }
void ClutPrintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(Clut.runner.stream, fmt, args);
  va_end(args);
}

void ClutPrintTime(double time) { fprintf(Clut.runner.stream, "%.3fs", time); }

void ClutPrintTestNameWithTime(double end_time) {
  ClutPrintf("%-50s", Clut.current.name);
  ClutPrintTime(end_time);
  ClutPrintChar('\n');
}

void ClutPrintTestLocation(const char *file, const int line) {
  ClutPrint(file);
  ClutPrintChar(':');
  ClutPrintInt(line);
  ClutPrintChar(':');
  ClutPrint(Clut.current.name);
  if (Clut.current.param_index >= 0) {
    ClutPrintf("[%d]", Clut.current.param_index);
  }
  ClutPrintChar(':');
}

void ClutPrintExpectedActualChar(char expected, char actual, const char *opStr) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintf("'%c' (", expected);
  ClutPrintHex((unsigned char)expected);
  ClutPrintf(") %s '%c' (", opStr, actual);
  ClutPrintHex((unsigned char)actual);
  ClutPrintChar(')');
}

void ClutPrintExpectedActualInt(int expected, int actual, const char *opStr) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintInt(expected);
  ClutPrint(opStr);
  ClutPrintInt(actual);
}

void ClutPrintExpectedActualUint(size_t expected, size_t actual, const char *opStr) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintUint(expected);
  ClutPrint(opStr);
  ClutPrintUint(actual);
}

void ClutPrintExpectedActualFloat(float expected, float actual, const char *opStr) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintFloat(expected);
  ClutPrint(opStr);
  ClutPrintFloat(actual);
}

void ClutPrintExpectedActualDouble(double expected, double actual, const char *opStr) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintDouble(expected);
  ClutPrint(opStr);
  ClutPrintDouble(actual);
}

void ClutPrintExpectedActualString(const char *expected, const char *actual, const char *opStr) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintString(expected);
  ClutPrint(opStr);
  ClutPrintString(actual);
}

void ClutPrintExpectedActualStringLen(const char *expected, const char *actual, size_t len, const char *opSrt) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintString(expected);
  ClutPrint(opSrt);
  ClutPrintString(actual);
  ClutPrint(CLUT_STR_UNTIL_LEN);
  ClutPrintInt(len);
}

void ClutPrintExpectedActualPtr(void *expected, void *actual, const char *opStr) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintPtr(expected);
  ClutPrint(opStr);
  ClutPrintPtr(actual);
}

void ClutPrintExpectedActualHex(int expected, int actual, const char *opStr) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintHex(expected);
  ClutPrint(opStr);
  ClutPrintHex(actual);
}

void ClutPrintWithinDiffChar(char expected, char delta, char diff) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintChar(expected);
  ClutPrint(CLUT_STR_WITHIN_DELTA);
  ClutPrintChar(delta);
  ClutPrint(CLUT_STR_BUT_DIFF);
  ClutPrintChar(diff);
}

void ClutPrintWithinDiffInt(int expected, int delta, int diff) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintInt(expected);
  ClutPrint(CLUT_STR_WITHIN_DELTA);
  ClutPrintInt(delta);
  ClutPrint(CLUT_STR_BUT_DIFF);
  ClutPrintInt(diff);
}

void ClutPrintWithinDiffUint(size_t expected, size_t delta, size_t diff) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintUint(expected);
  ClutPrint(CLUT_STR_WITHIN_DELTA);
  ClutPrintUint(delta);
  ClutPrint(CLUT_STR_BUT_DIFF);
  ClutPrintUint(diff);
}

void ClutPrintWithinDiffFloat(float expected, float delta, float diff) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintFloat(expected);
  ClutPrint(CLUT_STR_WITHIN_DELTA);
  ClutPrintFloat(delta);
  ClutPrint(CLUT_STR_BUT_DIFF);
  ClutPrintFloat(diff);
}

void ClutPrintWithinDiffDouble(double expected, double delta, double diff) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintDouble(expected);
  ClutPrint(CLUT_STR_WITHIN_DELTA);
  ClutPrintDouble(delta);
  ClutPrint(CLUT_STR_BUT_DIFF);
  ClutPrintDouble(diff);
}

void ClutPrintMismatchArray(size_t index) {
  ClutPrint(CLUT_STR_ARRAY_MISMATCH_AT);
  ClutPrint(CLUT_STR_ARRAY_INDEX_START);
  ClutPrintUint(index);
  ClutPrint(CLUT_STR_ARRAY_INDEX_END);
}

void ClutFail() {
  Clut.runner.stream = CLUT_STREAM_FAIL;
  Clut.current.failed = true;
}

void ClutAssertCompareChar(bool condition, char expected, char actual, const char *file, const int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;

  if (condition)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualChar(actual, expected, opStr);
  CLUT_END_FAILURE_LOG();
}

void ClutAssertCompareInt(bool condition, int expected, int actual, const char *file, const int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;

  if (condition)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualInt(actual, expected, opStr);
  CLUT_END_FAILURE_LOG();
}

void ClutAssertCompareUint(bool condition, size_t expected, size_t actual, const char *file, const int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;

  if (condition)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualUint(actual, expected, opStr);
  CLUT_END_FAILURE_LOG();
}

void ClutAssertCompareFloat(bool condition, float expected, float actual, const char *file, const int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;

  if (condition)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualFloat(actual, expected, opStr);
  CLUT_END_FAILURE_LOG();
}

void ClutAssertCompareDouble(bool condition, double expected, double actual, const char *file, const int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;

  if (condition)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualDouble(actual, expected, opStr);
  CLUT_END_FAILURE_LOG();
}

void ClutReset() {
  Clut.runner.total_tests = 0;
  Clut.runner.failures = 0;
  Clut.runner.start_time = 0;
  Clut.runner.stream = CLUT_STREAM_DEFAULT;
  ClutTestReset();
}

void ClutTestReset() {
  Clut.current.name = NULL;
  Clut.current.failed = false;
  Clut.current.header_printed = false;
  Clut.current.param_index = -1;
}

void ClutTestBegin() {
  ClutReset();
  if (Clut.hooks.before_all)
    Clut.hooks.before_all();
  Clut.runner.start_time = clock();
}

void ClutTestRun(ClutTestFn run_test_fn, const char *test_name) {
  ClutTestReset();
  Clut.runner.total_tests++;
  if (Clut.hooks.before_each)
    Clut.hooks.before_each();
  Clut.current.name = test_name;
  Clut.current.start_time = clock();
  run_test_fn();
  if (Clut.current.failed) {
    Clut.runner.failures++;
  } else {
    double end_time = ClutElapsedSeconds(Clut.current.start_time);
    ClutPrint(CLUT_STR_PASSED);
    ClutPrintTestNameWithTime(end_time);
    Clut.runner.passed++;
  }
  if (Clut.hooks.after_each)
    Clut.hooks.after_each();
}

int ClutTestEnd() {
  double total_time = ClutElapsedSeconds(Clut.runner.start_time);
  if (Clut.hooks.after_all)
    Clut.hooks.after_all();
  if (Clut.runner.total_tests == 0)
    return Clut.runner.failures;

  size_t total_tests = Clut.runner.total_tests;
  size_t failures = Clut.runner.failures;
  size_t passed = Clut.runner.passed;

  ClutReset();
  ClutPrint("--------------------------------\n");
  ClutPrintf(CLUT_BLUE_TEXT("Tests run:  ") CLUT_GREEN_TEXT("%zu\n"), total_tests);
  ClutPrintf(CLUT_BLUE_TEXT("Passed:     ") CLUT_GREEN_TEXT("%zu\n"), passed);
  ClutPrintf(CLUT_BLUE_TEXT("Failed:     ") CLUT_RED_TEXT("%zu\n"), failures);
  ClutPrint("--------------------------------\n");
  ClutPrintf(CLUT_BLUE_TEXT("Total time: %.3fs\n"), total_time);
  ClutPrint("\n");
  return failures;
}

void ClutTestAssert(bool condition, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (!condition) {
    CLUT_START_FAILURE_LOG(file, line, msg);
    CLUT_END_FAILURE_LOG();
  }
}

void ClutTestAssertEqualChar(char expected, char actual, const char *file, const int line, const char *msg) { ClutAssertCompareChar(expected == actual, expected, actual, file, line, msg, CLUT_STR_RECEIVED); }
void ClutTestAssertEqualInt(int expected, int actual, const char *file, const int line, const char *msg) { ClutAssertCompareInt(expected == actual, expected, actual, file, line, msg, CLUT_STR_RECEIVED); }
void ClutTestAssertEqualUint(size_t expected, size_t actual, const char *file, const int line, const char *msg) { ClutAssertCompareUint(expected == actual, expected, actual, file, line, msg, CLUT_STR_RECEIVED); }
void ClutTestAssertEqualFloat(float expected, float actual, const char *file, const int line, const char *msg) { ClutAssertCompareFloat(clut_float_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_RECEIVED); }
void ClutTestAssertEqualDouble(double expected, double actual, const char *file, const int line, const char *msg) { ClutAssertCompareDouble(clut_double_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_RECEIVED); }

void ClutTestAssertEqualString(const char *expected, const char *actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected == actual)
    return;

  if (expected == NULL || actual == NULL) {
    Clut.current.failed = true;
  } else {
    for (size_t i = 0; expected[i] || actual[i]; i++) {
      if (expected[i] != actual[i]) {
        Clut.current.failed = true;
        break;
      }
    }
  }

  if (!Clut.current.failed)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualString(actual, expected, CLUT_STR_EQUAL);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertEqualStringLen(const char *expected, const char *actual, size_t len, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected == actual)
    return;

  if (expected == NULL || actual == NULL) {
    Clut.current.failed = true;
  } else {
    for (size_t i = 0; i < len && (expected[i] || actual[i]); i++) {
      if (expected[i] != actual[i]) {
        Clut.current.failed = true;
        break;
      }
    }
  }

  if (!Clut.current.failed)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualStringLen(actual, expected, len, CLUT_STR_EQUAL);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertEqualPtr(void *expected, void *actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected == actual)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualPtr(expected, actual, CLUT_STR_RECEIVED);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertNotEqualChar(char expected, char actual, const char *file, const int line, const char *msg) { ClutAssertCompareChar(expected != actual, expected, actual, file, line, msg, CLUT_STR_NOT_EQUAL); }
void ClutTestAssertNotEqualInt(int expected, int actual, const char *file, const int line, const char *msg) { ClutAssertCompareInt(expected != actual, expected, actual, file, line, msg, CLUT_STR_NOT_EQUAL); }
void ClutTestAssertNotEqualUint(size_t expected, size_t actual, const char *file, const int line, const char *msg) { ClutAssertCompareUint(expected != actual, expected, actual, file, line, msg, CLUT_STR_NOT_EQUAL); }
void ClutTestAssertNotEqualFloat(float expected, float actual, const char *file, const int line, const char *msg) { ClutAssertCompareFloat(!clut_float_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_NOT_EQUAL); }
void ClutTestAssertNotEqualDouble(double expected, double actual, const char *file, const int line, const char *msg) { ClutAssertCompareDouble(!clut_double_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_NOT_EQUAL); }

void ClutTestAssertNotEqualString(const char *expected, const char *actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  bool are_equal = true;

  if (expected == actual) {
    are_equal = true;
  } else if (expected == NULL || actual == NULL) {
    are_equal = false;
  } else {
    for (size_t i = 0; expected[i] || actual[i]; i++) {
      if (expected[i] != actual[i]) {
        are_equal = false;
        break;
      }
    }
  }

  if (!are_equal)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualString(actual, expected, CLUT_STR_NOT_EQUAL);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertNotEqualStringLen(const char *expected, const char *actual, size_t len, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  bool are_equal = true;

  if (expected == actual) {
    are_equal = true;
  } else if (expected == NULL || actual == NULL) {
    are_equal = false;
  } else {
    for (size_t i = 0; i < len && (expected[i] || actual[i]); i++) {
      if (expected[i] != actual[i]) {
        are_equal = false;
        break;
      }
    }
  }

  if (!are_equal)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualStringLen(actual, expected, len, CLUT_STR_NOT_EQUAL);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertNotEqualPtr(void *expected, void *actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected != actual)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualPtr(actual, expected, CLUT_STR_NOT_EQUAL);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertIntGreaterThan(int expected, int actual, const char *file, int line, const char *msg) { ClutAssertCompareInt(actual > expected, expected, actual, file, line, msg, CLUT_STR_GREATER_THAN); }
void ClutTestAssertIntGreaterOrEqual(int expected, int actual, const char *file, int line, const char *msg) { ClutAssertCompareInt(actual >= expected, expected, actual, file, line, msg, CLUT_STR_GREATER_OR_EQUAL); }
void ClutTestAssertIntLessThan(int expected, int actual, const char *file, int line, const char *msg) { ClutAssertCompareInt(actual < expected, expected, actual, file, line, msg, CLUT_STR_LESS_THAN); }
void ClutTestAssertIntLessOrEqual(int expected, int actual, const char *file, int line, const char *msg) { ClutAssertCompareInt(actual <= expected, expected, actual, file, line, msg, CLUT_STR_LESS_OR_EQUAL); }

void ClutTestAssertUintGreaterThan(size_t expected, size_t actual, const char *file, int line, const char *msg) { ClutAssertCompareUint(actual > expected, expected, actual, file, line, msg, CLUT_STR_GREATER_THAN); }
void ClutTestAssertUintGreaterOrEqual(size_t expected, size_t actual, const char *file, int line, const char *msg) { ClutAssertCompareUint(actual >= expected, expected, actual, file, line, msg, CLUT_STR_GREATER_OR_EQUAL); }
void ClutTestAssertUintLessThan(size_t expected, size_t actual, const char *file, int line, const char *msg) { ClutAssertCompareUint(actual < expected, expected, actual, file, line, msg, CLUT_STR_LESS_THAN); }
void ClutTestAssertUintLessOrEqual(size_t expected, size_t actual, const char *file, int line, const char *msg) { ClutAssertCompareUint(actual <= expected, expected, actual, file, line, msg, CLUT_STR_LESS_OR_EQUAL); }

void ClutTestAssertFloatGreaterThan(float expected, float actual, const char *file, int line, const char *msg) { ClutAssertCompareFloat(actual > expected, expected, actual, file, line, msg, CLUT_STR_GREATER_THAN); }
void ClutTestAssertFloatGreaterOrEqual(float expected, float actual, const char *file, int line, const char *msg) { ClutAssertCompareFloat(actual > expected || clut_float_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_GREATER_OR_EQUAL); }
void ClutTestAssertFloatLessThan(float expected, float actual, const char *file, int line, const char *msg) { ClutAssertCompareFloat(actual < expected, expected, actual, file, line, msg, CLUT_STR_LESS_THAN); }
void ClutTestAssertFloatLessOrEqual(float expected, float actual, const char *file, int line, const char *msg) { ClutAssertCompareFloat(actual < expected || clut_float_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_LESS_OR_EQUAL); }

void ClutTestAssertDoubleGreaterThan(double expected, double actual, const char *file, int line, const char *msg) { ClutAssertCompareDouble(actual > expected, expected, actual, file, line, msg, CLUT_STR_GREATER_THAN); }
void ClutTestAssertDoubleGreaterOrEqual(double expected, double actual, const char *file, int line, const char *msg) {
  ClutAssertCompareDouble(actual > expected || clut_double_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_GREATER_OR_EQUAL);
}
void ClutTestAssertDoubleLessThan(double expected, double actual, const char *file, int line, const char *msg) { ClutAssertCompareDouble(actual < expected, expected, actual, file, line, msg, CLUT_STR_LESS_THAN); }
void ClutTestAssertDoubleLessOrEqual(double expected, double actual, const char *file, int line, const char *msg) { ClutAssertCompareDouble(actual < expected || clut_double_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_LESS_OR_EQUAL); }

void ClutTestAssertWithinChar(char expected, char delta, char actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  char diff = (actual > expected) ? (actual - expected) : (expected - actual);

  if (diff <= delta)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintWithinDiffChar(expected, delta, diff);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertWithinInt(int expected, int delta, int actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  int diff = (actual > expected) ? (actual - expected) : (expected - actual);

  if (diff <= delta)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintWithinDiffInt(expected, delta, diff);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertWithinUint(size_t expected, size_t delta, size_t actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  size_t diff = (actual > expected) ? (actual - expected) : (expected - actual);

  if (diff <= delta)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintWithinDiffUint(expected, delta, diff);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertWithinFloat(float expected, float delta, float actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  float diff = clut_fabsf(actual - expected);

  if (diff <= delta)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintWithinDiffFloat(expected, delta, diff);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertWithinDouble(double expected, double delta, double actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  double diff = clut_fabs(actual - expected);

  if (diff <= delta)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintWithinDiffDouble(expected, delta, diff);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertEqualMemory(const void *expected, const void *actual, size_t len, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  CHECK_MEMORY_PRECONDITIONS;

  if (len == 0)
    return;

  const unsigned char *ptr_exp = (const unsigned char *)expected;
  const unsigned char *ptr_act = (const unsigned char *)actual;

  for (size_t index = 0; index < num_elements; index++) {
    for (size_t byte = 0; byte < len; byte++) {
      size_t offset = (index * len) + byte;

      if (ptr_exp[offset] != ptr_act[offset]) {
        CLUT_START_FAILURE_LOG(file, line, msg);

        if (num_elements > 1) {
          ClutPrintMismatchArray(index);
          ClutPrint(CLUT_STR_BYTE_OFFSET);
          ClutPrint(CLUT_STR_ARRAY_INDEX_START);
          ClutPrintUint(byte);
          ClutPrint(CLUT_STR_ARRAY_INDEX_END);
        } else {
          ClutPrint(CLUT_STR_MISMATCH_AT);
          ClutPrintUint(byte);
        }

        ClutPrintExpectedActualHex(ptr_exp[offset], ptr_act[offset], CLUT_STR_WAS);

        CLUT_END_FAILURE_LOG();
        return;
      }
    }
  }
}

void ClutTestAssertEqualCharArray(const char *expected, const char *actual, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  CHECK_MEMORY_PRECONDITIONS;

  const unsigned char *ptr_exp = (const unsigned char *)expected;
  const unsigned char *ptr_act = (const unsigned char *)actual;

  for (size_t index = 0; index < num_elements; index++) {
    if (ptr_exp[index] != ptr_act[index]) {
      CLUT_START_FAILURE_LOG(file, line, msg);

      ClutPrintMismatchArray(index);
      ClutPrintExpectedActualChar(ptr_exp[index], ptr_act[index], CLUT_STR_WAS);

      CLUT_END_FAILURE_LOG();
      return;
    }
  }
}

void ClutTestAssertEqualIntArray(const int *expected, const int *actual, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  CHECK_MEMORY_PRECONDITIONS;

  const unsigned int *ptr_exp = (const unsigned int *)expected;
  const unsigned int *ptr_act = (const unsigned int *)actual;

  for (size_t index = 0; index < num_elements; index++) {
    if (ptr_exp[index] != ptr_act[index]) {
      CLUT_START_FAILURE_LOG(file, line, msg);

      ClutPrintMismatchArray(index);
      ClutPrintExpectedActualInt(ptr_exp[index], ptr_act[index], CLUT_STR_WAS);

      CLUT_END_FAILURE_LOG();
      return;
    }
  }
}

void ClutTestAssertEqualUintArray(const size_t *expected, const size_t *actual, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  CHECK_MEMORY_PRECONDITIONS;

  for (size_t index = 0; index < num_elements; index++) {
    if (expected[index] != actual[index]) {
      CLUT_START_FAILURE_LOG(file, line, msg);

      ClutPrintMismatchArray(index);
      ClutPrintExpectedActualUint(expected[index], actual[index], CLUT_STR_WAS);

      CLUT_END_FAILURE_LOG();
      return;
    }
  }
}

void ClutTestAssertEqualFloatArray(const float *expected, const float *actual, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  CHECK_MEMORY_PRECONDITIONS;

  for (size_t index = 0; index < num_elements; index++) {
    if (!clut_float_equal(expected[index], actual[index])) {
      CLUT_START_FAILURE_LOG(file, line, msg);

      ClutPrintMismatchArray(index);
      ClutPrintExpectedActualFloat(expected[index], actual[index], CLUT_STR_WAS);

      CLUT_END_FAILURE_LOG();
      return;
    }
  }
}

void ClutTestAssertEqualDoubleArray(const double *expected, const double *actual, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  CHECK_MEMORY_PRECONDITIONS;

  for (size_t index = 0; index < num_elements; index++) {
    if (!clut_double_equal(expected[index], actual[index])) {
      CLUT_START_FAILURE_LOG(file, line, msg);

      ClutPrintMismatchArray(index);
      ClutPrintExpectedActualDouble(expected[index], actual[index], CLUT_STR_WAS);

      CLUT_END_FAILURE_LOG();
      return;
    }
  }
}

void ClutTestAssertEqualStringArray(const char **expected, const char **actual, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  CHECK_MEMORY_PRECONDITIONS;

  for (size_t index = 0; index < num_elements; index++) {
    const char *exp_str = expected[index];
    const char *act_str = actual[index];

    bool mismatch = false;

    if (exp_str == act_str) {
      continue;
    } else if (exp_str == NULL || act_str == NULL) {
      mismatch = true;
    } else {
      for (size_t i = 0; exp_str[i] || act_str[i]; i++) {
        if (exp_str[i] != act_str[i]) {
          mismatch = true;
          break;
        }
      }
    }

    if (mismatch) {
      CLUT_START_FAILURE_LOG(file, line, msg);
      ClutPrintMismatchArray(index);
      ClutPrintExpectedActualString(exp_str, act_str, CLUT_STR_WAS);
      CLUT_END_FAILURE_LOG();
      return;
    }
  }
}

void ClutTestAssertWithinCharArray(const char *expected, char delta, const char *actual, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  CHECK_MEMORY_PRECONDITIONS;

  for (size_t index = 0; index < num_elements; index++) {
    char diff = (actual[index] > expected[index]) ? (actual[index] - expected[index]) : (expected[index] - actual[index]);

    if (diff > delta) {
      CLUT_START_FAILURE_LOG(file, line, msg);
      ClutPrintMismatchArray(index);
      ClutPrintWithinDiffChar(expected[index], delta, diff);
      CLUT_END_FAILURE_LOG();
      return;
    }
  }
}

void ClutTestAssertWithinIntArray(const int *expected, int delta, const int *actual, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  CHECK_MEMORY_PRECONDITIONS;

  for (size_t index = 0; index < num_elements; index++) {
    int diff = (actual[index] > expected[index]) ? (actual[index] - expected[index]) : (expected[index] - actual[index]);

    if (diff > delta) {
      CLUT_START_FAILURE_LOG(file, line, msg);
      ClutPrintMismatchArray(index);
      ClutPrintWithinDiffInt(expected[index], delta, diff);
      CLUT_END_FAILURE_LOG();
      return;
    }
  }
}

void ClutTestAssertWithinUintArray(const size_t *expected, size_t delta, const size_t *actual, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  CHECK_MEMORY_PRECONDITIONS;

  for (size_t index = 0; index < num_elements; index++) {
    size_t diff = (actual[index] > expected[index]) ? (actual[index] - expected[index]) : (expected[index] - actual[index]);

    if (diff > delta) {
      CLUT_START_FAILURE_LOG(file, line, msg);
      ClutPrintMismatchArray(index);
      ClutPrintWithinDiffUint(expected[index], delta, diff);
      CLUT_END_FAILURE_LOG();
      return;
    }
  }
}

void ClutTestAssertWithinFloatArray(const float *expected, float delta, const float *actual, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  CHECK_MEMORY_PRECONDITIONS;

  for (size_t index = 0; index < num_elements; index++) {
    float diff = clut_fabsf(actual[index] - expected[index]);

    if (diff > delta) {
      CLUT_START_FAILURE_LOG(file, line, msg);
      ClutPrintMismatchArray(index);
      ClutPrintWithinDiffFloat(expected[index], delta, diff);
      CLUT_END_FAILURE_LOG();
      return;
    }
  }
}

void ClutTestAssertWithinDoubleArray(const double *expected, double delta, const double *actual, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  CHECK_MEMORY_PRECONDITIONS;

  for (size_t index = 0; index < num_elements; index++) {
    double diff = clut_fabs(actual[index] - expected[index]);

    if (diff > delta) {
      CLUT_START_FAILURE_LOG(file, line, msg);
      ClutPrintMismatchArray(index);
      ClutPrintWithinDiffDouble(expected[index], delta, diff);
      CLUT_END_FAILURE_LOG();
      return;
    }
  }
}

#endif
#endif
