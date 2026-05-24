#ifndef INCLUDE_CLUT_H
#define INCLUDE_CLUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef CLUT_OUTPUT_COLOR
#define CLUT_STR_FAIL "\033[31mFAIL\033[0m"
#define CLUT_STR_PASSED "\033[32mPASS\033[0m"
#else
#define CLUT_STR_FAIL "FAIL"
#define CLUT_STR_PASSED "PASS"
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
#define CLUT_STR_ARRAY_MISMATCH_AT "Array mismatch at Element ["
#define CLUT_STR_ARRAY_BYTE "], Byte Offset ["
#define CLUT_STR_WAS " but was "

#ifndef CLUT_FLOAT_EPSILON
#define CLUT_FLOAT_EPSILON 1e-5f
#endif

#ifndef CLUT_DOUBLE_EPSILON
#define CLUT_DOUBLE_EPSILON 1e-9
#endif

typedef void (*ClutHookFn)();
typedef void (*ClutTestFn)();

typedef struct {
  size_t total_tests;
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
  const char *file;
  const char *name;

  bool failed;
} ClutTestState;

typedef struct {
  ClutRunner runner;
  ClutHooks hooks;
  ClutTestState current;
} ClutData;

#ifndef CLUT_STREAM_DEFAULT
#define CLUT_STREAM_DEFAULT stdout
#endif

#ifndef CLUT_STREAM_FAIL
#define CLUT_STREAM_FAIL stderr
#endif

#ifdef CLUT_META_TESTING
#define RETURN_IF_FAILED                                                                                                                                                                                                                                           \
  do {                                                                                                                                                                                                                                                             \
  } while (0)
#else
#define RETURN_IF_FAILED                                                                                                                                                                                                                                           \
  do {                                                                                                                                                                                                                                                             \
    if (Clut.current.failed)                                                                                                                                                                                                                                       \
      return;                                                                                                                                                                                                                                                      \
  } while (0)
#endif

#define CLUT_START_FAILURE_LOG(file, line, msg)                                                                                                                                                                                                                    \
  do {                                                                                                                                                                                                                                                             \
    ClutFail();                                                                                                                                                                                                                                                    \
    ClutBeginTestLog((file), (line));                                                                                                                                                                                                                              \
    ClutPrintFail();                                                                                                                                                                                                                                               \
    if (msg) {                                                                                                                                                                                                                                                     \
      ClutPrint(msg);                                                                                                                                                                                                                                              \
    } else {

#define CLUT_END_FAILURE_LOG()                                                                                                                                                                                                                                     \
  }                                                                                                                                                                                                                                                                \
  ClutEndTestLog();                                                                                                                                                                                                                                                \
  }                                                                                                                                                                                                                                                                \
  while (0)

#define CLUT_BEFORE_ALL(hook_fn) Clut.hooks.before_all = (hook_fn)
#define CLUT_BEFORE_EACH(hook_fn) Clut.hooks.before_each = (hook_fn)
#define CLUT_AFTER_ALL(hook_fn) Clut.hooks.after_all = (hook_fn)
#define CLUT_AFTER_EACH(hook_fn) Clut.hooks.after_each = (hook_fn)

#define TEST_BEGIN() ClutTestBegin(__FILE__)
#define TEST_RUN(test_fn) ClutTestRun((test_fn), __LINE__, #test_fn)
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

void ClutReset();
void ClutTestBegin(const char *file);
void ClutTestRun(ClutTestFn test_fn, const int line, const char *test_fn_name);
int ClutTestEnd();

void ClutBeginTestLog(const char *file, const int line);
void ClutEndTestLog();

void ClutPrint(const char *str);
void ClutPrintChar(const char c);
void ClutPrintInt(int number);
void ClutPrintUint(size_t number);
void ClutPrintFloat(float number);
void ClutPrintDouble(double number);
void ClutPrintPtr(void *ptr);
void ClutPrintHex(int value);
void ClutPrintf(const char *fmt, ...);

void ClutPrintFail();
void ClutPrintExpectedActualChar(char expected, char actual, const char *opStr);
void ClutPrintExpectedActualInt(int expected, int actual, const char *opStr);
void ClutPrintExpectedActualUint(size_t expected, size_t actual, const char *opStr);
void ClutPrintExpectedActualFloat(float expected, float actual, const char *opStr);
void ClutPrintExpectedActualDouble(double expected, double actual, const char *opStr);
void ClutPrintExpectedActualString(const char *expected, const char *actual, const char *opStr);
void ClutPrintExpectedActualStringLen(const char *expected, const char *actual, size_t len, const char *opStr);
void ClutPrintExpectedActualPtr(void *expected, void *actual, const char *opStr);

