#ifndef INCLUDE_CLUT_H
#define INCLUDE_CLUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef CLUT_OUTPUT_COLOR
const char ClutStrFail[] = "\033[31mFAIL\033[0m";
const char ClutStrPass[] = "\033[32mPASS\033[0m";
#else
const char ClutStrFail[] = "FAIL";
const char ClutStrPass[] = "PASS";
#endif

#define CLUT_STR_EXPECTED "Expected: "
#define CLUT_STR_RECEIVED " Received: "

#define CLUT_MSG_BUFFER_SIZE 256

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

#define RETURN_IF_FAILED                                                                                                                                                           \
  do {                                                                                                                                                                             \
    if (Clut.current_test_failed) {                                                                                                                                                \
      return;                                                                                                                                                                      \
    }                                                                                                                                                                              \
  } while (0)

#define TEST_BEGIN() ClutTestBegin(__FILE__)
#define TEST_RUN(clut_test_function) ClutTestRun((clut_test_function), __LINE__, #clut_test_function)
#define TEST_END() ClutTestEnd()

#define TEST_ASSERT(condition) ClutTestAssert((condition), __FILE__, __LINE__, "Expression Is False -> " #condition)
#define TEST_ASSERT_TRUE(condition) ClutTestAssert((condition), __FILE__, __LINE__, "Expected True But Was False -> " #condition)
#define TEST_ASSERT_FALSE(condition) ClutTestAssert(!(condition), __FILE__, __LINE__, "Expected False But Was True -> " #condition)
#define TEST_ASSERT_UNLESS(condition) ClutTestAssert(!(condition), __FILE__, __LINE__, "Expression Is True -> " #condition)
#define TEST_ASSERT_EQUALS_INT(expected, actual) ClutTestAssertEqualsInt((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUALS_FLOAT(expected, actual) ClutTestAssertEqualsFloat((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUALS_DOUBLE(expected, actual) ClutTestAssertEqualsDouble((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUALS_STRING(expected, actual) ClutTestAssertEqualsString((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUALS_STRING_LEN(expected, actual, len) ClutTestAssertEqualsStringLen((expected), (actual), (len), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_EQUALS_INT_MESSAGE(expected, actual, msg) ClutTestAssertEqualsInt((expected), (actual), __FILE__, __LINE__, msg)
#define TEST_ASSERT_EQUALS_FLOAT_MESSAGE(expected, actual, msg) ClutTestAssertEqualsFloat((expected), (actual), __FILE__, __LINE__, msg)
#define TEST_ASSERT_EQUALS_DOUBLE_MESSAGE(expected, actual, msg) ClutTestAssertEqualsDouble((expected), (actual), __FILE__, __LINE__, msg)
#define TEST_ASSERT_EQUALS_STRING_MESSAGE(expected, actual, msg) ClutTestAssertEqualsString((const char *)(expected), (const char *)(actual), __FILE__, __LINE__, msg)
#define TEST_ASSERT_EQUALS_STRING_LEN_MESSAGE(expected, actual, len, msg) ClutTestAssertEqualsStringLen((expected), (actual), (len), __FILE__, __LINE__, msg)

void ClutReset();
void ClutTestBegin(const char *file);
void ClutTestRun(ClutTestFunction clut_test_function, const int line, const char *clut_test_name);
int ClutTestEnd();

void ClutBeginTestLog(const char *file, const int line);
void ClutEndTestLog();

void ClutPrint(const char *str);
void ClutPrintChar(const char c);
void ClutPrintInt(int number);
void ClutPrintFloat(float number);
void ClutPrintDouble(double number);

void ClutPrintFail();
void ClutPrintExpectedActualInt(int expected, int actual);
void ClutPrintExpectedActualFloat(float expected, float actual);
void ClutPrintExpectedActualDouble(double expected, double actual);
void ClutPrintExpectedActualString(const char *expected, const char *actual);
void ClutPrintExpectedActualStringLen(const char *expected, const char *actual, size_t len);
void ClutPrintf(const char *fmt, ...);

