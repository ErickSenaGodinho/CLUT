#ifndef INCLUDE_CLUT_H
#define INCLUDE_CLUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Color / text macros */
#ifdef CLUT_OUTPUT_COLOR
#define CLUT_STR_BEGIN_RED_TEXT "\033[31m"
#define CLUT_STR_BEGIN_BLUE_TEXT "\033[34m"
#define CLUT_STR_END_COLOR_TEXT "\033[0m"
#define CLUT_RED_TEXT(text) CLUT_STR_BEGIN_RED_TEXT text CLUT_STR_END_COLOR_TEXT
#define CLUT_GREEN_TEXT(text) "\033[32m" text CLUT_STR_END_COLOR_TEXT
#define CLUT_BLUE_TEXT(text) CLUT_STR_BEGIN_BLUE_TEXT text CLUT_STR_END_COLOR_TEXT
#else
#define CLUT_STR_BEGIN_RED_TEXT ""
#define CLUT_STR_BEGIN_BLUE_TEXT ""
#define CLUT_STR_END_COLOR_TEXT ""
#define CLUT_RED_TEXT(text) text
#define CLUT_GREEN_TEXT(text) text
#define CLUT_BLUE_TEXT(text) text
#endif

/* Streams / epsilons */
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

/* Assertion message strings */
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

/* String Builder */
#define CLUT_SB_INIT_CAP 256

typedef struct {
  char *data;
  size_t len;
  size_t cap;
} ClutSB;

/* Suite Result */
typedef struct {
  size_t total_tests;
  size_t passed;
  size_t failures;
  double total_seconds;
} ClutSuiteResult;

typedef struct {
  const char *file;
  int line;
  int iteration_index;
} ClutLogRecord;

/* Function-pointer types */
typedef void (*ClutHookFn)();
typedef void (*ClutTestFn)();

/* Core data structures */
typedef struct {
  size_t total_tests;
  size_t passed;
  size_t failures;
  long start_time;
  ClutSB output;
  ClutSB test_message;
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
  int iteration_index;
} ClutTestState;

typedef struct {
  ClutRunner runner;
  ClutHooks hooks;
  ClutTestState current;
} ClutData;

/* TEST / REPEATED_TEST / PARAM_TEST macros */
#define TEST(name)                                                                                                                                                                                                                                                 \
  void name(void);                                                                                                                                                                                                                                                 \
  void run_##name(void) { name(); }                                                                                                                                                                                                                                \
  void name(void)

typedef struct {
  size_t current_repetition;
  size_t total_repetitions;
} ClutRepeatedTestInput;

#define REPEATED_TEST(name, value)                                                                                                                                                                                                                                 \
  void name(ClutRepeatedTestInput input);                                                                                                                                                                                                                          \
  void run_##name(void) {                                                                                                                                                                                                                                          \
    ClutRepeatedTestInput input = (ClutRepeatedTestInput){.total_repetitions = value};                                                                                                                                                                             \
                                                                                                                                                                                                                                                                   \
    bool failed = false;                                                                                                                                                                                                                                           \
    for (size_t i = 1; i <= value; ++i) {                                                                                                                                                                                                                          \
      clut_sb_clear(&Clut.runner.test_message);                                                                                                                                                                                                                    \
      Clut.current.iteration_index = (int)i;                                                                                                                                                                                                                       \
      input.current_repetition = i;                                                                                                                                                                                                                                \
      name(input);                                                                                                                                                                                                                                                 \
      if (Clut.current.failed) {                                                                                                                                                                                                                                   \
        failed = true;                                                                                                                                                                                                                                             \
        Clut.current.failed = false;                                                                                                                                                                                                                               \
      }                                                                                                                                                                                                                                                            \
    }                                                                                                                                                                                                                                                              \
    Clut.current.failed = failed;                                                                                                                                                                                                                                  \
  }                                                                                                                                                                                                                                                                \
  void name(ClutRepeatedTestInput input)

