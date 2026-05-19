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

#ifndef CLUT_FLOAT_EPSILON
#define CLUT_FLOAT_EPSILON 1e-5f
#endif

#ifndef CLUT_DOUBLE_EPSILON
#define CLUT_DOUBLE_EPSILON 1e-9
#endif

typedef void (*ClutTestFunction)(void);

typedef struct ClutData {
  size_t total_tests;
  size_t failures;
  const char *current_test_file;
  const char *current_test_function_name;
  bool current_test_failed;
  long start_time;
  FILE *stream;
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
    if (Clut.current_test_failed)                                                                                                                                                                                                                                  \
      return;                                                                                                                                                                                                                                                      \
  } while (0)
#endif

#define TEST_BEGIN() ClutTestBegin(__FILE__)
#define TEST_RUN(clut_test_function) ClutTestRun((clut_test_function), __LINE__, #clut_test_function)
#define TEST_END() ClutTestEnd()

/* Assertions */

#define TEST_ASSERT(condition) ClutTestAssert((condition), __FILE__, __LINE__, "Expression Is False -> " #condition)
#define TEST_ASSERT_TRUE(condition) ClutTestAssert((condition), __FILE__, __LINE__, "Expected True But Was False -> " #condition)
#define TEST_ASSERT_FALSE(condition) ClutTestAssert(!(condition), __FILE__, __LINE__, "Expected False But Was True -> " #condition)
#define TEST_ASSERT_UNLESS(condition) ClutTestAssert(!(condition), __FILE__, __LINE__, "Expression Is True -> " #condition)
#define TEST_ASSERT_NULL(pointer) ClutTestAssert(((pointer) == NULL), __FILE__, __LINE__, "Expected NULL")
#define TEST_ASSERT_NOT_NULL(pointer) ClutTestAssert(((pointer) != NULL), __FILE__, __LINE__, "Expected Non-NULL")

#define TEST_ASSERT_EQUAL_INT(expected, actual) ClutTestAssertEqualInt((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT(expected, actual) ClutTestAssertEqualUint((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_FLOAT(expected, actual) ClutTestAssertEqualFloat((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_DOUBLE(expected, actual) ClutTestAssertEqualDouble((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING(expected, actual) ClutTestAssertEqualString((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len) ClutTestAssertEqualStringLen((expected), (actual), (len), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_PTR(expected, actual) ClutTestAssertEqualPtr((expected), (actual), __FILE__, __LINE__, NULL)

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

/* Messages */

#define TEST_ASSERT_MESSAGE(condition, mgs) ClutTestAssert((condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_TRUE_MESSAGE(condition, msg) ClutTestAssert((condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_FALSE_MESSAGE(condition, msg) ClutTestAssert(!(condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_UNLESS_MESSAGE(condition, mgs) ClutTestAssert(!(condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NULL_MESSAGE(pointer, msg) ClutTestAssert(((pointer) == NULL), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_NULL_MESSAGE(pointer, msg) ClutTestAssert(((pointer) != NULL), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, msg) ClutTestAssertEqualInt((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_UINT_MESSAGE(expected, actual, msg) ClutTestAssertEqualUint((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected, actual, msg) ClutTestAssertEqualFloat((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(expected, actual, msg) ClutTestAssertEqualDouble((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, msg) ClutTestAssertEqualString((const char *)(expected), (const char *)(actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(expected, actual, len, msg) ClutTestAssertEqualStringLen((expected), (actual), (len), __FILE__, __LINE__, (msg))

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

void ClutReset();
void ClutTestBegin(const char *file);
void ClutTestRun(ClutTestFunction clut_test_function, const int line, const char *clut_test_name);
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
void ClutPrintf(const char *fmt, ...);

void ClutPrintFail();
void ClutPrintExpectedActualInt(int expected, int actual, const char *opStr);
void ClutPrintExpectedActualUint(size_t expected, size_t actual, const char *opStr);
void ClutPrintExpectedActualFloat(float expected, float actual, const char *opStr);
void ClutPrintExpectedActualDouble(double expected, double actual, const char *opStr);
void ClutPrintExpectedActualString(const char *expected, const char *actual, const char *opStr);
void ClutPrintExpectedActualStringLen(const char *expected, const char *actual, size_t len, const char *opStr);
void ClutPrintExpectedActualPtr(void *expected, void *actual, const char *opStr);