void ClutFail();
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

void ClutTestAssertCompareInternalInt(bool condition, int expected, int actual, const char *file, int line, const char *msg, const char *opStr);
void ClutTestAssertCompareInternalUint(bool condition, size_t expected, size_t actual, const char *file, int line, const char *msg, const char *opStr);
void ClutTestAssertCompareInternalFloat(bool condition, float expected, float actual, const char *file, int line, const char *msg, const char *opStr);
void ClutTestAssertCompareInternalDouble(bool condition, double expected, double actual, const char *file, int line, const char *msg, const char *opStr);

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

#ifdef CLUT_IMPLEMENTATION

#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

ClutData Clut = {};

void ClutReset() {
  Clut.runner.total_tests = 0;
  Clut.runner.failures = 0;
  Clut.runner.start_time = 0;
  Clut.runner.stream = CLUT_STREAM_DEFAULT;

  Clut.current.file = NULL;
  Clut.current.name = NULL;
  Clut.current.failed = false;
}

void ClutTestBegin(const char *file) {
  ClutReset();
  if (Clut.hooks.before_all)
    Clut.hooks.before_all();
  Clut.current.file = file;
  Clut.runner.start_time = clock();
}

void ClutTestRun(ClutTestFn test_fn, const int line, const char *test_fn_name) {
#ifndef CLUT_META_TESTING
  Clut.runner.total_tests++;
#endif
  if (Clut.hooks.before_each)
    Clut.hooks.before_each();
  Clut.current.failed = false;
  Clut.current.name = test_fn_name;
  test_fn();
  if (!Clut.current.failed) {
    ClutBeginTestLog(Clut.current.file, line);
    ClutPrint(CLUT_STR_PASSED);
    ClutEndTestLog();
  }
  if (Clut.hooks.after_each)
    Clut.hooks.after_each();
}

int ClutTestEnd() {
  int end_time = clock();
  if (Clut.hooks.after_all)
    Clut.hooks.after_all();
  if (Clut.runner.total_tests == 0)
    return Clut.runner.failures;

  double total_time = ((double)(end_time - Clut.runner.start_time)) / CLOCKS_PER_SEC;

  size_t total_tests = Clut.runner.total_tests;
  size_t failures = Clut.runner.failures;

  ClutReset();

  ClutPrintf("------------------------\n"
             "%zu Tests %zu Failures - Total Time %.2fs\n",
             total_tests, failures, total_time);

  return failures;
}

void ClutBeginTestLog(const char *file, const int line) {
  ClutPrint(file);
  ClutPrintChar(':');
  ClutPrintInt(line);
  ClutPrintChar(':');
  ClutPrint(Clut.current.name);
  ClutPrintChar(':');
}

void ClutEndTestLog() { ClutPrintChar('\n'); }

void ClutPrint(const char *str) {
  if (str == NULL) {
    str = "(null)";
  }
  fprintf(Clut.runner.stream, "%s", str);
}
void ClutPrintChar(const char c) { fprintf(Clut.runner.stream, "%c", c); }
void ClutPrintInt(int number) { fprintf(Clut.runner.stream, "%d", number); }
void ClutPrintUint(size_t number) { fprintf(Clut.runner.stream, "%zu", number); }
void ClutPrintFloat(float number) { fprintf(Clut.runner.stream, "%f", number); }
void ClutPrintDouble(double number) { fprintf(Clut.runner.stream, "%f", number); }
void ClutPrintPtr(void *ptr) { fprintf(Clut.runner.stream, "%p", ptr); }
void ClutPrintHex(int value) { fprintf(Clut.runner.stream, "0x%02X", value); }
void ClutPrintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(Clut.runner.stream, fmt, args);
  va_end(args);
}

void ClutPrintFail() {
  ClutPrint(CLUT_STR_FAIL);
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
  ClutPrint(expected);
  ClutPrint(opStr);
  ClutPrint(actual);
}

void ClutPrintExpectedActualStringLen(const char *expected, const char *actual, size_t len, const char *opSrt) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrint(expected);
  ClutPrint(opSrt);
  ClutPrint(actual);
  ClutPrint(CLUT_STR_UNTIL_LEN);
  ClutPrintInt(len);
}