#define PARAM_TEST(name, type, ...)                                                                                                                                                                                                                                \
  type clut_##name##_input_arr[] = __VA_ARGS__;                                                                                                                                                                                                                    \
  void name(type input);                                                                                                                                                                                                                                           \
  void run_##name(void) {                                                                                                                                                                                                                                          \
    size_t n = sizeof(clut_##name##_input_arr) / sizeof(clut_##name##_input_arr[0]);                                                                                                                                                                               \
    bool failed = false;                                                                                                                                                                                                                                           \
    for (size_t i = 0; i < n; ++i) {                                                                                                                                                                                                                               \
      clut_sb_clear(&Clut.runner.test_message);                                                                                                                                                                                                                    \
      Clut.current.iteration_index = (int)i;                                                                                                                                                                                                                       \
      name(clut_##name##_input_arr[i]);                                                                                                                                                                                                                            \
      if (Clut.current.failed) {                                                                                                                                                                                                                                   \
        failed = true;                                                                                                                                                                                                                                             \
        Clut.current.failed = false;                                                                                                                                                                                                                               \
      }                                                                                                                                                                                                                                                            \
    }                                                                                                                                                                                                                                                              \
    Clut.current.failed = failed;                                                                                                                                                                                                                                  \
  }                                                                                                                                                                                                                                                                \
  void name(type input)

/* Hook-registration macros */
#define CLUT_BEFORE_ALL(hook_fn) Clut.hooks.before_all = (hook_fn)
#define CLUT_BEFORE_EACH(hook_fn) Clut.hooks.before_each = (hook_fn)
#define CLUT_AFTER_ALL(hook_fn) Clut.hooks.after_all = (hook_fn)
#define CLUT_AFTER_EACH(hook_fn) Clut.hooks.after_each = (hook_fn)

/* Test lifecycle macros */
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
#define TEST_ASSERT_MESSAGE(condition, msg) ClutTestAssert((condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_TRUE_MESSAGE(condition, msg) ClutTestAssert((condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_FALSE_MESSAGE(condition, msg) ClutTestAssert(!(condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_UNLESS_MESSAGE(condition, msg) ClutTestAssert(!(condition), __FILE__, __LINE__, (msg))
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
#define TEST_ASSERT_NOT_EQUAL_INT_MESSAGE(expected, actual, msg) ClutTestAssertNotEqualInt((expected), (actual), __FILE__, __LINE__, (msg))
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

#define TEST_ASSERT_EQUAL_MEMORY_MESSAGE(expected, actual, len, msg) ClutTestAssertEqualMemory((expected), (actual), (len), 1, __FILE__, __LINE__, (msg))
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

/* Public API Declarations */
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef CLUT_REALLOC
#include <stdlib.h>
#define CLUT_REALLOC realloc
#endif

#ifndef CLUT_FREE
#include <stdlib.h>
#define CLUT_FREE free
#endif

ClutData Clut = {0};

/* String Builder Implementation */
static void clut_sb_grow(ClutSB *sb, size_t needed) {
  if (sb->len + needed <= sb->cap)
    return;
  size_t new_cap = sb->cap ? sb->cap * 2 : CLUT_SB_INIT_CAP;
  while (new_cap < sb->len + needed)
    new_cap *= 2;
  sb->data = (char *)CLUT_REALLOC(sb->data, new_cap);
  sb->cap = new_cap;
}

static void clut_sb_append(ClutSB *sb, const char *str) {
  if (!str)
    str = "(null)";
  size_t n = strlen(str);
  clut_sb_grow(sb, n);
  memcpy(sb->data + sb->len, str, n);
  sb->len += n;
}

static void clut_sb_appendc(ClutSB *sb, char c) {
  clut_sb_grow(sb, 1);
  sb->data[sb->len++] = c;
}

static void clut_sb_appendf(ClutSB *sb, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int n = vsnprintf(NULL, 0, fmt, args);
  va_end(args);
  if (n <= 0)
    return;
  clut_sb_grow(sb, (size_t)n);
  va_start(args, fmt);
  vsnprintf(sb->data + sb->len, (size_t)n + 1, fmt, args);
  va_end(args);
  sb->len += (size_t)n;
}

static void clut_sb_init(ClutSB *sb) {
  sb->data = NULL;
  sb->len = 0;
  sb->cap = 0;
}

static void clut_sb_clear(ClutSB *sb) { sb->len = 0; }

static void clut_sb_free(ClutSB *sb) {
  CLUT_FREE(sb->data);
  sb->data = NULL;
  sb->len = 0;
  sb->cap = 0;
}

static const char *clut_sb_cstr(ClutSB *sb) {
  if (!sb->data)
    return "";
  clut_sb_grow(sb, 1);
  sb->data[sb->len] = '\0';
  return sb->data;
}

/* Helpers */
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

/* Log Helpers */

static void clut_log_record_capture(ClutLogRecord *record, const char *file, int line) {
  record->file = file;
  record->line = line;
  record->iteration_index = Clut.current.iteration_index;
}

#ifdef CLUT_OUTPUT_GITHUB
static void clut_backend_github_on_pass(const char *test_name, double elapsed_seconds) { fprintf(CLUT_STREAM_DEFAULT, "::notice title=PASS::%s (%.3fs)\n", test_name, elapsed_seconds); }
static void clut_backend_github_on_fail_header(void) {
  double elapsed_seconds = ClutElapsedSeconds(Clut.current.start_time);
  clut_sb_appendf(&Clut.runner.output, "::notice title=FAIL::%s (%.3fs)\n", Clut.current.name, elapsed_seconds);
}
static void clut_backend_github_append_failure(const ClutLogRecord *record) { clut_sb_appendf(&Clut.runner.output, "::error file=%s,line=%d,title=%s::%s\n", record->file, record->line, Clut.current.name, clut_sb_cstr(&Clut.runner.test_message)); }
static void clut_backend_github_flush_failures(void) { fprintf(CLUT_STREAM_FAIL, "%s", clut_sb_cstr(&Clut.runner.output)); }
static void clut_backend_github_on_suite_end(const ClutSuiteResult *result) {
  fprintf(CLUT_STREAM_DEFAULT, "::notice title=Suite Results::Tests=%zu Passed=%zu Failed=%zu Time=%.3fs\n", result->total_tests, result->passed, result->failures, result->total_seconds);
}
#define clut_dispatch_pass(test_name, elapsed_seconds) clut_backend_github_on_pass((test_name), (elapsed_seconds))
#define clut_dispatch_fail_header() clut_backend_github_on_fail_header()
#define clut_dispatch_fail_append(record) clut_backend_github_append_failure((record))
#define clut_dispatch_fail_flush() clut_backend_github_flush_failures()
#define clut_dispatch_suite_end(result) clut_backend_github_on_suite_end((result))
#else /* default */
static void clut_backend_default_on_pass(const char *test_name, double elapsed_seconds) { fprintf(CLUT_STREAM_DEFAULT, "%s%-50s%.3fs\n", CLUT_GREEN_TEXT("[ PASS ] "), test_name, elapsed_seconds); }
static void clut_backend_default_on_fail_header(void) {
  FILE *stream = CLUT_STREAM_FAIL;
  double elapsed_seconds = ClutElapsedSeconds(Clut.current.start_time);
  fprintf(stream, "%s%s%-50s%.3fs%s\n", CLUT_RED_TEXT("[ FAIL ] "), CLUT_STR_BEGIN_RED_TEXT, Clut.current.name, elapsed_seconds, CLUT_STR_END_COLOR_TEXT);
}
static void clut_backend_default_append_failure(const ClutLogRecord *record) {
  clut_sb_append(&Clut.runner.output, CLUT_STR_BEGIN_RED_TEXT);
  clut_sb_appendf(&Clut.runner.output, "%s:%d:%s", record->file, record->line, Clut.current.name);
  if (record->iteration_index >= 0) {
    clut_sb_appendf(&Clut.runner.output, "[%d]", record->iteration_index);
  }
  clut_sb_appendc(&Clut.runner.output, ':');
  clut_sb_append(&Clut.runner.output, clut_sb_cstr(&Clut.runner.test_message));
  clut_sb_append(&Clut.runner.output, CLUT_STR_END_COLOR_TEXT);
  clut_sb_appendc(&Clut.runner.output, '\n');
}
static void clut_backend_default_flush_failures(void) { fprintf(CLUT_STREAM_FAIL, "%s", clut_sb_cstr(&Clut.runner.output)); }
static void clut_backend_default_on_suite_end(const ClutSuiteResult *result) {
  FILE *stream = CLUT_STREAM_DEFAULT;
  fprintf(stream, "--------------------------------\n");
  fprintf(stream, CLUT_BLUE_TEXT("Tests run:  ") CLUT_GREEN_TEXT("%zu\n"), result->total_tests);
  fprintf(stream, CLUT_BLUE_TEXT("Passed:     ") CLUT_GREEN_TEXT("%zu\n"), result->passed);
  fprintf(stream, CLUT_BLUE_TEXT("Failed:     ") CLUT_RED_TEXT("%zu\n"), result->failures);
  fprintf(stream, "--------------------------------\n");
  fprintf(stream, CLUT_BLUE_TEXT("Total time: %.3fs\n"), result->total_seconds);
  fprintf(stream, "\n");
}

#define clut_dispatch_pass(test_name, elapsed_seconds) clut_backend_default_on_pass((test_name), (elapsed_seconds))
#define clut_dispatch_fail_header() clut_backend_default_on_fail_header()
#define clut_dispatch_fail_append(record) clut_backend_default_append_failure((record))
#define clut_dispatch_fail_flush() clut_backend_default_flush_failures()
#define clut_dispatch_suite_end(result) clut_backend_default_on_suite_end((result))
#endif

static void clut_record_failure(const char *file, int line) {
  Clut.current.failed = true;
  ClutLogRecord record;
  clut_log_record_capture(&record, file, line);
  clut_dispatch_fail_append(&record);
}

static void clut_append_message(const char *msg) { clut_sb_append(&Clut.runner.test_message, msg); }
static void clut_append_message_char(char c) { clut_sb_appendf(&Clut.runner.test_message, "'%c' (0x%02X)", c, (unsigned char)c); }
static void clut_append_message_int(int v) { clut_sb_appendf(&Clut.runner.test_message, "%d", v); }
static void clut_append_message_uint(size_t v) { clut_sb_appendf(&Clut.runner.test_message, "%zu", v); }
static void clut_append_message_float(float v) { clut_sb_appendf(&Clut.runner.test_message, "%.5f", v); }
static void clut_append_message_double(double v) { clut_sb_appendf(&Clut.runner.test_message, "%.9f", v); }
static void clut_append_message_ptr(void *p) { clut_sb_appendf(&Clut.runner.test_message, "%p", p); }
static void clut_append_message_hex(int v) { clut_sb_appendf(&Clut.runner.test_message, "0x%02X", v); }
static void clut_append_message_str(const char *str) {
  if (str == NULL) {
    clut_append_message("(null)");
  } else {
    clut_sb_appendc(&Clut.runner.test_message, '"');
    clut_append_message(str);
    clut_sb_appendc(&Clut.runner.test_message, '"');
  }
}
#define clut_append_message_formated(fmt, ...) clut_sb_appendf(&Clut.runner.test_message, fmt, __VA_ARGS__)

static void clut_append_message_array_prefix(size_t index) {
  clut_append_message(CLUT_STR_ARRAY_MISMATCH_AT);
  clut_append_message_formated("[%zu] ", index);
}

#define CLUT_MESSAGE_EXPECTED_ACTUAL(append_val, lhs, op_str, rhs)                                                                                                                                                                                                 \
  do {                                                                                                                                                                                                                                                             \
    clut_append_message(CLUT_STR_EXPECTED);                                                                                                                                                                                                                        \
    append_val((lhs));                                                                                                                                                                                                                                             \
    clut_append_message((op_str));                                                                                                                                                                                                                                 \
    append_val((rhs));                                                                                                                                                                                                                                             \
  } while (0)

#define CLUT_MESSAGE_WITHIN_DIFF(append_val, expected, delta, diff)                                                                                                                                                                                                \
  do {                                                                                                                                                                                                                                                             \
    clut_append_message(CLUT_STR_EXPECTED);                                                                                                                                                                                                                        \
    append_val((expected));                                                                                                                                                                                                                                        \
    clut_append_message(CLUT_STR_WITHIN_DELTA);                                                                                                                                                                                                                    \
    append_val((delta));                                                                                                                                                                                                                                           \
    clut_append_message(CLUT_STR_BUT_DIFF);                                                                                                                                                                                                                        \
    append_val((diff));                                                                                                                                                                                                                                            \
  } while (0)

#define RETURN_IF_FAILED                                                                                                                                                                                                                                           \
  do {                                                                                                                                                                                                                                                             \
    if (Clut.current.failed)                                                                                                                                                                                                                                       \
      return;                                                                                                                                                                                                                                                      \
  } while (0)

#define CLUT_ASSERT_COMPARE(append_fn, condition, expected, actual, file, line, msg, op_str)                                                                                                                                                                       \
  do {                                                                                                                                                                                                                                                             \
    RETURN_IF_FAILED;                                                                                                                                                                                                                                              \
    if (condition)                                                                                                                                                                                                                                                 \
      return;                                                                                                                                                                                                                                                      \
    if (msg) {                                                                                                                                                                                                                                                     \
      clut_append_message((msg));                                                                                                                                                                                                                                  \
    } else {                                                                                                                                                                                                                                                       \
      CLUT_MESSAGE_EXPECTED_ACTUAL(append_fn, (actual), (op_str), (expected));                                                                                                                                                                                     \
    }                                                                                                                                                                                                                                                              \
    clut_record_failure(file, line);                                                                                                                                                                                                                               \
  } while (0)

#define CLUT_ASSERT_WITHIN(append_fn, diff_val, expected, delta, file, line, msg)                                                                                                                                                                                  \
  do {                                                                                                                                                                                                                                                             \
    RETURN_IF_FAILED;                                                                                                                                                                                                                                              \
    if ((diff_val) <= (delta))                                                                                                                                                                                                                                     \
      return;                                                                                                                                                                                                                                                      \
    if (msg) {                                                                                                                                                                                                                                                     \
      clut_append_message((msg));                                                                                                                                                                                                                                  \
    } else {                                                                                                                                                                                                                                                       \
      CLUT_MESSAGE_WITHIN_DIFF(append_fn, (expected), (delta), (diff_val));                                                                                                                                                                                        \
    }                                                                                                                                                                                                                                                              \
    clut_record_failure(file, line);                                                                                                                                                                                                                               \
  } while (0)

#define CHECK_MEMORY_PRECONDITIONS                                                                                                                                                                                                                                 \
  do {                                                                                                                                                                                                                                                             \
    if (num_elements == 0)                                                                                                                                                                                                                                         \
      return;                                                                                                                                                                                                                                                      \
    if (expected == actual)                                                                                                                                                                                                                                        \
      return;                                                                                                                                                                                                                                                      \
    if (expected == NULL || actual == NULL) {                                                                                                                                                                                                                      \
      clut_append_message(msg ? msg : CLUT_STR_MEMORY_NULL);                                                                                                                                                                                                       \
      clut_record_failure(file, line);                                                                                                                                                                                                                             \
      return;                                                                                                                                                                                                                                                      \
    }                                                                                                                                                                                                                                                              \
  } while (0)

void ClutTestReset(void) {
  Clut.current.name = NULL;
  Clut.current.failed = false;
  Clut.current.iteration_index = -1;
  clut_sb_clear(&Clut.runner.output);
  clut_sb_clear(&Clut.runner.test_message);
}

void ClutRunnerReset(void) {
  Clut.runner.total_tests = 0;
  Clut.runner.failures = 0;
  Clut.runner.passed = 0;
  Clut.runner.start_time = 0;
}

void ClutTestBegin(void) {
  ClutRunnerReset();
  clut_sb_init(&Clut.runner.output);
  clut_sb_init(&Clut.runner.test_message);
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
    clut_dispatch_fail_header();
    clut_dispatch_fail_flush();
    Clut.runner.failures++;
  } else {
    clut_dispatch_pass(test_name, ClutElapsedSeconds(Clut.current.start_time));
    Clut.runner.passed++;
  }

  if (Clut.hooks.after_each)
    Clut.hooks.after_each();
}