void ClutFail();
void ClutTestAssert(bool condition, const char *file, const int line, const char *msg);
void ClutTestAssertEqualInt(int expected, int actual, const char *file, const int line, const char *msg);
void ClutTestAssertEqualUint(size_t expected, size_t actual, const char *file, const int line, const char *msg);
void ClutTestAssertEqualFloat(float expected, float actual, const char *file, const int line, const char *msg);
void ClutTestAssertEqualDouble(double expected, double actual, const char *file, const int line, const char *msg);
void ClutTestAssertEqualString(const char *expected, const char *actual, const char *file, const int line, const char *msg);
void ClutTestAssertEqualStringLen(const char *expected, const char *actual, size_t len, const char *file, const int line, const char *msg);
void ClutTestAssertEqualPtr(void *expected, void *actual, const char *file, const int line, const char *msg);

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

#ifdef CLUT_IMPLEMENTATION

#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

ClutData Clut = {};

void ClutReset() {
  Clut.total_tests = 0;
  Clut.failures = 0;
  Clut.current_test_file = NULL;
  Clut.current_test_function_name = NULL;
  Clut.current_test_failed = false;
  Clut.start_time = 0;
  Clut.stream = CLUT_STREAM_DEFAULT;
}

void ClutTestBegin(const char *file) {
  ClutReset();
  Clut.current_test_file = file;
  Clut.start_time = clock();
}

void ClutTestRun(ClutTestFunction clut_test_function, const int line, const char *clut_test_function_name) {
#ifndef CLUT_META_TESTING
  Clut.total_tests++;
#endif
  Clut.current_test_failed = false;
  Clut.current_test_function_name = clut_test_function_name;
  clut_test_function();
  if (!Clut.current_test_failed) {
    ClutBeginTestLog(Clut.current_test_file, line);
    ClutPrint(CLUT_STR_PASSED);
    ClutEndTestLog();
  }
}

int ClutTestEnd() {
  if (Clut.total_tests == 0)
    return Clut.failures;

  int end_time = clock();
  double total_time = ((double)(end_time - Clut.start_time)) / CLOCKS_PER_SEC;

  size_t total_tests = Clut.total_tests;
  size_t failures = Clut.failures;

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
  ClutPrint(Clut.current_test_function_name);
  ClutPrintChar(':');
}

void ClutEndTestLog() { ClutPrintChar('\n'); }

void ClutPrint(const char *str) {
  if (str == NULL) {
    str = "(null)";
  }
  fprintf(Clut.stream, "%s", str);
}
void ClutPrintChar(const char c) { fprintf(Clut.stream, "%c", c); }
void ClutPrintInt(int number) { fprintf(Clut.stream, "%d", number); }
void ClutPrintUint(size_t number) { fprintf(Clut.stream, "%zu", number); }
void ClutPrintPtr(void *ptr) { fprintf(Clut.stream, "%p", ptr); }
void ClutPrintFloat(float number) { fprintf(Clut.stream, "%f", number); }
void ClutPrintDouble(double number) { fprintf(Clut.stream, "%f", number); }
void ClutPrintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(Clut.stream, fmt, args);
  va_end(args);
}

void ClutPrintFail() {
  ClutPrint(CLUT_STR_FAIL);
  ClutPrintChar(':');
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

void ClutFail() {
#ifndef CLUT_META_TESTING
  Clut.failures++;
#endif
  Clut.current_test_failed = true;
  Clut.stream = CLUT_STREAM_FAIL;
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

void ClutTestAssertEqualInt(int expected, int actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;
  if (expected == actual) {
    return;
  }

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualInt(expected, actual, CLUT_STR_RECEIVED);
  }
  ClutEndTestLog();
}

void ClutTestAssertEqualUint(size_t expected, size_t actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;
  if (expected == actual) {
    return;
  }

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualUint(expected, actual, CLUT_STR_RECEIVED);
  }
  ClutEndTestLog();
}

void ClutTestAssertEqualFloat(float expected, float actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;
  if (fabsf(expected - actual) < CLUT_FLOAT_EPSILON) {
    return;
  }
  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualFloat(expected, actual, CLUT_STR_RECEIVED);
  }
  ClutEndTestLog();
}

