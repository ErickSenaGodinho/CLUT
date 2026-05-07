#ifndef INCLUDE_CLUT_H
#define INCLUDE_CLUT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const char ClutStrFail[] = "FAIL";

#define CLUT_TEST_FAIL(file, line, message) ClutFail((file), (line), (message))

#define TEST_ASSERT(condition)                                                 \
  CLUT_TEST_ASSERT((condition), __FILE__, __LINE__, " Expression Is False");

#define TEST_ASSERT_FALSE(condition)                                           \
  CLUT_TEST_ASSERT(!(condition), __LINE__, " Expected FALSE But Was TRUE")

#define CLUT_TEST_ASSERT(condition, file, line, message)                       \
  do {                                                                         \
    if (!(condition)) {                                                        \
      CLUT_TEST_FAIL((file), (line), (message));                               \
    }                                                                          \
  } while (0)

#ifdef CLUT_IMPLEMENTATION
static void ClutFail(const char *file, const int line, const char *message) {
  fprintf(stderr, "%s:%d: %s: %s\n", file, line, ClutStrFail, message);
  exit(1);
}
#endif

#endif