int ClutTestEnd(void) {
  double total_time = ClutElapsedSeconds(Clut.runner.start_time);
  if (Clut.hooks.after_all)
    Clut.hooks.after_all();
  if (Clut.runner.total_tests == 0)
    return (int)Clut.runner.failures;

  ClutSuiteResult result = {Clut.runner.total_tests, Clut.runner.passed, Clut.runner.failures, total_time};
  clut_dispatch_suite_end(&result);

  clut_sb_free(&Clut.runner.output);
  clut_sb_free(&Clut.runner.test_message);

  int failures = (int)Clut.runner.failures;
  ClutRunnerReset();
  return failures;
}

void ClutTestAssert(bool condition, const char *file, int line, const char *msg) {
  if (!condition) {
    clut_append_message(msg);
    clut_record_failure(file, line);
  }
}
void ClutTestAssertEqualChar(char expected, char actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_char, expected == actual, expected, actual, file, line, msg, CLUT_STR_RECEIVED); }
void ClutTestAssertEqualInt(int expected, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_int, expected == actual, expected, actual, file, line, msg, CLUT_STR_RECEIVED); }
void ClutTestAssertEqualUint(size_t expected, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_uint, expected == actual, expected, actual, file, line, msg, CLUT_STR_RECEIVED); }
void ClutTestAssertEqualFloat(float expected, float actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_float, clut_float_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_RECEIVED); }
void ClutTestAssertEqualDouble(double expected, double actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_double, clut_double_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_RECEIVED); }