void ClutPrintExpectedActualPtr(void *expected, void *actual, const char *opStr) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintPtr(expected);
  ClutPrint(opStr);
  ClutPrintPtr(actual);
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

void ClutFail() {
#ifndef CLUT_META_TESTING
  Clut.runner.failures++;
#endif
  Clut.current.failed = true;
  Clut.runner.stream = CLUT_STREAM_FAIL;
}

void ClutTestAssert(bool condition, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (!condition) {
    ClutFail();
    ClutBeginTestLog(file, line);
    ClutPrintFail();
    ClutPrint(msg);
    ClutEndTestLog();
  }
}

void ClutTestAssertEqualChar(char expected, char actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected == actual) {
    return;
  }

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualChar(expected, actual, CLUT_STR_RECEIVED);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertEqualInt(int expected, int actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected == actual) {
    return;
  }

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualInt(expected, actual, CLUT_STR_RECEIVED);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertEqualUint(size_t expected, size_t actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected == actual) {
    return;
  }

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualUint(expected, actual, CLUT_STR_RECEIVED);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertEqualFloat(float expected, float actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (fabsf(expected - actual) < CLUT_FLOAT_EPSILON) {
    return;
  }

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualFloat(expected, actual, CLUT_STR_RECEIVED);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertEqualDouble(double expected, double actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (fabs(expected - actual) < CLUT_DOUBLE_EPSILON) {
    return;
  }

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualDouble(expected, actual, CLUT_STR_RECEIVED);
  CLUT_END_FAILURE_LOG();
}

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

void ClutTestAssertNotEqualChar(char expected, char actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected != actual) {
    return;
  }

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualChar(actual, expected, CLUT_STR_NOT_EQUAL);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertNotEqualInt(int expected, int actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected != actual) {
    return;
  }

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualInt(actual, expected, CLUT_STR_NOT_EQUAL);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertNotEqualUint(size_t expected, size_t actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected != actual) {
    return;
  }

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualUint(actual, expected, CLUT_STR_NOT_EQUAL);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertNotEqualFloat(float expected, float actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (fabsf(expected - actual) >= CLUT_FLOAT_EPSILON) {
    return;
  }

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualFloat(actual, expected, CLUT_STR_NOT_EQUAL);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertNotEqualDouble(double expected, double actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (fabs(expected - actual) >= CLUT_DOUBLE_EPSILON) {
    return;
  }

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualDouble(actual, expected, CLUT_STR_NOT_EQUAL);
  CLUT_END_FAILURE_LOG();
}

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

void ClutAssertCompareInternalInt(bool condition, int expected, int actual, const char *file, int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;

  if (condition)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualInt(actual, expected, opStr);
  CLUT_END_FAILURE_LOG();
}

void ClutAssertCompareInternalUint(bool condition, size_t expected, size_t actual, const char *file, int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;

  if (condition)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualUint(actual, expected, opStr);
  CLUT_END_FAILURE_LOG();
}

void ClutAssertCompareInternalFloat(bool condition, float expected, float actual, const char *file, int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;

  if (condition)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualFloat(actual, expected, opStr);
  CLUT_END_FAILURE_LOG();
}

void ClutAssertCompareInternalDouble(bool condition, double expected, double actual, const char *file, int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;

  if (condition)
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintExpectedActualDouble(actual, expected, opStr);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertIntGreaterThan(int expected, int actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalInt(actual > expected, expected, actual, file, line, msg, CLUT_STR_GREATER_THAN); }
void ClutTestAssertIntGreaterOrEqual(int expected, int actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalInt(actual >= expected, expected, actual, file, line, msg, CLUT_STR_GREATER_OR_EQUAL); }
void ClutTestAssertIntLessThan(int expected, int actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalInt(actual < expected, expected, actual, file, line, msg, CLUT_STR_LESS_THAN); }
void ClutTestAssertIntLessOrEqual(int expected, int actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalInt(actual <= expected, expected, actual, file, line, msg, CLUT_STR_LESS_OR_EQUAL); }

void ClutTestAssertUintGreaterThan(size_t expected, size_t actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalUint(actual > expected, expected, actual, file, line, msg, CLUT_STR_GREATER_THAN); }
void ClutTestAssertUintGreaterOrEqual(size_t expected, size_t actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalUint(actual >= expected, expected, actual, file, line, msg, CLUT_STR_GREATER_OR_EQUAL); }
void ClutTestAssertUintLessThan(size_t expected, size_t actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalUint(actual < expected, expected, actual, file, line, msg, CLUT_STR_LESS_THAN); }
void ClutTestAssertUintLessOrEqual(size_t expected, size_t actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalUint(actual <= expected, expected, actual, file, line, msg, CLUT_STR_LESS_OR_EQUAL); }

void ClutTestAssertFloatGreaterThan(float expected, float actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalFloat(actual > expected + CLUT_FLOAT_EPSILON, expected, actual, file, line, msg, CLUT_STR_GREATER_THAN); }
void ClutTestAssertFloatGreaterOrEqual(float expected, float actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalFloat(actual >= expected - CLUT_FLOAT_EPSILON, expected, actual, file, line, msg, CLUT_STR_GREATER_OR_EQUAL); }
void ClutTestAssertFloatLessThan(float expected, float actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalFloat(actual < expected - CLUT_FLOAT_EPSILON, expected, actual, file, line, msg, CLUT_STR_LESS_THAN); }
void ClutTestAssertFloatLessOrEqual(float expected, float actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalFloat(actual <= expected + CLUT_FLOAT_EPSILON, expected, actual, file, line, msg, CLUT_STR_LESS_OR_EQUAL); }

void ClutTestAssertDoubleGreaterThan(double expected, double actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalDouble(actual > expected + CLUT_DOUBLE_EPSILON, expected, actual, file, line, msg, CLUT_STR_GREATER_THAN); }
void ClutTestAssertDoubleGreaterOrEqual(double expected, double actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalDouble(actual >= expected - CLUT_DOUBLE_EPSILON, expected, actual, file, line, msg, CLUT_STR_GREATER_OR_EQUAL); }
void ClutTestAssertDoubleLessThan(double expected, double actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalDouble(actual < expected - CLUT_DOUBLE_EPSILON, expected, actual, file, line, msg, CLUT_STR_LESS_THAN); }
void ClutTestAssertDoubleLessOrEqual(double expected, double actual, const char *file, int line, const char *msg) { ClutAssertCompareInternalDouble(actual <= expected + CLUT_DOUBLE_EPSILON, expected, actual, file, line, msg, CLUT_STR_LESS_OR_EQUAL); }

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

  float diff = fabsf(actual - expected);

  if (diff <= (delta + CLUT_FLOAT_EPSILON))
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintWithinDiffFloat(expected, delta, diff);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertWithinDouble(double expected, double delta, double actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  double diff = fabs(actual - expected);

  if (diff <= (delta + CLUT_DOUBLE_EPSILON))
    return;

  CLUT_START_FAILURE_LOG(file, line, msg);
  ClutPrintWithinDiffDouble(expected, delta, diff);
  CLUT_END_FAILURE_LOG();
}

void ClutTestAssertEqualMemory(const void *expected, const void *actual, size_t len, size_t num_elements, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (num_elements == 0 || len == 0)
    return;

  if (expected == actual)
    return;

  if (expected == NULL || actual == NULL) {
    CLUT_START_FAILURE_LOG(file, line, msg);
    ClutPrint(CLUT_STR_MEMORY_NULL);
    CLUT_END_FAILURE_LOG();
    return;
  }

  const unsigned char *ptr_exp = (const unsigned char *)expected;
  const unsigned char *ptr_act = (const unsigned char *)actual;

  for (size_t elem = 0; elem < num_elements; elem++) {
    for (size_t byte = 0; byte < len; byte++) {
      size_t offset = (elem * len) + byte;

      if (ptr_exp[offset] != ptr_act[offset]) {
        CLUT_START_FAILURE_LOG(file, line, msg);

        if (num_elements > 1) {
          ClutPrint(CLUT_STR_ARRAY_MISMATCH_AT);
          ClutPrintUint(elem);
          ClutPrint(CLUT_STR_ARRAY_BYTE);
          ClutPrintUint(byte);
          ClutPrint("]. ");
        } else {
          ClutPrint(CLUT_STR_MISMATCH_AT);
          ClutPrintUint(byte);
          ClutPrint(". ");
        }

        ClutPrint(CLUT_STR_EXPECTED);
        ClutPrintHex(ptr_exp[offset]);
        ClutPrint(CLUT_STR_WAS);
        ClutPrintHex(ptr_act[offset]);

        CLUT_END_FAILURE_LOG();
        return;
      }
    }
  }
}
#endif
#endif
