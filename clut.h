#ifndef INCLUDE_CLUT_H
#define INCLUDE_CLUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

/* Abort / long-jump */
#ifndef CLUT_DISABLE_LONGJMP
#include <setjmp.h>
#define CLUT_HAS_LONGJMP
#endif

/* Linkage */
#ifdef CLUT_STATIC
#define CLUT_API static
#define CLUT_STATE_LINKAGE static
#else
#define CLUT_API
#define CLUT_STATE_LINKAGE
#endif

/* Assertion message strings */
#define CLUT_MSG_EXPECTED "Expected "
#define CLUT_MSG_RECEIVED " Received "
#define CLUT_MSG_EQUAL " to be equal to "
#define CLUT_MSG_NOT_EQUAL " to not be equal to "
#define CLUT_MSG_GREATER_THAN " to be greater than "
#define CLUT_MSG_LESS_THAN " to be less than "
#define CLUT_MSG_GREATER_OR_EQUAL " to be greater or equal than "
#define CLUT_MSG_LESS_OR_EQUAL " to be less or equal than "
#define CLUT_MSG_UNTIL_LEN " until length "
#define CLUT_MSG_WITHIN_DELTA " within +/- "
#define CLUT_MSG_BUT_DIFF " but actual diff was "
#define CLUT_MSG_MEMORY_NULL "One of the pointers is NULL"
#define CLUT_MSG_MISMATCH_AT "Memory mismatch at byte offset "
#define CLUT_MSG_ARRAY_MISMATCH_AT "Array mismatch at Element "
#define CLUT_MSG_ARRAY_INDEX_START "["
#define CLUT_MSG_ARRAY_INDEX_END "] "
#define CLUT_MSG_BYTE_OFFSET ", Byte Offset "
#define CLUT_MSG_WAS " but was "

/* String Builder */
typedef struct {
  char *items;
  size_t count;
  size_t capacity;
} ClutSB;

/* Suite Result */
typedef struct {
  size_t total_tests;
  size_t passed;
  size_t failed;
  size_t skipped;
  double total_seconds;
} ClutSuiteResult;

typedef struct {
  const char *file;
  int line;
  int iteration;
} ClutLogRecord;

/* Function-pointer types */
typedef struct {
  size_t current_repetition;
  size_t total_repetitions;
} ClutRepeatedTestInput;

typedef void (*ClutHookFn)();
typedef void (*ClutTestFn)();
typedef void (*ClutRepeatedTestFn)(ClutRepeatedTestInput);

/* Core data structures */
typedef struct {
  size_t total_failures;
  ClutSB failure_report;
  ClutSB failure_message;
} ClutRunner;

typedef struct {
  ClutTestFn fn;
  const char *name;
} ClutPendingTest;

typedef struct {
  ClutPendingTest *items;
  size_t count;
  size_t capacity;
} ClutPendingTests;

typedef struct {
  size_t total_tests;
  size_t passed;
  size_t failed;
  size_t skipped;
  long start_time;
  ClutPendingTests registered_tests;
} ClutSuite;

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
  bool skipped;
  int iteration;
#ifdef CLUT_HAS_LONGJMP
  jmp_buf jmp_buffer;
#endif
} ClutTestState;

typedef struct {
  ClutSuite suite;
  ClutRunner runner;
  ClutHooks hooks;
  ClutTestState current;
} ClutData;

/* TEST / REPEATED_TEST / PARAM_TEST macros */
#define CLUT_TEST_NAME(name) clut_test_##name
#define CLUT_RUN_TEST_NAME(name) clut_run_test_##name

#define TEST(name)                                                                                                                                                                                                                                                 \
  void CLUT_TEST_NAME(name)(void);                                                                                                                                                                                                                                 \
  void CLUT_RUN_TEST_NAME(name)(void) { ClutRunSimpleTest(CLUT_TEST_NAME(name)); }                                                                                                                                                                                 \
  void CLUT_TEST_NAME(name)(void)

#define REPEATED_TEST(name, repetitions)                                                                                                                                                                                                                           \
  void CLUT_TEST_NAME(name)(ClutRepeatedTestInput input);                                                                                                                                                                                                          \
  void CLUT_RUN_TEST_NAME(name)(void) { ClutRunRepeatedTest(CLUT_TEST_NAME(name), repetitions); }                                                                                                                                                                  \
  void CLUT_TEST_NAME(name)(ClutRepeatedTestInput input)

#define REPEATED_TEST_WITH_THRESHOLD(name, repetitions, failure_threshold)                                                                                                                                                                                         \
  void CLUT_TEST_NAME(name)(ClutRepeatedTestInput input);                                                                                                                                                                                                          \
  void CLUT_RUN_TEST_NAME(name)(void) { ClutRunRepeatedTestWithThreshold(CLUT_TEST_NAME(name), repetitions, failure_threshold); }                                                                                                                                  \
  void CLUT_TEST_NAME(name)(ClutRepeatedTestInput input)

#define PARAM_TEST(name, Type, ...)                                                                                                                                                                                                                                \
  void CLUT_TEST_NAME(name)(Type input);                                                                                                                                                                                                                           \
  void CLUT_RUN_TEST_NAME(name)(void) { ClutRunParamTest(CLUT_TEST_NAME(name), Type, __VA_ARGS__); }                                                                                                                                                               \
  void CLUT_TEST_NAME(name)(Type input)

/* Test Runner lifecycle macros */
#define RUNNER_BEGIN() ClutRunnerBegin()
#define RUNNER_END() ClutRunnerEnd()

/* Test Suite lifecycle macros */
#define SUITE_BEGIN() ClutSuiteBegin()
#define RUN_TEST(test_name) ClutSuiteAddTest(CLUT_RUN_TEST_NAME(test_name), #test_name)
#define SUITE_END() ClutSuiteEnd()

/* Hook definition / Hook registration macros */
#define CLUT_HOOK_NAME(name, suffix) clut_##name##_##suffix
#define CLUT_HOOK_DECL(name, suffix) void CLUT_HOOK_NAME(name, suffix)(void)

#define CLUT_HOOK_DEFINE(name, suffix)                                                                                                                                                                                                                             \
  CLUT_HOOK_DECL(name, suffix);                                                                                                                                                                                                                                    \
  CLUT_HOOK_DECL(name, suffix)

#define BEFORE_ALL_HOOK(name) CLUT_HOOK_DEFINE(name, before_all)
#define BEFORE_EACH_HOOK(name) CLUT_HOOK_DEFINE(name, before_each)
#define AFTER_ALL_HOOK(name) CLUT_HOOK_DEFINE(name, after_all)
#define AFTER_EACH_HOOK(name) CLUT_HOOK_DEFINE(name, after_each)

#define SET_BEFORE_ALL(hook) ClutSetBeforeAll(CLUT_HOOK_NAME(hook, before_all))
#define SET_BEFORE_EACH(hook) ClutSetBeforeEach(CLUT_HOOK_NAME(hook, before_each))
#define SET_AFTER_ALL(hook) ClutSetAfterAll(CLUT_HOOK_NAME(hook, after_all))
#define SET_AFTER_EACH(hook) ClutSetAfterEach(CLUT_HOOK_NAME(hook, after_each))

/* Assertions */
#define TEST_ASSERT(condition) ClutAssert((condition), __FILE__, __LINE__, "Expression Is False -> " #condition)
#define TEST_ASSERT_TRUE(condition) ClutAssert((condition), __FILE__, __LINE__, "Expected True But Was False -> " #condition)
#define TEST_ASSERT_FALSE(condition) ClutAssert(!(condition), __FILE__, __LINE__, "Expected False But Was True -> " #condition)
#define TEST_ASSERT_UNLESS(condition) ClutAssert(!(condition), __FILE__, __LINE__, "Expression Is True -> " #condition)
#define TEST_ASSERT_NULL(pointer) ClutAssert(((pointer) == NULL), __FILE__, __LINE__, "Expected NULL")
#define TEST_ASSERT_NOT_NULL(pointer) ClutAssert(((pointer) != NULL), __FILE__, __LINE__, "Expected Non-NULL")

#define TEST_FAIL(msg) ClutAssert(0, __FILE__, __LINE__, (msg))
#define TEST_SKIP() ClutSkipTest()