void ClutTestAssertEqualString(const char *expected, const char *actual, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  if (expected == actual)
    return;
  bool failed = (expected == NULL || actual == NULL);
  if (!failed) {
    for (size_t i = 0; expected[i] || actual[i]; i++) {
      if (expected[i] != actual[i]) {
        failed = true;
        break;
      }
    }
  }
  if (!failed)
    return;
  CLUT_ASSERT_COMPARE(clut_append_message_str, !failed, expected, actual, file, line, msg, CLUT_STR_EQUAL);
}

void ClutTestAssertEqualStringLen(const char *expected, const char *actual, size_t len, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  if (expected == actual)
    return;
  bool failed = (expected == NULL || actual == NULL);
  if (!failed) {
    for (size_t i = 0; i < len && (expected[i] || actual[i]); i++) {
      if (expected[i] != actual[i]) {
        failed = true;
        break;
      }
    }
  }
  if (!failed)
    return;
  if (msg) {
    clut_append_message(msg);
  } else {
    clut_append_message(CLUT_STR_EXPECTED);
    clut_append_message_str(expected);
    clut_append_message(CLUT_STR_EQUAL);
    clut_append_message_str(actual);
    clut_append_message(CLUT_STR_UNTIL_LEN);
    clut_append_message_uint(len);
  }
  clut_record_failure(file, line);
}

