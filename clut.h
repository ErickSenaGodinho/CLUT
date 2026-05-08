#ifndef INCLUDE_CLUT_H
#define INCLUDE_CLUT_H

#include <stdbool.h>
#include <stddef.h>

#ifdef CLUT_OUTPUT_COLOR
const char ClutStrFail[] = "\033[31mFAIL\033[0m";
const char ClutStrPass[] = "\033[32mPASS\033[0m";
#else
const char ClutStrFail[] = "FAIL";
const char ClutStrPass[] = "PASS";
#endif

#define CLUT_MSG_BUFFER_SIZE 256

typedef void (*ClutTestFunction)(void);

typedef struct ClutData {
  size_t total_tests;
  size_t failures;
  const char *current_test_file;
  const char *current_test_function_name;
  bool current_test_failed;
  long start_time;
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

#define TEST_ASSERT_EQUALS_INT_MESSAGE(expected, actual, msg) ClutTestAssertEqualsInt((expected), (actual), __FILE__, __LINE__, msg)
#define TEST_ASSERT_EQUALS_FLOAT_MESSAGE(expected, actual, msg) ClutTestAssertEqualsFloat((expected), (actual), __FILE__, __LINE__, msg)
#define TEST_ASSERT_EQUALS_DOUBLE_MESSAGE(expected, actual, msg) ClutTestAssertEqualsDouble((expected), (actual), __FILE__, __LINE__, msg)
#define TEST_ASSERT_EQUALS_STRING_MESSAGE(expected, actual, msg) ClutTestAssertEqualsString((const char *)(expected), (const char *)(actual), __FILE__, __LINE__, msg)

void ClutReset();
void ClutTestBegin(const char *file);
int ClutTestEnd();
void ClutTestRun(ClutTestFunction clut_test_function, const int line, const char *clut_test_name);
void ClutFail(const char *file, const int line, const char *msg);
void ClutFormatMessage(char *buffer, size_t size, const char *fmt, ...);
void ClutTestAssert(bool condition, const char *file, const int line, const char *msg);
void ClutTestAssertEqualsInt(int expected, int actual, const char *file, const int line, char *msg);
void ClutTestAssertEqualsFloat(float expected, float actual, const char *file, const int line, char *msg);
void ClutTestAssertEqualsDouble(double expected, double actual, const char *file, const int line, char *msg);
void ClutTestAssertEqualsString(const char *expected, const char *actual, const char *file, const int line, char *msg);

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
    printf("%s:%d:%s:%s\n", Clut.current_test_file, line, clut_test_function_name, ClutStrPass);
  }
}

int ClutTestEnd() {
  if (Clut.total_tests == 0)
    return Clut.failures;

  int end_time = clock();
  double total_time = ((double)(end_time - Clut.start_time)) / CLOCKS_PER_SEC;
  size_t failures = Clut.failures;

  char buffer[CLUT_MSG_BUFFER_SIZE];
  ClutFormatMessage(buffer, sizeof(buffer),
                    "------------------------\n"
                    "%zu Tests %zu Failures - Total Time %.2fs\n",
                    Clut.total_tests, Clut.failures, total_time);
  printf("%s", buffer);

  ClutReset();
  return failures;
}

void ClutFail(const char *file, const int line, const char *msg) {
  if (Clut.current_test_function_name) {
    fprintf(stderr, "%s:%d:%s:%s:%s\n", file, line, Clut.current_test_function_name, ClutStrFail, msg);
  } else {
    fprintf(stderr, "%s:%d:%s:%s\n", file, line, ClutStrFail, msg);
  }
  Clut.current_test_failed = true;
  Clut.failures++;
}

void ClutFormatMessage(char *buffer, size_t size, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, size, fmt, args);
  va_end(args);
}

void ClutTestAssert(bool condition, const char *file, const int line, const char *msg) {
  RETURN_IF_FAILED;
  if (!condition)
    ClutFail(file, line, msg);
}

void ClutTestAssertEqualsInt(int expected, int actual, const char *file, const int line, char *msg) {
  RETURN_IF_FAILED;
  if (expected == actual) {
    return;
  }
  char buffer[CLUT_MSG_BUFFER_SIZE];
  const char *msg_to_print = msg;
  if (msg == NULL) {
    ClutFormatMessage(buffer, sizeof(buffer), "Expected: %d, Received: %d", expected, actual);
    msg_to_print = buffer;
  }
  ClutFail(file, line, msg_to_print);
}

void ClutTestAssertEqualsFloat(float expected, float actual, const char *file, const int line, char *msg) {
  RETURN_IF_FAILED;
  if (fabsf(expected - actual) < FLT_EPSILON) {
    return;
  }
  char buffer[CLUT_MSG_BUFFER_SIZE];
  const char *msg_to_print = msg;
  if (msg == NULL) {
    ClutFormatMessage(buffer, sizeof(buffer), "Expected: %f, Received: %f", expected, actual);
    msg_to_print = buffer;
  }
  ClutFail(file, line, msg_to_print);
}

void ClutTestAssertEqualsDouble(double expected, double actual, const char *file, const int line, char *msg) {
  RETURN_IF_FAILED;
  if (fabs(expected - actual) < DBL_EPSILON) {
    return;
  }
  char buffer[CLUT_MSG_BUFFER_SIZE];
  const char *msg_to_print = msg;
  if (msg == NULL) {
    ClutFormatMessage(buffer, sizeof(buffer), "Expected: %f, Received: %f", expected, actual);
    msg_to_print = buffer;
  }
  ClutFail(file, line, msg_to_print);
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

  char buffer[CLUT_MSG_BUFFER_SIZE];
  const char *msg_to_print = msg;
  if (msg == NULL) {
    ClutFormatMessage(buffer, sizeof(buffer), "Expected: %s, Received: %s", expected, actual);
    msg_to_print = buffer;
  }
  ClutFail(file, line, msg_to_print);
}

#endif
#endif