#define TEST_ASSERT_EQUAL_CHAR(expected, actual) ClutAssertEqualChar((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT(expected, actual) ClutAssertEqualInt((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT(expected, actual) ClutAssertEqualUint((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_FLOAT(expected, actual) ClutAssertEqualFloat((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_DOUBLE(expected, actual) ClutAssertEqualDouble((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING(expected, actual) ClutAssertEqualString((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len) ClutAssertEqualStringLen((expected), (actual), (len), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_PTR(expected, actual) ClutAssertEqualPtr((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_NOT_EQUAL_CHAR(expected, actual) ClutAssertNotEqualChar((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_INT(expected, actual) ClutAssertNotEqualInt((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_UINT(expected, actual) ClutAssertNotEqualUint((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_FLOAT(expected, actual) ClutAssertNotEqualFloat((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_DOUBLE(expected, actual) ClutAssertNotEqualDouble((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_STRING(expected, actual) ClutAssertNotEqualString((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_STRING_LEN(expected, actual, len) ClutAssertNotEqualStringLen((expected), (actual), (len), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL_PTR(expected, actual) ClutAssertNotEqualPtr((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_CHAR(expected, actual) ClutAssertCharGreaterThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_CHAR(expected, actual) ClutAssertCharLessThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_CHAR(expected, actual) ClutAssertCharGreaterOrEqual((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_CHAR(expected, actual) ClutAssertCharLessOrEqual((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_INT(expected, actual) ClutAssertIntGreaterThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT(expected, actual) ClutAssertIntLessThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT(expected, actual) ClutAssertIntGreaterOrEqual((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT(expected, actual) ClutAssertIntLessOrEqual((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_UINT(expected, actual) ClutAssertUintGreaterThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT(expected, actual) ClutAssertUintLessThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT(expected, actual) ClutAssertUintGreaterOrEqual((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT(expected, actual) ClutAssertUintLessOrEqual((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_FLOAT(expected, actual) ClutAssertFloatGreaterThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(expected, actual) ClutAssertFloatGreaterOrEqual((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_FLOAT(expected, actual) ClutAssertFloatLessThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_FLOAT(expected, actual) ClutAssertFloatLessOrEqual((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_DOUBLE(expected, actual) ClutAssertDoubleGreaterThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(expected, actual) ClutAssertDoubleGreaterOrEqual((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_DOUBLE(expected, actual) ClutAssertDoubleLessThan((expected), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(expected, actual) ClutAssertDoubleLessOrEqual((expected), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_WITHIN_CHAR(expected, delta, actual) ClutAssertWithinChar((expected), (delta), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_INT(expected, delta, actual) ClutAssertWithinInt((expected), (delta), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_UINT(expected, delta, actual) ClutAssertWithinUint((expected), (delta), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_FLOAT(expected, delta, actual) ClutAssertWithinFloat((expected), (delta), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_DOUBLE(expected, delta, actual) ClutAssertWithinDouble((expected), (delta), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_EQUAL_MEMORY(expected, actual, len) ClutAssertEqualMemory((expected), (actual), (len), 1, __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements) ClutAssertEqualMemory((expected), (actual), (len), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, actual, num_elements) ClutAssertEqualCharArray((expected), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements) ClutAssertEqualIntArray((expected), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements) ClutAssertEqualUintArray((expected), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements) ClutAssertEqualFloatArray((expected), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, num_elements) ClutAssertEqualDoubleArray((expected), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements) ClutAssertEqualStringArray((expected), (actual), (num_elements), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_WITHIN_CHAR_ARRAY(expected, delta, actual, num_elements) ClutAssertWithinCharArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_INT_ARRAY(expected, delta, actual, num_elements) ClutAssertWithinIntArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_UINT_ARRAY(expected, delta, actual, num_elements) ClutAssertWithinUintArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_FLOAT_ARRAY(expected, delta, actual, num_elements) ClutAssertWithinFloatArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_WITHIN_DOUBLE_ARRAY(expected, delta, actual, num_elements) ClutAssertWithinDoubleArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, NULL)

/* Messages */
#define TEST_ASSERT_MESSAGE(condition, msg) ClutAssert((condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_TRUE_MESSAGE(condition, msg) ClutAssert((condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_FALSE_MESSAGE(condition, msg) ClutAssert(!(condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_UNLESS_MESSAGE(condition, msg) ClutAssert(!(condition), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NULL_MESSAGE(pointer, msg) ClutAssert(((pointer) == NULL), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_NULL_MESSAGE(pointer, msg) ClutAssert(((pointer) != NULL), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_EQUAL_CHAR_MESSAGE(expected, actual, msg) ClutAssertEqualChar((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, msg) ClutAssertEqualInt((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_UINT_MESSAGE(expected, actual, msg) ClutAssertEqualUint((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected, actual, msg) ClutAssertEqualFloat((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(expected, actual, msg) ClutAssertEqualDouble((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, msg) ClutAssertEqualString((const char *)(expected), (const char *)(actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(expected, actual, len, msg) ClutAssertEqualStringLen((expected), (actual), (len), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_NOT_EQUAL_CHAR_MESSAGE(expected, actual, msg) ClutAssertNotEqualChar((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_INT_MESSAGE(expected, actual, msg) ClutAssertNotEqualInt((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_UINT_MESSAGE(expected, actual, msg) ClutAssertNotEqualUint((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_FLOAT_MESSAGE(expected, actual, msg) ClutAssertNotEqualFloat((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_DOUBLE_MESSAGE(expected, actual, msg) ClutAssertNotEqualDouble((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_STRING_MESSAGE(expected, actual, msg) ClutAssertNotEqualString((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_STRING_LEN_MESSAGE(expected, actual, len, msg) ClutAssertNotEqualStringLen((expected), (actual), (len), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_PTR_MESSAGE(expected, actual, msg) ClutAssertNotEqualPtr((expected), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_CHAR_MESSAGE(expected, actual, msg) ClutAssertCharGreaterThan((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_CHAR_MESSAGE(expected, actual, msg) ClutAssertCharLessThan((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_CHAR_MESSAGE(expected, actual, msg) ClutAssertCharGreaterOrEqual((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_CHAR_MESSAGE(expected, actual, msg) ClutAssertCharLessOrEqual((expected), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_INT_MESSAGE(threshold, actual, msg) ClutAssertIntGreaterThan((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_INT_MESSAGE(threshold, actual, msg) ClutAssertIntLessThan((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(threshold, actual, msg) ClutAssertIntGreaterOrEqual((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_INT_MESSAGE(threshold, actual, msg) ClutAssertIntLessOrEqual((threshold), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(threshold, actual, msg) ClutAssertUintGreaterThan((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_UINT_MESSAGE(threshold, actual, msg) ClutAssertUintLessThan((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT_MESSAGE(threshold, actual, msg) ClutAssertUintGreaterOrEqual((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT_MESSAGE(threshold, actual, msg) ClutAssertUintLessOrEqual((threshold), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_FLOAT_MESSAGE(threshold, actual, msg) ClutAssertFloatGreaterThan((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_FLOAT_MESSAGE(threshold, actual, msg) ClutAssertFloatGreaterOrEqual((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_FLOAT_MESSAGE(threshold, actual, msg) ClutAssertFloatLessThan((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_FLOAT_MESSAGE(threshold, actual, msg) ClutAssertFloatLessOrEqual((threshold), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_DOUBLE_MESSAGE(threshold, actual, msg) ClutAssertDoubleGreaterThan((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE_MESSAGE(threshold, actual, msg) ClutAssertDoubleGreaterOrEqual((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_DOUBLE_MESSAGE(threshold, actual, msg) ClutAssertDoubleLessThan((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_DOUBLE_MESSAGE(threshold, actual, msg) ClutAssertDoubleLessOrEqual((threshold), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_WITHIN_CHAR_MESSAGE(expected, delta, actual, msg) ClutAssertWithinChar((expected), (delta), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_INT_MESSAGE(expected, delta, actual, msg) ClutAssertWithinInt((expected), (delta), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_UINT_MESSAGE(expected, delta, actual, msg) ClutAssertWithinUint((expected), (delta), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_FLOAT_MESSAGE(expected, delta, actual, msg) ClutAssertWithinFloat((expected), (delta), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_DOUBLE_MESSAGE(expected, delta, actual, msg) ClutAssertWithinDouble((expected), (delta), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_EQUAL_MEMORY_MESSAGE(expected, actual, len, msg) ClutAssertEqualMemory((expected), (actual), (len), 1, __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_MEMORY_ARRAY_MESSAGE(expected, actual, len, num_elements, msg) ClutAssertEqualMemory((expected), (actual), (len), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE(expected, actual, num_elements, msg) ClutAssertEqualCharArray((expected), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, num_elements, msg) ClutAssertEqualIntArray((expected), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_UINT_ARRAY_MESSAGE(expected, actual, num_elements, msg) ClutAssertEqualUintArray((expected), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(expected, actual, num_elements, msg) ClutAssertEqualFloatArray((expected), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_DOUBLE_ARRAY_MESSAGE(expected, actual, num_elements, msg) ClutAssertEqualDoubleArray((expected), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_EQUAL_STRING_ARRAY_MESSAGE(expected, actual, num_elements, msg) ClutAssertEqualStringArray((expected), (actual), (num_elements), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_WITHIN_CHAR_ARRAY_MESSAGE(expected, delta, actual, num_elements, msg) ClutAssertWithinCharArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_INT_ARRAY_MESSAGE(expected, delta, actual, num_elements, msg) ClutAssertWithinIntArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_UINT_ARRAY_MESSAGE(expected, delta, actual, num_elements, msg) ClutAssertWithinUintArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_FLOAT_ARRAY_MESSAGE(expected, delta, actual, num_elements, msg) ClutAssertWithinFloatArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_WITHIN_DOUBLE_ARRAY_MESSAGE(expected, delta, actual, num_elements, msg) ClutAssertWithinDoubleArray((expected), (delta), (actual), (num_elements), __FILE__, __LINE__, (msg))