void ClutTestAssertEqualPtr(void *expected, void *actual, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  if (expected == actual)
    return;
  if (msg) {
    clut_append_message(msg);
  } else {
    CLUT_MESSAGE_EXPECTED_ACTUAL(clut_append_message_ptr, expected, CLUT_STR_RECEIVED, actual);
  }
  clut_record_failure(file, line);
}

void ClutTestAssertNotEqualChar(char expected, char actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_char, expected != actual, expected, actual, file, line, msg, CLUT_STR_NOT_EQUAL); }
void ClutTestAssertNotEqualInt(int expected, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_int, expected != actual, expected, actual, file, line, msg, CLUT_STR_NOT_EQUAL); }
void ClutTestAssertNotEqualUint(size_t expected, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_uint, expected != actual, expected, actual, file, line, msg, CLUT_STR_NOT_EQUAL); }
void ClutTestAssertNotEqualFloat(float expected, float actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_float, !clut_float_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_NOT_EQUAL); }
void ClutTestAssertNotEqualDouble(double expected, double actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_double, !clut_double_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_NOT_EQUAL); }

void ClutTestAssertNotEqualString(const char *expected, const char *actual, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  bool equal;
  if (expected == actual)
    equal = true;
  else if (expected == NULL || actual == NULL)
    equal = false;
  else {
    equal = true;
    for (size_t i = 0; expected[i] || actual[i]; i++) {
      if (expected[i] != actual[i]) {
        equal = false;
        break;
      }
    }
  }
  if (!equal)
    return;
  CLUT_ASSERT_COMPARE(clut_append_message_str, !equal, expected, actual, file, line, msg, CLUT_STR_NOT_EQUAL);
}