void ClutFail();
void ClutTestAssert(bool condition, const char *file, const int line, const char *msg);
void ClutTestAssertEqualsInt(int expected, int actual, const char *file, const int line, char *msg);
void ClutTestAssertEqualsFloat(float expected, float actual, const char *file, const int line, char *msg);
void ClutTestAssertEqualsDouble(double expected, double actual, const char *file, const int line, char *msg);
void ClutTestAssertEqualsString(const char *expected, const char *actual, const char *file, const int line, char *msg);
void ClutTestAssertEqualsStringLen(const char *expected, const char *actual, size_t len, const char *file, const int line, char *msg);

#ifdef CLUT_IMPLEMENTATION

#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
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
  Clut.stream = stdout;
}

void ClutTestBegin(const char *file) {
  ClutReset();
  Clut.current_test_file = file;
  Clut.start_time = clock();
}

void ClutTestRun(ClutTestFunction clut_test_function, const int line, const char *clut_test_function_name) {
  Clut.total_tests++;
  Clut.current_test_failed = false;
  Clut.current_test_function_name = clut_test_function_name;
  clut_test_function();
  if (!Clut.current_test_failed) {
    ClutBeginTestLog(Clut.current_test_file, line);
    ClutPrint(ClutStrPass);
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
void ClutPrintFloat(float number) { fprintf(Clut.stream, "%f", number); }
void ClutPrintDouble(double number) { fprintf(Clut.stream, "%f", number); }
void ClutPrintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(Clut.stream, fmt, args);
  va_end(args);
}

void ClutPrintFail() {
  ClutPrint(ClutStrFail);
  ClutPrintChar(':');
}

void ClutPrintExpectedActualInt(int expected, int actual) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintInt(expected);
  ClutPrint(CLUT_STR_RECEIVED);
  ClutPrintInt(actual);
}

void ClutPrintExpectedActualFloat(float expected, float actual) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintFloat(expected);
  ClutPrint(CLUT_STR_RECEIVED);
  ClutPrintFloat(actual);
}

void ClutPrintExpectedActualDouble(double expected, double actual) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrintDouble(expected);
  ClutPrint(CLUT_STR_RECEIVED);
  ClutPrintDouble(actual);
}

void ClutPrintExpectedActualString(const char *expected, const char *actual) {
  ClutPrint(CLUT_STR_EXPECTED);
  ClutPrint(expected);
  ClutPrint(CLUT_STR_RECEIVED);
  ClutPrint(actual);
}

void ClutPrintExpectedActualStringLen(const char *expected, const char *actual, size_t len) { ClutPrintf("Expected %s to equal %s until index %zu", expected, actual, len); }

void ClutFail() {
  Clut.current_test_failed = true;
  Clut.failures++;
  Clut.stream = stderr;
}

void ClutTestAssert(bool condition, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;
  if (!condition) {
    ClutBeginTestLog(file, line);
    ClutPrintFail();
    ClutPrint(msg);
    ClutEndTestLog();
    ClutFail();
  }
}

void ClutTestAssertEqualsInt(int expected, int actual, const char *file, const int line, char *msg) {
  RETURN_IF_FAILED;
  if (expected == actual) {
    return;
  }
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualInt(expected, actual);
  }
  ClutEndTestLog();
  ClutFail();
}

void ClutTestAssertEqualsFloat(float expected, float actual, const char *file, const int line, char *msg) {
  RETURN_IF_FAILED;
  if (fabsf(expected - actual) < FLT_EPSILON) {
    return;
  }
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualFloat(expected, actual);
  }
  ClutEndTestLog();
  ClutFail();
}

void ClutTestAssertEqualsDouble(double expected, double actual, const char *file, const int line, char *msg) {
  RETURN_IF_FAILED;
  if (fabs(expected - actual) < DBL_EPSILON) {
    return;
  }
  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualDouble(expected, actual);
  }
  ClutEndTestLog();
  ClutFail();
}

void ClutTestAssertEqualsString(const char *expected, const char *actual, const char *file, const int line, char *msg) {
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

  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualString(expected, actual);
  }
  ClutEndTestLog();
  ClutFail();
}

void ClutTestAssertEqualsStringLen(const char *expected, const char *actual, size_t len, const char *file, const int line, char *msg) {
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

  ClutBeginTestLog(file, line);
  ClutPrintFail();
  if (msg) {
    ClutPrint(msg);
  } else {
    ClutPrintExpectedActualStringLen(expected, actual, len);
  }
  ClutEndTestLog();
  ClutFail();
}

#endif
#endif
