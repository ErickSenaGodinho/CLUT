#ifndef INCLUDE_CLUT_H
#define INCLUDE_CLUT_H

#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const char ClutStrFail[] = "FAIL";

#define TEST_ASSERT(condition) ClutTestAssert((condition), __FILE__, __LINE__, "Expression Is False -> " #condition)
#define TEST_ASSERT_TRUE(condition) ClutTestAssert((condition), __FILE__, __LINE__, "Expected True But Was False -> " #condition)
#define TEST_ASSERT_FALSE(condition) ClutTestAssert(!(condition), __FILE__, __LINE__, "Expected False But Was True -> " #condition)
#define TEST_ASSERT_UNLESS(condition) ClutTestAssert(!(condition), __FILE__, __LINE__, "Expression Is True -> " #condition)
#define TEST_ASSERT_EQUALS_INT(expected, actual) ClutTestAssertEqualsInt((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUALS_FLOAT(expected, actual) ClutTestAssertEqualsFloat((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUALS_DOUBLE(expected, actual) ClutTestAssertEqualsDouble((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_EQUALS_INT_MESSAGE(expected, actual, msg) ClutTestAssertEqualsInt((expected), (actual), __FILE__, __LINE__, msg)
#define TEST_ASSERT_EQUALS_FLOAT_MESSAGE(expected, actual, msg) ClutTestAssertEqualsFloat((expected), (actual), __FILE__, __LINE__, msg)
#define TEST_ASSERT_EQUALS_DOUBLE_MESSAGE(expected, actual, msg) ClutTestAssertEqualsDouble((expected), (actual), __FILE__, __LINE__, msg)

void ClutTestAssert(bool condition, const char *file, const int line, const char *msg);
void ClutTestAssertEqualsInt(int expected, int actual, const char *file, const int line, char *msg);
void ClutTestAssertEqualsFloat(float expected, float actual, const char *file, const int line, char *msg);
void ClutTestAssertEqualsDouble(double expected, double actual, const char *file, const int line, char *msg);

#ifdef CLUT_IMPLEMENTATION
static void ClutFail(const char *file, const int line, const char *msg) {
  fprintf(stderr, "%s:%d: %s: %s\n", file, line, ClutStrFail, msg);
  exit(1);
}

void ClutTestAssert(bool condition, const char *file, const int line, const char *msg) {
  if (!condition)
    ClutFail(file, line, msg);
}

static void ClutFormatMessage(char *buffer, size_t size, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, size, fmt, args);
  va_end(args);
}

void ClutTestAssertEqualsInt(int expected, int actual, const char *file, const int line, char *msg) {
  if (expected == actual) {
    return;
  }
  char buffer[255];
  const char *msg_to_print = msg;
  if (msg == NULL) {
    ClutFormatMessage(buffer, sizeof(buffer), "Expected: %d, Received: %d", expected, actual);
    msg_to_print = buffer;
  }
  ClutFail(file, line, msg_to_print);
}

void ClutTestAssertEqualsFloat(float expected, float actual, const char *file, const int line, char *msg) {
  if (fabsf(expected - actual) < FLT_EPSILON) {
    return;
  }
  char buffer[255];
  const char *msg_to_print = msg;
  if (msg == NULL) {
    ClutFormatMessage(buffer, sizeof(buffer), "Expected: %f, Received: %f", expected, actual);
    msg_to_print = buffer;
  }
  ClutFail(file, line, msg_to_print);
}

void ClutTestAssertEqualsDouble(double expected, double actual, const char *file, const int line, char *msg) {
  if (fabs(expected - actual) < DBL_EPSILON) {
    return;
  }
  char buffer[255];
  const char *msg_to_print = msg;
  if (msg == NULL) {
    ClutFormatMessage(buffer, sizeof(buffer), "Expected: %f, Received: %f", expected, actual);
    msg_to_print = buffer;
  }
  ClutFail(file, line, msg_to_print);
}

#endif
#endif