void ClutTestAssertNotEqualStringLen(const char *expected, const char *actual, size_t len, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  bool equal;
  if (expected == actual)
    equal = true;
  else if (expected == NULL || actual == NULL)
    equal = false;
  else {
    equal = true;
    for (size_t i = 0; i < len && (expected[i] || actual[i]); i++) {
      if (expected[i] != actual[i]) {
        equal = false;
        break;
      }
    }
  }
  if (!equal)
    return;
  if (msg) {
    clut_append_message(msg);
  } else {
    clut_append_message(CLUT_STR_EXPECTED);
    clut_append_message_str(expected);
    clut_append_message(CLUT_STR_NOT_EQUAL);
    clut_append_message_str(actual);
    clut_append_message(CLUT_STR_UNTIL_LEN);
    clut_append_message_uint(len);
  }
  clut_record_failure(file, line);
}

void ClutTestAssertNotEqualPtr(void *expected, void *actual, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  if (expected != actual)
    return;
  if (msg) {
    clut_append_message(msg);
  } else {
    CLUT_MESSAGE_EXPECTED_ACTUAL(clut_append_message_ptr, expected, CLUT_STR_NOT_EQUAL, actual);
  }
  clut_record_failure(file, line);
}

void ClutTestAssertIntGreaterThan(int expected, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_int, actual > expected, expected, actual, file, line, msg, CLUT_STR_GREATER_THAN); }
void ClutTestAssertIntGreaterOrEqual(int expected, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_int, actual >= expected, expected, actual, file, line, msg, CLUT_STR_GREATER_OR_EQUAL); }
void ClutTestAssertIntLessThan(int expected, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_int, actual < expected, expected, actual, file, line, msg, CLUT_STR_LESS_THAN); }
void ClutTestAssertIntLessOrEqual(int expected, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_int, actual <= expected, expected, actual, file, line, msg, CLUT_STR_LESS_OR_EQUAL); }

void ClutTestAssertUintGreaterThan(size_t expected, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_uint, actual > expected, expected, actual, file, line, msg, CLUT_STR_GREATER_THAN); }
void ClutTestAssertUintGreaterOrEqual(size_t expected, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_uint, actual >= expected, expected, actual, file, line, msg, CLUT_STR_GREATER_OR_EQUAL); }
void ClutTestAssertUintLessThan(size_t expected, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_uint, actual < expected, expected, actual, file, line, msg, CLUT_STR_LESS_THAN); }
void ClutTestAssertUintLessOrEqual(size_t expected, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_uint, actual <= expected, expected, actual, file, line, msg, CLUT_STR_LESS_OR_EQUAL); }

void ClutTestAssertFloatGreaterThan(float expected, float actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_float, actual > expected, expected, actual, file, line, msg, CLUT_STR_GREATER_THAN); }
void ClutTestAssertFloatGreaterOrEqual(float expected, float actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_append_message_float, actual > expected || clut_float_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_GREATER_OR_EQUAL);
}
void ClutTestAssertFloatLessThan(float expected, float actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_float, actual < expected, expected, actual, file, line, msg, CLUT_STR_LESS_THAN); }
void ClutTestAssertFloatLessOrEqual(float expected, float actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_append_message_float, actual < expected || clut_float_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_LESS_OR_EQUAL);
}

void ClutTestAssertDoubleGreaterThan(double expected, double actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_double, actual > expected, expected, actual, file, line, msg, CLUT_STR_GREATER_THAN); }
void ClutTestAssertDoubleGreaterOrEqual(double expected, double actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_append_message_double, actual > expected || clut_double_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_GREATER_OR_EQUAL);
}
void ClutTestAssertDoubleLessThan(double expected, double actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_append_message_double, actual < expected, expected, actual, file, line, msg, CLUT_STR_LESS_THAN); }
void ClutTestAssertDoubleLessOrEqual(double expected, double actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_append_message_double, actual < expected || clut_double_equal(expected, actual), expected, actual, file, line, msg, CLUT_STR_LESS_OR_EQUAL);
}