/* Public API Declarations */
CLUT_API void ClutRunnerBegin();
CLUT_API int ClutRunnerEnd();

CLUT_API void ClutSuiteBegin();
CLUT_API void ClutSuiteAddTest(ClutTestFn test_fn, const char *test_name);
CLUT_API void ClutSuiteEnd();

CLUT_API void ClutSetBeforeAll(ClutHookFn hook_fn);
CLUT_API void ClutSetBeforeEach(ClutHookFn hook_fn);
CLUT_API void ClutSetAfterAll(ClutHookFn hook_fn);
CLUT_API void ClutSetAfterEach(ClutHookFn hook_fn);

CLUT_API void ClutRunSimpleTest(ClutTestFn test_fn);
CLUT_API void ClutRunRepeatedTest(ClutRepeatedTestFn test_fn, size_t repetitions);
CLUT_API void ClutRunRepeatedTestWithThreshold(ClutRepeatedTestFn test_fn, size_t repetitions, size_t failure_threshold);
#define ClutRunParamTest(test_fn, Type, ...)

CLUT_API void ClutSkipTest();

CLUT_API void ClutAssert(bool condition, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualChar(char expected, char actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualInt(int expected, int actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualUint(size_t expected, size_t actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualFloat(float expected, float actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualDouble(double expected, double actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualString(const char *expected, const char *actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualStringLen(const char *expected, const char *actual, size_t len, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualPtr(const void *expected, const void *actual, const char *file, const int line, const char *msg);

CLUT_API void ClutAssertNotEqualChar(char expected, char actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertNotEqualInt(int expected, int actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertNotEqualUint(size_t expected, size_t actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertNotEqualFloat(float expected, float actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertNotEqualDouble(double expected, double actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertNotEqualString(const char *expected, const char *actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertNotEqualStringLen(const char *expected, const char *actual, size_t len, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertNotEqualPtr(const void *expected, const void *actual, const char *file, const int line, const char *msg);

CLUT_API void ClutAssertIntGreaterThan(int expected, int actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertIntGreaterOrEqual(int expected, int actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertIntLessThan(int expected, int actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertIntLessOrEqual(int expected, int actual, const char *file, int line, const char *msg);

CLUT_API void ClutAssertUintGreaterThan(size_t expected, size_t actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertUintGreaterOrEqual(size_t expected, size_t actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertUintLessThan(size_t expected, size_t actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertUintLessOrEqual(size_t expected, size_t actual, const char *file, int line, const char *msg);

CLUT_API void ClutAssertFloatGreaterThan(float expected, float actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertFloatGreaterOrEqual(float expected, float actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertFloatLessThan(float expected, float actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertFloatLessOrEqual(float expected, float actual, const char *file, int line, const char *msg);

CLUT_API void ClutAssertDoubleGreaterThan(double expected, double actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertDoubleGreaterOrEqual(double expected, double actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertDoubleLessThan(double expected, double actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertDoubleLessOrEqual(double expected, double actual, const char *file, int line, const char *msg);

CLUT_API void ClutAssertWithinChar(char expected, size_t delta, char actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertWithinInt(int expected, size_t delta, int actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertWithinUint(size_t expected, size_t delta, size_t actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertWithinFloat(float expected, float delta, float actual, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertWithinDouble(double expected, double delta, double actual, const char *file, const int line, const char *msg);

CLUT_API void ClutAssertEqualMemory(const void *expected, const void *actual, size_t len, size_t num_elements, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualCharArray(const char *expected, const char *actual, size_t num_elements, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualIntArray(const int *expected, const int *actual, size_t num_elements, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualUintArray(const size_t *expected, const size_t *actual, size_t num_elements, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualFloatArray(const float *expected, const float *actual, size_t num_elements, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualDoubleArray(const double *expected, const double *actual, size_t num_elements, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertEqualStringArray(const char **expected, const char **actual, size_t num_elements, const char *file, const int line, const char *msg);

CLUT_API void ClutAssertWithinCharArray(const char *expected, size_t delta, const char *actual, size_t num_elements, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertWithinIntArray(const int *expected, size_t delta, const int *actual, size_t num_elements, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertWithinUintArray(const size_t *expected, size_t delta, const size_t *actual, size_t num_elements, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertWithinFloatArray(const float *expected, float delta, const float *actual, size_t num_elements, const char *file, const int line, const char *msg);
CLUT_API void ClutAssertWithinDoubleArray(const double *expected, double delta, const double *actual, size_t num_elements, const char *file, const int line, const char *msg);

#endif /* INCLUDE_CLUT_H */

#ifdef CLUT_IMPLEMENTATION
#ifndef CLUT_IMPLEMENTATION_INCLUDED
#define CLUT_IMPLEMENTATION_INCLUDED

#include <ctype.h>
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

/* Color macros */
#ifdef CLUT_OUTPUT_COLOR
#define CLUT_MSG_END_COLOR_TEXT "\033[0m"

#define CLUT_MSG_BEGIN_RED_TEXT "\033[31m"
#define CLUT_MSG_BEGIN_GREEN_TEXT "\033[32m"
#define CLUT_MSG_BEGIN_YELLOW_TEXT "\033[33m"
#define CLUT_MSG_BEGIN_BLUE_TEXT "\033[34m"

#define CLUT_RED_TEXT(text) CLUT_MSG_BEGIN_RED_TEXT text CLUT_MSG_END_COLOR_TEXT
#define CLUT_GREEN_TEXT(text) CLUT_MSG_BEGIN_GREEN_TEXT text CLUT_MSG_END_COLOR_TEXT
#define CLUT_YELLOW_TEXT(text) CLUT_MSG_BEGIN_YELLOW_TEXT text CLUT_MSG_END_COLOR_TEXT
#define CLUT_BLUE_TEXT(text) CLUT_MSG_BEGIN_BLUE_TEXT text CLUT_MSG_END_COLOR_TEXT
#else
#define CLUT_MSG_END_COLOR_TEXT ""

#define CLUT_MSG_BEGIN_RED_TEXT ""
#define CLUT_MSG_BEGIN_GREEN_TEXT ""
#define CLUT_MSG_BEGIN_YELLOW_TEXT ""
#define CLUT_MSG_BEGIN_BLUE_TEXT ""

#define CLUT_RED_TEXT(text) text
#define CLUT_GREEN_TEXT(text) text
#define CLUT_YELLOW_TEXT(text) text
#define CLUT_BLUE_TEXT(text) text
#endif

#ifndef CLUT_DA_INIT_CAP
#define CLUT_DA_INIT_CAP 256
#endif

#define clut_da_reserve(da, expected_capacity)                                                                                                                                                                                                                     \
  do {                                                                                                                                                                                                                                                             \
    if ((expected_capacity) > (da)->capacity) {                                                                                                                                                                                                                    \
      if ((da)->capacity == 0) {                                                                                                                                                                                                                                   \
        (da)->capacity = CLUT_DA_INIT_CAP;                                                                                                                                                                                                                         \
      }                                                                                                                                                                                                                                                            \
      while ((expected_capacity) > (da)->capacity) {                                                                                                                                                                                                               \
        (da)->capacity *= 2;                                                                                                                                                                                                                                       \
      }                                                                                                                                                                                                                                                            \
      (da)->items = CLUT_REALLOC((da)->items, (da)->capacity * sizeof(*(da)->items));                                                                                                                                                                              \
    }                                                                                                                                                                                                                                                              \
  } while (0)

#define clut_da_append(da, item)                                                                                                                                                                                                                                   \
  do {                                                                                                                                                                                                                                                             \
    clut_da_reserve((da), (da)->count + 1);                                                                                                                                                                                                                        \
    (da)->items[(da)->count++] = (item);                                                                                                                                                                                                                           \
  } while (0)

#define clut_da_append_many(da, new_items, new_items_count)                                                                                                                                                                                                        \
  do {                                                                                                                                                                                                                                                             \
    clut_da_reserve((da), (da)->count + (new_items_count));                                                                                                                                                                                                        \
    memcpy((da)->items + (da)->count, (new_items), (new_items_count) * sizeof(*(da)->items));                                                                                                                                                                      \
    (da)->count += (new_items_count);                                                                                                                                                                                                                              \
  } while (0)

#define clut_da_foreach(Type, it, da) for (Type *it = (da)->items; it < (da)->items + (da)->count; ++it)

#define clut_da_free(da)                                                                                                                                                                                                                                           \
  do {                                                                                                                                                                                                                                                             \
    CLUT_FREE((da)->items);                                                                                                                                                                                                                                        \
    (da)->items = NULL;                                                                                                                                                                                                                                            \
    (da)->count = 0;                                                                                                                                                                                                                                               \
    (da)->capacity = 0;                                                                                                                                                                                                                                            \
  } while (0)

#define clut_da_clear(da)                                                                                                                                                                                                                                          \
  do {                                                                                                                                                                                                                                                             \
    (da)->count = 0;                                                                                                                                                                                                                                               \
  } while (0)

/* String Builder Implementation */
static void clut_string_builder_append(ClutSB *sb, const char *str) {
  if (!str)
    str = "(null)";
  size_t n = strlen(str);
  clut_da_append_many(sb, str, n);
}

static void clut_string_builder_append_char(ClutSB *sb, char c) { clut_da_append(sb, c); }

static void clut_string_builder_append_format(ClutSB *sb, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int n = vsnprintf(NULL, 0, fmt, args);
  va_end(args);
  if (n <= 0)
    return;
  clut_da_reserve(sb, sb->count + (size_t)n + 1);
  va_start(args, fmt);
  vsnprintf(sb->items + sb->count, (size_t)n + 1, fmt, args);
  va_end(args);
  sb->count += (size_t)n;
}

static const char *clut_string_builder_to_cstr(ClutSB *sb) {
  if (!sb->items)
    return "";
  clut_da_reserve(sb, sb->count + 1);
  sb->items[sb->count] = '\0';
  return sb->items;
}

/* Helpers */
static inline float clut_abs_float(float x) { return x < 0.0f ? -x : x; }
static inline double clut_abs_double(double x) { return x < 0.0 ? -x : x; }

static inline bool clut_floats_are_close(float expected, float actual) {
  if (expected == 0.0f)
    return actual == 0.0f;
  return clut_abs_float(expected - actual) < clut_abs_float(expected) * CLUT_FLOAT_EPSILON;
}

static inline bool clut_doubles_are_close(double expected, double actual) {
  if (expected == 0.0)
    return actual == 0.0;
  return clut_abs_double(expected - actual) < clut_abs_double(expected) * CLUT_DOUBLE_EPSILON;
}

static inline double clut_elapsed_seconds(long start) { return ((double)(clock() - start)) / CLOCKS_PER_SEC; }

CLUT_STATE_LINKAGE ClutData Clut = {0};

/* Log Helpers */
static void clut_capture_failure_location(ClutLogRecord *record, const char *file, int line) {
  record->file = file;
  record->line = line;
  record->iteration = Clut.current.iteration;
}

#ifdef CLUT_OUTPUT_TAP
static size_t clut_tap_test_index = 0;
static bool clut_tap_plan_printed = false;

static void clut_report_plan(void) {
  if (!clut_tap_plan_printed) {
    fprintf(CLUT_STREAM_DEFAULT, "1..%zu\n", Clut.suite.total_tests);
    clut_tap_plan_printed = true;
  }
}

static void clut_report_pass(const char *test_name, double elapsed_seconds) {
  clut_report_plan();
  clut_tap_test_index++;
  fprintf(CLUT_STREAM_DEFAULT, "ok %zu - %s # (%.3fs)\n", clut_tap_test_index, test_name, elapsed_seconds);
}

static void clut_report_skip(const char *test_name) {
  clut_report_plan();
  clut_tap_test_index++;
  fprintf(CLUT_STREAM_DEFAULT, "# skip %s\n", test_name);
}

static void clut_report_fail_header() {
  clut_report_plan();
  clut_tap_test_index++;
  double elapsed_seconds = ClutElapsedSeconds(Clut.current.start_time);
  fprintf(CLUT_STREAM_DEFAULT, "not ok %zu - %s # (%.3fs)\n", clut_tap_test_index, Clut.current.name, elapsed_seconds);
}

static void clut_report_format_failure(const ClutLogRecord *record, ClutSB *failure_report, ClutSB *failure_message) {
  clut_string_builder_append_format(failure_report, "# %s:%d: %s", record->file, record->line, clut_string_builder_to_cstr(failure_message));
  if (record->iteration >= 0) {
    clut_string_builder_append_format(failure_report, " [iteration %d]", record->iteration);
  }
  clut_string_builder_append_char(failure_report, '\n');
}

static void clut_report_print_failures(ClutSB *failure_report) { fprintf(CLUT_STREAM_DEFAULT, "%s", clut_string_builder_to_cstr(failure_report)); }

static void clut_report_suite_end(const ClutSuiteResult *result) {
  clut_report_plan();
  fprintf(CLUT_STREAM_DEFAULT, "# Tests: %zu, Passed: %zu, Failed: %zu, Skipped: %zu, Time: %.3fs\n", result->total_tests, result->passed, result->failed, result->skipped, result->total_seconds);
  clut_tap_test_index = 0;
  clut_tap_plan_printed = false;
}

#elif defined(CLUT_OUTPUT_GITHUB)
static void clut_report_pass(const char *test_name, double elapsed_seconds) { fprintf(CLUT_STREAM_DEFAULT, "::notice title=PASS::%s (%.3fs)\n", test_name, elapsed_seconds); }
static void clut_report_skip(const char *test_name) { fprintf(CLUT_STREAM_DEFAULT, "::notice title=SKIP::%s\n", test_name); }
static void clut_report_fail_header() {
  double elapsed_seconds = ClutElapsedSeconds(Clut.current.start_time);
  fprintf(CLUT_STREAM_FAIL, "::notice title=FAIL::%s (%.3fs)\n", Clut.current.name, elapsed_seconds);
}
static void clut_report_format_failure(const ClutLogRecord *record, ClutSB *failure_report, ClutSB *failure_message) {
  clut_string_builder_append_format(failure_report, "::error file=%s,line=%d,title=%s::%s\n", record->file, record->line, Clut.current.name, clut_string_builder_to_cstr(failure_message));
}
static void clut_report_print_failures(ClutSB *failure_report) { fprintf(CLUT_STREAM_FAIL, "%s", clut_string_builder_to_cstr(failure_report)); }
static void clut_report_suite_end(const ClutSuiteResult *result) {
  fprintf(CLUT_STREAM_DEFAULT, "::notice title=Suite Results::Tests=%zu Passed=%zu Failed=%zu Skipped=%zu Time=%.3fs\n", result->total_tests, result->passed, result->failed, result->skipped, result->total_seconds);
}
#else /* default */
static void clut_report_pass(const char *test_name, double elapsed_seconds) { fprintf(CLUT_STREAM_DEFAULT, "%s%-50s%.3fs\n", CLUT_GREEN_TEXT("[ PASS ] "), test_name, elapsed_seconds); }
static void clut_report_skip(const char *test_name) { fprintf(CLUT_STREAM_DEFAULT, "%s%-50s\n", CLUT_YELLOW_TEXT("[ SKIP ] "), test_name); }
static void clut_report_fail_header() {
  double elapsed_seconds = clut_elapsed_seconds(Clut.current.start_time);
  fprintf(CLUT_STREAM_FAIL, "%s%s%-50s%.3fs%s\n", CLUT_RED_TEXT("[ FAIL ] "), CLUT_MSG_BEGIN_RED_TEXT, Clut.current.name, elapsed_seconds, CLUT_MSG_END_COLOR_TEXT);
}

static void clut_report_format_failure(const ClutLogRecord *record, ClutSB *failure_report, ClutSB *failure_message) {
  clut_string_builder_append(failure_report, CLUT_MSG_BEGIN_RED_TEXT);
  clut_string_builder_append_format(failure_report, "%s:%d:%s", record->file, record->line, Clut.current.name);
  if (record->iteration >= 0) {
    clut_string_builder_append_format(failure_report, "[%d]", record->iteration);
  }
  clut_string_builder_append_char(failure_report, ':');
  clut_string_builder_append(failure_report, clut_string_builder_to_cstr(failure_message));
  clut_string_builder_append(failure_report, CLUT_MSG_END_COLOR_TEXT);
  clut_string_builder_append_char(failure_report, '\n');
}

static void clut_report_print_failures(ClutSB *failure_report) { fprintf(CLUT_STREAM_FAIL, "%s", clut_string_builder_to_cstr(failure_report)); }
static void clut_report_suite_end(const ClutSuiteResult *result) {
  FILE *stream = CLUT_STREAM_DEFAULT;
  fprintf(stream, "--------------------------------\n");
  fprintf(stream, CLUT_BLUE_TEXT("Tests:      ") CLUT_GREEN_TEXT("%zu\n"), result->total_tests);
  fprintf(stream, CLUT_BLUE_TEXT("Passed:     ") CLUT_GREEN_TEXT("%zu\n"), result->passed);
  fprintf(stream, CLUT_BLUE_TEXT("Failed:     ") CLUT_RED_TEXT("%zu\n"), result->failed);
  fprintf(stream, CLUT_BLUE_TEXT("Skipped:    ") CLUT_YELLOW_TEXT("%zu\n"), result->skipped);
  fprintf(stream, "--------------------------------\n");
  fprintf(stream, CLUT_BLUE_TEXT("Total time: %.3fs\n"), result->total_seconds);
  fprintf(stream, "\n");
}
#endif

static void clut_register_failure(const char *file, int line) {
  Clut.current.failed = true;
  ClutLogRecord record;
  clut_capture_failure_location(&record, file, line);
  clut_report_format_failure(&record, &Clut.runner.failure_report, &Clut.runner.failure_message);
}

static void clut_message_append(const char *msg) { clut_string_builder_append(&Clut.runner.failure_message, msg); }
static void clut_message_append_char(char c) {
  unsigned char byte_value = (unsigned char)c;
  const char *escaped = NULL;

  switch (byte_value) {
  case '\n':
    escaped = "\\n";
    break;
  case '\t':
    escaped = "\\t";
    break;
  case '\r':
    escaped = "\\r";
    break;
  case '\\':
    escaped = "\\\\";
    break;
  case '\'':
    escaped = "\\'";
    break;
  }

  if (escaped) {
    clut_string_builder_append_format(&Clut.runner.failure_message, "'%s' (0x%02X)", escaped, byte_value);
  } else if (isprint(byte_value)) {
    clut_string_builder_append_format(&Clut.runner.failure_message, "'%c' (0x%02X)", byte_value, byte_value);
  } else {
    clut_string_builder_append_format(&Clut.runner.failure_message, "'\\x%02X' (0x%02X)", byte_value, byte_value);
  }
}
static void clut_message_append_int(int v) { clut_string_builder_append_format(&Clut.runner.failure_message, "%d", v); }
static void clut_message_append_uint(size_t v) { clut_string_builder_append_format(&Clut.runner.failure_message, "%zu", v); }
static void clut_message_append_float(float v) { clut_string_builder_append_format(&Clut.runner.failure_message, "%.5f", v); }
static void clut_message_append_double(double v) { clut_string_builder_append_format(&Clut.runner.failure_message, "%.9f", v); }
static void clut_message_append_ptr(const void *p) { clut_string_builder_append_format(&Clut.runner.failure_message, "%p", p); }
static void clut_message_append_hex(int v) { clut_string_builder_append_format(&Clut.runner.failure_message, "0x%02X", v); }
static void clut_message_append_str(const char *str) {
  if (str == NULL) {
    clut_message_append("(null)");
  } else {
    clut_string_builder_append_char(&Clut.runner.failure_message, '"');
    clut_message_append(str);
    clut_string_builder_append_char(&Clut.runner.failure_message, '"');
  }
}
#define clut_message_append_format(fmt, ...) clut_string_builder_append_format(&Clut.runner.failure_message, fmt, __VA_ARGS__)

static void clut_message_append_array_prefix(size_t index) {
  clut_message_append(CLUT_MSG_ARRAY_MISMATCH_AT);
  clut_message_append_format("[%zu] ", index);
}

#define CLUT_FORMAT_COMPARISON_MESSAGE(append_val, lhs, op_str, rhs)                                                                                                                                                                                               \
  do {                                                                                                                                                                                                                                                             \
    clut_message_append(CLUT_MSG_EXPECTED);                                                                                                                                                                                                                        \
    append_val((lhs));                                                                                                                                                                                                                                             \
    clut_message_append((op_str));                                                                                                                                                                                                                                 \
    append_val((rhs));                                                                                                                                                                                                                                             \
  } while (0)

#define CLUT_FORMAT_WITHIN_MESSAGE(append_val, append_num, expected, delta, diff)                                                                                                                                                                                  \
  do {                                                                                                                                                                                                                                                             \
    clut_message_append(CLUT_MSG_EXPECTED);                                                                                                                                                                                                                        \
    append_val((expected));                                                                                                                                                                                                                                        \
    clut_message_append(CLUT_MSG_WITHIN_DELTA);                                                                                                                                                                                                                    \
    append_num((delta));                                                                                                                                                                                                                                           \
    clut_message_append(CLUT_MSG_BUT_DIFF);                                                                                                                                                                                                                        \
    append_num((diff));                                                                                                                                                                                                                                            \
  } while (0)

#ifndef CLUT_ABORT_TEST
#ifdef CLUT_HAS_LONGJMP
#define CLUT_ABORT_TEST longjmp(Clut.current.jmp_buffer, 1)
#else
#define CLUT_ABORT_TEST return
#endif // CLUT_HAS_LONGJMP
#endif // CLUT_ABORT_TEST

#define CLUT_ABORT_IF_FAILED_OR_SKIPPED                                                                                                                                                                                                                            \
  do {                                                                                                                                                                                                                                                             \
    if (Clut.current.failed || Clut.current.skipped) {                                                                                                                                                                                                             \
      CLUT_ABORT_TEST;                                                                                                                                                                                                                                             \
    }                                                                                                                                                                                                                                                              \
  } while (0)

#define CLUT_ASSERT_COMPARE(append_fn, condition, lhs, op_str, rhs, file, line, msg)                                                                                                                                                                               \
  do {                                                                                                                                                                                                                                                             \
    CLUT_ABORT_IF_FAILED_OR_SKIPPED;                                                                                                                                                                                                                               \
    if (condition)                                                                                                                                                                                                                                                 \
      return;                                                                                                                                                                                                                                                      \
    if (msg) {                                                                                                                                                                                                                                                     \
      clut_message_append((msg));                                                                                                                                                                                                                                  \
    } else {                                                                                                                                                                                                                                                       \
      CLUT_FORMAT_COMPARISON_MESSAGE(append_fn, (lhs), (op_str), (rhs));                                                                                                                                                                                           \
    }                                                                                                                                                                                                                                                              \
    clut_register_failure(file, line);                                                                                                                                                                                                                             \
  } while (0)

#define CLUT_ASSERT_WITHIN(append_fn_val, append_fn_num, diff_val, expected, delta, file, line, msg)                                                                                                                                                               \
  do {                                                                                                                                                                                                                                                             \
    CLUT_ABORT_IF_FAILED_OR_SKIPPED;                                                                                                                                                                                                                               \
    if ((diff_val) <= (delta))                                                                                                                                                                                                                                     \
      return;                                                                                                                                                                                                                                                      \
    if (msg) {                                                                                                                                                                                                                                                     \
      clut_message_append((msg));                                                                                                                                                                                                                                  \
    } else {                                                                                                                                                                                                                                                       \
      CLUT_FORMAT_WITHIN_MESSAGE(append_fn_val, append_fn_num, (expected), (delta), (diff_val));                                                                                                                                                                   \
    }                                                                                                                                                                                                                                                              \
    clut_register_failure(file, line);                                                                                                                                                                                                                             \
  } while (0)

#define CLUT_CHECK_BUFFER_ARGS                                                                                                                                                                                                                                     \
  do {                                                                                                                                                                                                                                                             \
    if (num_elements == 0)                                                                                                                                                                                                                                         \
      return;                                                                                                                                                                                                                                                      \
    if (expected == actual)                                                                                                                                                                                                                                        \
      return;                                                                                                                                                                                                                                                      \
    if (expected == NULL || actual == NULL) {                                                                                                                                                                                                                      \
      clut_message_append(msg ? msg : CLUT_MSG_MEMORY_NULL);                                                                                                                                                                                                       \
      clut_register_failure(file, line);                                                                                                                                                                                                                           \
      return;                                                                                                                                                                                                                                                      \
    }                                                                                                                                                                                                                                                              \
  } while (0)

CLUT_API void ClutRunnerBegin() { Clut.runner = (ClutRunner){0}; }

CLUT_API int ClutRunnerEnd() {
  clut_da_free(&Clut.runner.failure_report);
  clut_da_free(&Clut.runner.failure_message);
  clut_da_free(&Clut.suite.registered_tests);
  return Clut.runner.total_failures > 255 ? 255 : (int)Clut.runner.total_failures;
}

static void clut_reset_suite(void) {
  Clut.suite.total_tests = 0;
  Clut.suite.passed = 0;
  Clut.suite.failed = 0;
  Clut.suite.start_time = 0;
  clut_da_clear(&Clut.suite.registered_tests);
}

CLUT_API void ClutSuiteBegin(void) {
  clut_reset_suite();
  Clut.hooks = (ClutHooks){0};
}

CLUT_API void ClutSuiteAddTest(ClutTestFn run_test_fn, const char *test_name) {
  ClutPendingTest pending_test = (ClutPendingTest){run_test_fn, test_name};
  clut_da_append(&Clut.suite.registered_tests, pending_test);
  Clut.suite.total_tests = Clut.suite.registered_tests.count;
}

static void clut_reset_current_test(void) {
  Clut.current.name = NULL;
  Clut.current.failed = false;
  Clut.current.iteration = -1;
  clut_da_clear(&Clut.runner.failure_report);
  clut_da_clear(&Clut.runner.failure_message);
}

static void clut_execute_test(ClutTestFn run_test_fn, const char *test_name) {
  clut_reset_current_test();

  if (Clut.hooks.before_each)
    Clut.hooks.before_each();

  Clut.current.name = test_name;
  Clut.current.start_time = clock();

  run_test_fn();

  if (Clut.current.failed) {
    clut_report_fail_header();
    clut_report_print_failures(&Clut.runner.failure_report);
    Clut.suite.failed++;
  } else if (Clut.current.skipped) {
    Clut.suite.skipped++;
    clut_report_skip(test_name);
  } else {
    clut_report_pass(test_name, clut_elapsed_seconds(Clut.current.start_time));
    Clut.suite.passed++;
  }

  if (Clut.hooks.after_each)
    Clut.hooks.after_each();
}

CLUT_API void ClutSuiteEnd(void) {
  if (Clut.suite.total_tests == 0) {
    return;
  }

  Clut.suite.start_time = clock();

  if (Clut.hooks.before_all)
    Clut.hooks.before_all();

  clut_da_foreach(ClutPendingTest, test, &Clut.suite.registered_tests) { clut_execute_test(test->fn, test->name); }

  double total_time = clut_elapsed_seconds(Clut.suite.start_time);

  if (Clut.hooks.after_all)
    Clut.hooks.after_all();

  ClutSuiteResult result = {Clut.suite.total_tests, Clut.suite.passed, Clut.suite.failed, Clut.suite.skipped, total_time};
  clut_report_suite_end(&result);

  Clut.runner.total_failures += Clut.suite.failed;
  clut_reset_suite();
}

CLUT_API void ClutSetBeforeAll(ClutHookFn hook_fn) { Clut.hooks.before_all = hook_fn; }
CLUT_API void ClutSetBeforeEach(ClutHookFn hook_fn) { Clut.hooks.before_each = hook_fn; }
CLUT_API void ClutSetAfterAll(ClutHookFn hook_fn) { Clut.hooks.after_all = hook_fn; }
CLUT_API void ClutSetAfterEach(ClutHookFn hook_fn) { Clut.hooks.after_each = hook_fn; }

#ifdef CLUT_HAS_LONGJMP

#define CLUT_RUN_GUARDED(call)                                                                                                                                                                                                                                     \
  if (setjmp(Clut.current.jmp_buffer) == 0) {                                                                                                                                                                                                                      \
    call;                                                                                                                                                                                                                                                          \
  }
#else
#define CLUT_RUN_GUARDED(call)                                                                                                                                                                                                                                     \
  {                                                                                                                                                                                                                                                                \
    call;                                                                                                                                                                                                                                                          \
  }
#endif

CLUT_API void ClutRunSimpleTest(ClutTestFn test_fn) { CLUT_RUN_GUARDED(test_fn()); }

CLUT_API void ClutRunRepeatedTest(ClutRepeatedTestFn test_fn, size_t repetitions) {
  volatile ClutRepeatedTestInput input = (ClutRepeatedTestInput){.total_repetitions = repetitions};
  volatile bool any_iteration_failed = false;
  for (volatile size_t i = 1; i <= repetitions; ++i) {
    clut_da_clear(&Clut.runner.failure_message);
    Clut.current.iteration = (int)i;
    input.current_repetition = i;
    CLUT_RUN_GUARDED(test_fn(input));
    if (Clut.current.failed) {
      any_iteration_failed = true;
      Clut.current.failed = false;
    }
  }
  Clut.current.failed = any_iteration_failed;
}

CLUT_API void ClutRunRepeatedTestWithThreshold(ClutRepeatedTestFn test_fn, size_t repetitions, size_t failure_threshold) {
  volatile ClutRepeatedTestInput input = (ClutRepeatedTestInput){.total_repetitions = repetitions};
  volatile size_t failures = 0;
  for (volatile size_t i = 1; i <= repetitions; ++i) {
    clut_da_clear(&Clut.runner.failure_message);
    Clut.current.iteration = (int)i;
    input.current_repetition = i;
    CLUT_RUN_GUARDED(test_fn(input));
    if (Clut.current.failed) {
      failures++;
      if (failures >= failure_threshold) {
        break;
      }
      Clut.current.failed = false;
    }
  }
}

#undef ClutRunParamTest
#define ClutRunParamTest(test_fn, Type, ...)                                                                                                                                                                                                                       \
  do {                                                                                                                                                                                                                                                             \
    Type cases[] = __VA_ARGS__;                                                                                                                                                                                                                                    \
    size_t case_count = sizeof(cases) / sizeof(cases[0]);                                                                                                                                                                                                          \
    volatile bool failed = false;                                                                                                                                                                                                                                  \
    for (volatile size_t i = 0; i < case_count; ++i) {                                                                                                                                                                                                             \
      clut_da_clear(&Clut.runner.failure_message);                                                                                                                                                                                                                 \
      Clut.current.iteration = (int)i;                                                                                                                                                                                                                             \
      CLUT_RUN_GUARDED(test_fn(cases[i]));                                                                                                                                                                                                                         \
      if (Clut.current.failed) {                                                                                                                                                                                                                                   \
        failed = true;                                                                                                                                                                                                                                             \
        Clut.current.failed = false;                                                                                                                                                                                                                               \
      }                                                                                                                                                                                                                                                            \
    }                                                                                                                                                                                                                                                              \
    Clut.current.failed = failed;                                                                                                                                                                                                                                  \
  } while (0)

CLUT_API void ClutSkipTest() { Clut.current.skipped = true; }

CLUT_API void ClutAssert(bool condition, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  if (!condition) {
    clut_message_append(msg);
    clut_register_failure(file, line);
  }
}
CLUT_API void ClutAssertEqualChar(char expected, char actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_char, expected == actual, expected, CLUT_MSG_RECEIVED, actual, file, line, msg); }
CLUT_API void ClutAssertEqualInt(int expected, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_int, expected == actual, expected, CLUT_MSG_RECEIVED, actual, file, line, msg); }
CLUT_API void ClutAssertEqualUint(size_t expected, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_uint, expected == actual, expected, CLUT_MSG_RECEIVED, actual, file, line, msg); }
CLUT_API void ClutAssertEqualFloat(float expected, float actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_float, clut_floats_are_close(expected, actual), expected, CLUT_MSG_RECEIVED, actual, file, line, msg); }
CLUT_API void ClutAssertEqualDouble(double expected, double actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_message_append_double, clut_doubles_are_close(expected, actual), expected, CLUT_MSG_RECEIVED, actual, file, line, msg);
}

CLUT_API void ClutAssertEqualString(const char *expected, const char *actual, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
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
  CLUT_ASSERT_COMPARE(clut_message_append_str, !failed, actual, CLUT_MSG_EQUAL, expected, file, line, msg);
}

CLUT_API void ClutAssertEqualStringLen(const char *expected, const char *actual, size_t len, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
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
    clut_message_append(msg);
  } else {
    clut_message_append(CLUT_MSG_EXPECTED);
    clut_message_append_str(actual);
    clut_message_append(CLUT_MSG_EQUAL);
    clut_message_append_str(expected);
    clut_message_append(CLUT_MSG_UNTIL_LEN);
    clut_message_append_uint(len);
  }
  clut_register_failure(file, line);
}

CLUT_API void ClutAssertEqualPtr(const void *expected, const void *actual, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  if (expected == actual)
    return;
  if (msg) {
    clut_message_append(msg);
  } else {
    CLUT_FORMAT_COMPARISON_MESSAGE(clut_message_append_ptr, expected, CLUT_MSG_RECEIVED, actual);
  }
  clut_register_failure(file, line);
}

CLUT_API void ClutAssertNotEqualChar(char expected, char actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_char, expected != actual, actual, CLUT_MSG_NOT_EQUAL, expected, file, line, msg); }
CLUT_API void ClutAssertNotEqualInt(int expected, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_int, expected != actual, actual, CLUT_MSG_NOT_EQUAL, expected, file, line, msg); }
CLUT_API void ClutAssertNotEqualUint(size_t expected, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_uint, expected != actual, actual, CLUT_MSG_NOT_EQUAL, expected, file, line, msg); }
CLUT_API void ClutAssertNotEqualFloat(float expected, float actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_message_append_float, !clut_floats_are_close(expected, actual), actual, CLUT_MSG_NOT_EQUAL, expected, file, line, msg);
}
CLUT_API void ClutAssertNotEqualDouble(double expected, double actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_message_append_double, !clut_doubles_are_close(expected, actual), actual, CLUT_MSG_NOT_EQUAL, expected, file, line, msg);
}

CLUT_API void ClutAssertNotEqualString(const char *expected, const char *actual, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  bool is_equal;
  if (expected == actual)
    is_equal = true;
  else if (expected == NULL || actual == NULL)
    is_equal = false;
  else {
    is_equal = true;
    for (size_t i = 0; expected[i] || actual[i]; i++) {
      if (expected[i] != actual[i]) {
        is_equal = false;
        break;
      }
    }
  }
  if (!is_equal)
    return;
  CLUT_ASSERT_COMPARE(clut_message_append_str, !is_equal, actual, CLUT_MSG_NOT_EQUAL, expected, file, line, msg);
}

CLUT_API void ClutAssertNotEqualStringLen(const char *expected, const char *actual, size_t len, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  bool is_equal;
  if (expected == actual)
    is_equal = true;
  else if (expected == NULL || actual == NULL)
    is_equal = false;
  else {
    is_equal = true;
    for (size_t i = 0; i < len && (expected[i] || actual[i]); i++) {
      if (expected[i] != actual[i]) {
        is_equal = false;
        break;
      }
    }
  }
  if (!is_equal)
    return;
  if (msg) {
    clut_message_append(msg);
  } else {
    clut_message_append(CLUT_MSG_EXPECTED);
    clut_message_append_str(actual);
    clut_message_append(CLUT_MSG_NOT_EQUAL);
    clut_message_append_str(expected);
    clut_message_append(CLUT_MSG_UNTIL_LEN);
    clut_message_append_uint(len);
  }
  clut_register_failure(file, line);
}

CLUT_API void ClutAssertNotEqualPtr(const void *expected, const void *actual, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  if (expected != actual)
    return;
  if (msg) {
    clut_message_append(msg);
  } else {
    CLUT_FORMAT_COMPARISON_MESSAGE(clut_message_append_ptr, actual, CLUT_MSG_NOT_EQUAL, expected);
  }
  clut_register_failure(file, line);
}

CLUT_API void ClutAssertCharGreaterThan(char threshold, char actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_char, actual > threshold, actual, CLUT_MSG_GREATER_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertCharGreaterOrEqual(char threshold, char actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_char, actual >= threshold, actual, CLUT_MSG_GREATER_OR_EQUAL, threshold, file, line, msg); }
CLUT_API void ClutAssertCharLessThan(char threshold, char actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_char, actual < threshold, actual, CLUT_MSG_LESS_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertCharLessOrEqual(char threshold, char actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_char, actual <= threshold, actual, CLUT_MSG_LESS_OR_EQUAL, threshold, file, line, msg); }

CLUT_API void ClutAssertIntGreaterThan(int threshold, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_int, actual > threshold, actual, CLUT_MSG_GREATER_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertIntGreaterOrEqual(int threshold, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_int, actual >= threshold, actual, CLUT_MSG_GREATER_OR_EQUAL, threshold, file, line, msg); }
CLUT_API void ClutAssertIntLessThan(int threshold, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_int, actual < threshold, actual, CLUT_MSG_LESS_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertIntLessOrEqual(int threshold, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_int, actual <= threshold, actual, CLUT_MSG_LESS_OR_EQUAL, threshold, file, line, msg); }

CLUT_API void ClutAssertUintGreaterThan(size_t threshold, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_uint, actual > threshold, actual, CLUT_MSG_GREATER_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertUintGreaterOrEqual(size_t threshold, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_uint, actual >= threshold, actual, CLUT_MSG_GREATER_OR_EQUAL, threshold, file, line, msg); }
CLUT_API void ClutAssertUintLessThan(size_t threshold, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_uint, actual < threshold, actual, CLUT_MSG_LESS_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertUintLessOrEqual(size_t threshold, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_uint, actual <= threshold, actual, CLUT_MSG_LESS_OR_EQUAL, threshold, file, line, msg); }

CLUT_API void ClutAssertFloatGreaterThan(float threshold, float actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_float, actual > threshold, actual, CLUT_MSG_GREATER_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertFloatGreaterOrEqual(float threshold, float actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_message_append_float, actual > threshold || clut_floats_are_close(threshold, actual), actual, CLUT_MSG_GREATER_OR_EQUAL, threshold, file, line, msg);
}
CLUT_API void ClutAssertFloatLessThan(float threshold, float actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_float, actual < threshold, actual, CLUT_MSG_LESS_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertFloatLessOrEqual(float threshold, float actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_message_append_float, actual < threshold || clut_floats_are_close(threshold, actual), actual, CLUT_MSG_LESS_OR_EQUAL, threshold, file, line, msg);
}

CLUT_API void ClutAssertDoubleGreaterThan(double threshold, double actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_double, actual > threshold, actual, CLUT_MSG_GREATER_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertDoubleGreaterOrEqual(double threshold, double actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_message_append_double, actual > threshold || clut_doubles_are_close(threshold, actual), actual, CLUT_MSG_GREATER_OR_EQUAL, threshold, file, line, msg);
}
CLUT_API void ClutAssertDoubleLessThan(double threshold, double actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_double, actual < threshold, actual, CLUT_MSG_LESS_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertDoubleLessOrEqual(double threshold, double actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_message_append_double, actual < threshold || clut_doubles_are_close(threshold, actual), actual, CLUT_MSG_LESS_OR_EQUAL, threshold, file, line, msg);
}

CLUT_API void ClutAssertWithinChar(char expected, size_t delta, char actual, const char *file, int line, const char *msg) {
  size_t diff = (actual > expected) ? (actual - expected) : (expected - actual);
  CLUT_ASSERT_WITHIN(clut_message_append_char, clut_message_append_uint, diff, expected, delta, file, line, msg);
}
CLUT_API void ClutAssertWithinInt(int expected, size_t delta, int actual, const char *file, int line, const char *msg) {
  size_t diff = (actual > expected) ? (actual - expected) : (expected - actual);
  CLUT_ASSERT_WITHIN(clut_message_append_int, clut_message_append_uint, diff, expected, delta, file, line, msg);
}
CLUT_API void ClutAssertWithinUint(size_t expected, size_t delta, size_t actual, const char *file, int line, const char *msg) {
  size_t diff = (actual > expected) ? (actual - expected) : (expected - actual);
  CLUT_ASSERT_WITHIN(clut_message_append_uint, clut_message_append_uint, diff, expected, delta, file, line, msg);
}
CLUT_API void ClutAssertWithinFloat(float expected, float delta, float actual, const char *file, int line, const char *msg) {
  float diff = clut_abs_float(actual - expected);
  CLUT_ASSERT_WITHIN(clut_message_append_float, clut_message_append_float, diff, expected, delta, file, line, msg);
}
CLUT_API void ClutAssertWithinDouble(double expected, double delta, double actual, const char *file, int line, const char *msg) {
  double diff = clut_abs_double(actual - expected);
  CLUT_ASSERT_WITHIN(clut_message_append_double, clut_message_append_double, diff, expected, delta, file, line, msg);
}

CLUT_API void ClutAssertEqualMemory(const void *expected, const void *actual, size_t len, size_t num_elements, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  CLUT_CHECK_BUFFER_ARGS;
  if (len == 0)
    return;

  const unsigned char *expected_bytes = (const unsigned char *)expected;
  const unsigned char *actual_bytes = (const unsigned char *)actual;

  for (size_t element_index = 0; element_index < num_elements; element_index++) {
    for (size_t byte_index = 0; byte_index < len; byte_index++) {
      size_t offset = element_index * len + byte_index;
      if (expected_bytes[offset] == actual_bytes[offset])
        continue;

      if (msg) {
        clut_message_append(msg);
      } else {
        if (num_elements > 1) {
          clut_message_append_array_prefix(element_index);
          clut_message_append(CLUT_MSG_BYTE_OFFSET);
          clut_message_append_format("[%zu] ", byte_index);
        } else {
          clut_message_append(CLUT_MSG_MISMATCH_AT);
          clut_message_append_uint(byte_index);
        }
        CLUT_FORMAT_COMPARISON_MESSAGE(clut_message_append_hex, expected_bytes[offset], CLUT_MSG_WAS, actual_bytes[offset]);
      }
      clut_register_failure(file, line);
      return;
    }
  }
}

#define CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, index, append_fn, exp_val, act_val)                                                                                                                                                                            \
  do {                                                                                                                                                                                                                                                             \
    if (msg) {                                                                                                                                                                                                                                                     \
      clut_message_append((msg));                                                                                                                                                                                                                                  \
    } else {                                                                                                                                                                                                                                                       \
      clut_message_append_array_prefix((index));                                                                                                                                                                                                                   \
      CLUT_FORMAT_COMPARISON_MESSAGE(append_fn, (exp_val), CLUT_MSG_WAS, (act_val));                                                                                                                                                                               \
    }                                                                                                                                                                                                                                                              \
    clut_register_failure(file, line);                                                                                                                                                                                                                             \
  } while (0)

CLUT_API void ClutAssertEqualCharArray(const char *expected, const char *actual, size_t num_elements, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  CLUT_CHECK_BUFFER_ARGS;
  for (size_t i = 0; i < num_elements; i++) {
    if (expected[i] == actual[i])
      continue;
    CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, i, clut_message_append_char, expected[i], actual[i]);
    return;
  }
}

CLUT_API void ClutAssertEqualIntArray(const int *expected, const int *actual, size_t num_elements, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  CLUT_CHECK_BUFFER_ARGS;
  for (size_t i = 0; i < num_elements; i++) {
    if (expected[i] == actual[i])
      continue;
    CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, i, clut_message_append_int, expected[i], actual[i]);
    return;
  }
}

CLUT_API void ClutAssertEqualUintArray(const size_t *expected, const size_t *actual, size_t num_elements, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  CLUT_CHECK_BUFFER_ARGS;
  for (size_t i = 0; i < num_elements; i++) {
    if (expected[i] == actual[i])
      continue;
    CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, i, clut_message_append_uint, expected[i], actual[i]);
    return;
  }
}

CLUT_API void ClutAssertEqualFloatArray(const float *expected, const float *actual, size_t num_elements, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  CLUT_CHECK_BUFFER_ARGS;
  for (size_t i = 0; i < num_elements; i++) {
    if (clut_floats_are_close(expected[i], actual[i]))
      continue;
    CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, i, clut_message_append_float, expected[i], actual[i]);
    return;
  }
}

CLUT_API void ClutAssertEqualDoubleArray(const double *expected, const double *actual, size_t num_elements, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  CLUT_CHECK_BUFFER_ARGS;
  for (size_t i = 0; i < num_elements; i++) {
    if (clut_doubles_are_close(expected[i], actual[i]))
      continue;
    CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, i, clut_message_append_double, expected[i], actual[i]);
    return;
  }
}

CLUT_API void ClutAssertEqualStringArray(const char **expected, const char **actual, size_t num_elements, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  CLUT_CHECK_BUFFER_ARGS;
  for (size_t i = 0; i < num_elements; i++) {
    const char *expected_text = expected[i];
    const char *actual_text = actual[i];
    bool is_equal = false;
    if (expected_text == actual_text)
      is_equal = true;
    else if (expected_text == NULL || actual_text == NULL)
      is_equal = false;
    else {
      for (size_t j = 0; expected_text[j] || actual_text[j]; j++) {
        if (expected_text[j] != actual_text[j]) {
          is_equal = false;
          break;
        }
      }
    }
    if (is_equal)
      continue;
    CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, i, clut_message_append_str, expected_text, actual_text);
    return;
  }
}

#define CLUT_ARRAY_WITHIN_RECORD(file, line, msg, index, append_fn_val, append_fn_num, exp_val, delta_val, diff_val)                                                                                                                                               \
  do {                                                                                                                                                                                                                                                             \
    if (msg) {                                                                                                                                                                                                                                                     \
      clut_message_append((msg));                                                                                                                                                                                                                                  \
    } else {                                                                                                                                                                                                                                                       \
      clut_message_append_array_prefix((index));                                                                                                                                                                                                                   \
      CLUT_FORMAT_WITHIN_MESSAGE(append_fn_val, append_fn_num, (exp_val), (delta_val), (diff_val));                                                                                                                                                                \
    }                                                                                                                                                                                                                                                              \
    clut_register_failure(file, line);                                                                                                                                                                                                                             \
  } while (0)

CLUT_API void ClutAssertWithinCharArray(const char *expected, size_t delta, const char *actual, size_t num_elements, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  CLUT_CHECK_BUFFER_ARGS;
  for (size_t i = 0; i < num_elements; i++) {
    size_t diff = (actual[i] > expected[i]) ? (actual[i] - expected[i]) : (expected[i] - actual[i]);
    if (diff <= delta)
      continue;
    CLUT_ARRAY_WITHIN_RECORD(file, line, msg, i, clut_message_append_char, clut_message_append_uint, expected[i], delta, diff);
    return;
  }
}

CLUT_API void ClutAssertWithinIntArray(const int *expected, size_t delta, const int *actual, size_t num_elements, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  CLUT_CHECK_BUFFER_ARGS;
  for (size_t i = 0; i < num_elements; i++) {
    size_t diff = (actual[i] > expected[i]) ? (actual[i] - expected[i]) : (expected[i] - actual[i]);
    if (diff <= delta)
      continue;
    CLUT_ARRAY_WITHIN_RECORD(file, line, msg, i, clut_message_append_int, clut_message_append_uint, expected[i], delta, diff);
    return;
  }
}

CLUT_API void ClutAssertWithinUintArray(const size_t *expected, size_t delta, const size_t *actual, size_t num_elements, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  CLUT_CHECK_BUFFER_ARGS;
  for (size_t i = 0; i < num_elements; i++) {
    size_t diff = (actual[i] > expected[i]) ? (actual[i] - expected[i]) : (expected[i] - actual[i]);
    if (diff <= delta)
      continue;
    CLUT_ARRAY_WITHIN_RECORD(file, line, msg, i, clut_message_append_uint, clut_message_append_uint, expected[i], delta, diff);
    return;
  }
}

CLUT_API void ClutAssertWithinFloatArray(const float *expected, float delta, const float *actual, size_t num_elements, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  CLUT_CHECK_BUFFER_ARGS;
  for (size_t i = 0; i < num_elements; i++) {
    float diff = clut_abs_float(actual[i] - expected[i]);
    if (diff <= delta)
      continue;
    CLUT_ARRAY_WITHIN_RECORD(file, line, msg, i, clut_message_append_float, clut_message_append_float, expected[i], delta, diff);
    return;
  }
}

CLUT_API void ClutAssertWithinDoubleArray(const double *expected, double delta, const double *actual, size_t num_elements, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  CLUT_CHECK_BUFFER_ARGS;
  for (size_t i = 0; i < num_elements; i++) {
    double diff = clut_abs_double(actual[i] - expected[i]);
    if (diff <= delta)
      continue;
    CLUT_ARRAY_WITHIN_RECORD(file, line, msg, i, clut_message_append_double, clut_message_append_double, expected[i], delta, diff);
    return;
  }
}

#endif /* CLUT_IMPLEMENTATION_INCLUDED */
#endif /* CLUT_IMPLEMENTATION */