void ClutTestAssertEqualDouble(double expected, double actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;
  if (fabs(expected - actual) < CLUT_DOUBLE_EPSILON) {
    return;
  }
  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualDouble(expected, actual, CLUT_STR_RECEIVED);
  }
  ClutEndTestLog();
}

void ClutTestAssertEqualString(const char *expected, const char *actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected == actual)
    return;

  if (expected == NULL || actual == NULL) {
    Clut.current_test_failed = true;
  } else {
    for (size_t i = 0; expected[i] || actual[i]; i++) {
      if (expected[i] != actual[i]) {
        Clut.current_test_failed = true;
        break;
      }
    }
  }

  if (!Clut.current_test_failed)
    return;

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualString(actual, expected, CLUT_STR_EQUAL);
  }
  ClutEndTestLog();
}

void ClutTestAssertEqualStringLen(const char *expected, const char *actual, size_t len, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected == actual)
    return;

  if (expected == NULL || actual == NULL) {
    Clut.current_test_failed = true;
  } else {
    for (size_t i = 0; i < len && (expected[i] || actual[i]); i++) {
      if (expected[i] != actual[i]) {
        Clut.current_test_failed = true;
        break;
      }
    }
  }

  if (!Clut.current_test_failed)
    return;

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualStringLen(actual, expected, len, CLUT_STR_EQUAL);
  }
  ClutEndTestLog();
}

void ClutTestAssertEqualPtr(void *expected, void *actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;
  if (expected == actual)
    return;

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualPtr(expected, actual, CLUT_STR_RECEIVED);
  }
  ClutEndTestLog();
}

void ClutTestAssertNotEqualInt(int expected, int actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;
  if (expected != actual) {
    return;
  }

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualInt(actual, expected, CLUT_STR_NOT_EQUAL);
  }
  ClutEndTestLog();
}

void ClutTestAssertNotEqualUint(size_t expected, size_t actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;
  if (expected != actual) {
    return;
  }

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualUint(actual, expected, CLUT_STR_NOT_EQUAL);
  }
  ClutEndTestLog();
}

void ClutTestAssertNotEqualFloat(float expected, float actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;
  if (fabsf(expected - actual) >= CLUT_FLOAT_EPSILON) {
    return;
  }

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualFloat(actual, expected, CLUT_STR_NOT_EQUAL);
  }
  ClutEndTestLog();
}

void ClutTestAssertNotEqualDouble(double expected, double actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;
  if (fabs(expected - actual) >= CLUT_DOUBLE_EPSILON) {
    return;
  }

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualDouble(actual, expected, CLUT_STR_NOT_EQUAL);
  }
  ClutEndTestLog();
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

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualString(actual, expected, CLUT_STR_NOT_EQUAL);
  }
  ClutEndTestLog();
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

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualStringLen(actual, expected, len, CLUT_STR_NOT_EQUAL);
  }
  ClutEndTestLog();
}

void ClutTestAssertNotEqualPtr(void *expected, void *actual, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;

  if (expected != actual)
    return;

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualPtr(actual, expected, CLUT_STR_NOT_EQUAL);
  }
  ClutEndTestLog();
}

void ClutAssertCompareInternalInt(bool condition, int expected, int actual, const char *file, int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;

  if (condition)
    return;

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualInt(actual, expected, opStr);
  }
  ClutEndTestLog();
}

void ClutAssertCompareInternalUint(bool condition, size_t expected, size_t actual, const char *file, int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;
  if (condition)
    return;

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualUint(actual, expected, opStr);
  }
  ClutEndTestLog();
}

void ClutAssertCompareInternalFloat(bool condition, float expected, float actual, const char *file, int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;

  if (condition)
    return;

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualFloat(actual, expected, opStr);
  }
  ClutEndTestLog();
}

void ClutAssertCompareInternalDouble(bool condition, double expected, double actual, const char *file, int line, const char *msg, const char *opStr) {
  RETURN_IF_FAILED;

  if (condition)
    return;

  ClutFail();
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualDouble(actual, expected, opStr);
  }
  ClutEndTestLog();
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

#endif
#endif