void ClutTestAssertWithinChar(char expected, char delta, char actual, const char *file, int line, const char *msg) {
  char diff = (actual > expected) ? (actual - expected) : (expected - actual);
  CLUT_ASSERT_WITHIN(clut_append_message_char, diff, expected, delta, file, line, msg);
}
void ClutTestAssertWithinInt(int expected, int delta, int actual, const char *file, int line, const char *msg) {
  int diff = (actual > expected) ? (actual - expected) : (expected - actual);
  CLUT_ASSERT_WITHIN(clut_append_message_int, diff, expected, delta, file, line, msg);
}
void ClutTestAssertWithinUint(size_t expected, size_t delta, size_t actual, const char *file, int line, const char *msg) {
  size_t diff = (actual > expected) ? (actual - expected) : (expected - actual);
  CLUT_ASSERT_WITHIN(clut_append_message_uint, diff, expected, delta, file, line, msg);
}
void ClutTestAssertWithinFloat(float expected, float delta, float actual, const char *file, int line, const char *msg) {
  float diff = clut_fabsf(actual - expected);
  CLUT_ASSERT_WITHIN(clut_append_message_float, diff, expected, delta, file, line, msg);
}
void ClutTestAssertWithinDouble(double expected, double delta, double actual, const char *file, int line, const char *msg) {
  double diff = clut_fabs(actual - expected);
  CLUT_ASSERT_WITHIN(clut_append_message_double, diff, expected, delta, file, line, msg);
}

void ClutTestAssertEqualMemory(const void *expected, const void *actual, size_t len, size_t num_elements, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  CHECK_MEMORY_PRECONDITIONS;
  if (len == 0)
    return;

  const unsigned char *ptr_expected = (const unsigned char *)expected;
  const unsigned char *ptr_actual = (const unsigned char *)actual;

  for (size_t idx = 0; idx < num_elements; idx++) {
    for (size_t byte = 0; byte < len; byte++) {
      size_t offset = idx * len + byte;
      if (ptr_expected[offset] == ptr_actual[offset])
        continue;

      if (msg) {
        clut_append_message(msg);
      } else {
        if (num_elements > 1) {
          clut_append_message_array_prefix(idx);
          clut_append_message(CLUT_STR_BYTE_OFFSET);
          clut_append_message_formated("[%zu] ", byte);
        } else {
          clut_append_message(CLUT_STR_MISMATCH_AT);
          clut_append_message_uint(byte);
        }
        CLUT_MESSAGE_EXPECTED_ACTUAL(clut_append_message_hex, ptr_expected[offset], CLUT_STR_WAS, ptr_actual[offset]);
      }
      clut_record_failure(file, line);
      return;
    }
  }
}

#define CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, index, append_fn, exp_val, act_val)                                                                                                                                                                            \
  do {                                                                                                                                                                                                                                                             \
    if (msg) {                                                                                                                                                                                                                                                     \
      clut_append_message((msg));                                                                                                                                                                                                                                  \
    } else {                                                                                                                                                                                                                                                       \
      clut_append_message_array_prefix((index));                                                                                                                                                                                                                   \
      CLUT_MESSAGE_EXPECTED_ACTUAL(append_fn, (exp_val), CLUT_STR_WAS, (act_val));                                                                                                                                                                                 \
    }                                                                                                                                                                                                                                                              \
    clut_record_failure(file, line);                                                                                                                                                                                                                               \
  } while (0)

void ClutTestAssertEqualCharArray(const char *expected, const char *actual, size_t num_elements, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  CHECK_MEMORY_PRECONDITIONS;
  for (size_t i = 0; i < num_elements; i++) {
    if (expected[i] == actual[i])
      continue;
    CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, i, clut_append_message_char, expected[i], actual[i]);
    return;
  }
}

void ClutTestAssertEqualIntArray(const int *expected, const int *actual, size_t num_elements, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  CHECK_MEMORY_PRECONDITIONS;
  for (size_t i = 0; i < num_elements; i++) {
    if (expected[i] == actual[i])
      continue;
    CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, i, clut_append_message_int, expected[i], actual[i]);
    return;
  }
}

void ClutTestAssertEqualUintArray(const size_t *expected, const size_t *actual, size_t num_elements, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  CHECK_MEMORY_PRECONDITIONS;
  for (size_t i = 0; i < num_elements; i++) {
    if (expected[i] == actual[i])
      continue;
    CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, i, clut_append_message_uint, expected[i], actual[i]);
    return;
  }
}

void ClutTestAssertEqualFloatArray(const float *expected, const float *actual, size_t num_elements, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  CHECK_MEMORY_PRECONDITIONS;
  for (size_t i = 0; i < num_elements; i++) {
    if (clut_float_equal(expected[i], actual[i]))
      continue;
    CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, i, clut_append_message_float, expected[i], actual[i]);
    return;
  }
}

void ClutTestAssertEqualDoubleArray(const double *expected, const double *actual, size_t num_elements, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  CHECK_MEMORY_PRECONDITIONS;
  for (size_t i = 0; i < num_elements; i++) {
    if (clut_double_equal(expected[i], actual[i]))
      continue;
    CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, i, clut_append_message_double, expected[i], actual[i]);
    return;
  }
}

void ClutTestAssertEqualStringArray(const char **expected, const char **actual, size_t num_elements, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  CHECK_MEMORY_PRECONDITIONS;
  for (size_t i = 0; i < num_elements; i++) {
    const char *exp_str = expected[i];
    const char *act_str = actual[i];
    bool mismatch = false;
    if (exp_str == act_str)
      mismatch = false;
    else if (exp_str == NULL || act_str == NULL)
      mismatch = true;
    else {
      for (size_t j = 0; exp_str[j] || act_str[j]; j++) {
        if (exp_str[j] != act_str[j]) {
          mismatch = true;
          break;
        }
      }
    }
    if (!mismatch)
      continue;
    CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, i, clut_append_message_str, exp_str, act_str);
    return;
  }
}

#define CLUT_ARRAY_WITHIN_RECORD(file, line, msg, index, append_fn, exp_val, delta_val, diff_val)                                                                                                                                                                  \
  do {                                                                                                                                                                                                                                                             \
    if (msg) {                                                                                                                                                                                                                                                     \
      clut_append_message((msg));                                                                                                                                                                                                                                  \
    } else {                                                                                                                                                                                                                                                       \
      clut_append_message_array_prefix((index));                                                                                                                                                                                                                   \
      CLUT_MESSAGE_WITHIN_DIFF(append_fn, (exp_val), (delta_val), (diff_val));                                                                                                                                                                                     \
    }                                                                                                                                                                                                                                                              \
    clut_record_failure(file, line);                                                                                                                                                                                                                               \
  } while (0)

void ClutTestAssertWithinCharArray(const char *expected, char delta, const char *actual, size_t num_elements, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  CHECK_MEMORY_PRECONDITIONS;
  for (size_t i = 0; i < num_elements; i++) {
    char diff = (actual[i] > expected[i]) ? (actual[i] - expected[i]) : (expected[i] - actual[i]);
    if (diff <= delta)
      continue;
    ;
    CLUT_ARRAY_WITHIN_RECORD(file, line, msg, i, clut_append_message_char, expected[i], delta, diff);
    return;
  }
}

void ClutTestAssertWithinIntArray(const int *expected, int delta, const int *actual, size_t num_elements, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  CHECK_MEMORY_PRECONDITIONS;
  for (size_t i = 0; i < num_elements; i++) {
    int diff = (actual[i] > expected[i]) ? (actual[i] - expected[i]) : (expected[i] - actual[i]);
    if (diff <= delta)
      continue;
    CLUT_ARRAY_WITHIN_RECORD(file, line, msg, i, clut_append_message_int, expected[i], delta, diff);
    return;
  }
}

void ClutTestAssertWithinUintArray(const size_t *expected, size_t delta, const size_t *actual, size_t num_elements, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  CHECK_MEMORY_PRECONDITIONS;
  for (size_t i = 0; i < num_elements; i++) {
    size_t diff = (actual[i] > expected[i]) ? (actual[i] - expected[i]) : (expected[i] - actual[i]);
    if (diff <= delta)
      continue;
    CLUT_ARRAY_WITHIN_RECORD(file, line, msg, i, clut_append_message_uint, expected[i], delta, diff);
    return;
  }
}

void ClutTestAssertWithinFloatArray(const float *expected, float delta, const float *actual, size_t num_elements, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  CHECK_MEMORY_PRECONDITIONS;
  for (size_t i = 0; i < num_elements; i++) {
    float diff = clut_fabsf(actual[i] - expected[i]);
    if (diff <= delta)
      continue;
    CLUT_ARRAY_WITHIN_RECORD(file, line, msg, i, clut_append_message_float, expected[i], delta, diff);
    return;
  }
}

void ClutTestAssertWithinDoubleArray(const double *expected, double delta, const double *actual, size_t num_elements, const char *file, int line, const char *msg) {
  RETURN_IF_FAILED;
  CHECK_MEMORY_PRECONDITIONS;
  for (size_t i = 0; i < num_elements; i++) {
    double diff = clut_fabs(actual[i] - expected[i]);
    if (diff <= delta)
      continue;
    CLUT_ARRAY_WITHIN_RECORD(file, line, msg, i, clut_append_message_double, expected[i], delta, diff);
    return;
  }
}

#endif /* CLUT_IMPLEMENTATION */
#endif /* INCLUDE_CLUT_H */
