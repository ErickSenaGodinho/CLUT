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
#ifndef CLUT_MSG_USER_SEPARATOR
#define CLUT_MSG_USER_SEPARATOR ": "
#endif

/* Utility Types */
typedef struct {
  char *items;
  size_t count;
  size_t capacity;
} ClutStringBuilder;

typedef struct {
  size_t total_tests;
  size_t passed;
  size_t failed;
  size_t skipped;
  double total_seconds;
} ClutSummary;

/* Function Types */
typedef struct {
  size_t current_repetition;
  size_t total_repetitions;
} ClutRepetitionInfo;

typedef void (*ClutHookFn)();
typedef void (*ClutTestFn)();
typedef void (*ClutRepeatedTestFn)(ClutRepetitionInfo);
typedef void (*ClutParamTestFn)(const void *param);

/* Test Data */
typedef struct {
  const char *file;
  int line;
  int iteration;
  const char *message;
  size_t message_offset;
} ClutTestFailure;

typedef struct {
  ClutTestFailure *items;
  size_t count;
  size_t capacity;
} ClutTestFailureList;

typedef struct {
  const char *test_name;
  const ClutTestFailure *failures;
  size_t failure_count;
  double duration;
} ClutTestResult;

/* Test Registration */
typedef struct {
  ClutTestFn fn;
  const char *name;
} ClutRegisteredTest;

typedef struct {
  ClutRegisteredTest *items;
  size_t count;
  size_t capacity;
} ClutRegisteredTests;

/* Runner State */
typedef struct {
  ClutTestFailureList failures;
  size_t next_message_offset;
  ClutStringBuilder failure_message;
  ClutSummary summary;
} ClutRunnerState;

/* Suite State */
typedef struct {
  size_t total_tests;
  size_t passed;
  size_t failed;
  size_t skipped;
  long start_time;
  ClutRegisteredTests registered_tests;
} ClutSuiteState;

/* Test Lifecycle */
typedef struct {
  ClutHookFn before_all;
  ClutHookFn before_each;
  ClutHookFn after_each;
  ClutHookFn after_all;
} ClutHooks;

typedef struct {
  const char *name;
  int iteration;
  bool failed;
  bool skipped;
  long start_time;
#ifdef CLUT_HAS_LONGJMP
  jmp_buf jmp_buffer;
#endif
} ClutTestState;

/* Core Context */
typedef struct {
  ClutRunnerState runner;
  ClutSuiteState suite;
  ClutHooks hooks;
  ClutTestState current_test;
} ClutContext;

/* Reporter */
typedef struct {
  void (*on_runner_start)(void *context);
  void (*on_test_start)(void *context, const char *test_name);
  void (*on_test_pass)(void *context, const ClutTestResult *result);
  void (*on_test_fail)(void *context, const ClutTestResult *result);
  void (*on_test_skip)(void *context, const ClutTestResult *result);
  void (*on_suite_end)(void *context, const ClutSummary *summary);
  void (*on_runner_end)(void *context, const ClutSummary *summary);

  void *context;
} ClutReporter;

#define CLUT_REPORTER_SYMBOL(name) clut_reporter_##name
#define REPORTER(name, ...) CLUT_API const ClutReporter CLUT_REPORTER_SYMBOL(name) = {__VA_ARGS__}
#define USE_REPORTER(name) ClutSetReporter(&CLUT_REPORTER_SYMBOL(name))

/* TEST / REPEATED_TEST / PARAM_TEST macros */
#define CLUT_TEST_BODY(name) clut_test_##name
#define CLUT_TEST_WRAPPER(name) clut_run_test_##name

#define TEST(name)                                                                                                                                                                                                                                                 \
  static void CLUT_TEST_BODY(name)(void);                                                                                                                                                                                                                          \
  CLUT_API void CLUT_TEST_WRAPPER(name)(void) { ClutRunTest(CLUT_TEST_BODY(name)); }                                                                                                                                                                               \
  static void CLUT_TEST_BODY(name)(void)

#define REPEATED_TEST(name, repetitions)                                                                                                                                                                                                                           \
  static void CLUT_TEST_BODY(name)(ClutRepetitionInfo input);                                                                                                                                                                                                      \
  CLUT_API void CLUT_TEST_WRAPPER(name)(void) { ClutRunRepeated(CLUT_TEST_BODY(name), repetitions); }                                                                                                                                                              \
  static void CLUT_TEST_BODY(name)(ClutRepetitionInfo input)

#define REPEATED_TEST_WITH_THRESHOLD(name, repetitions, failure_threshold)                                                                                                                                                                                         \
  static void CLUT_TEST_BODY(name)(ClutRepetitionInfo input);                                                                                                                                                                                                      \
  CLUT_API void CLUT_TEST_WRAPPER(name)(void) { ClutRunRepeatedWithThreshold(CLUT_TEST_BODY(name), repetitions, failure_threshold); }                                                                                                                              \
  static void CLUT_TEST_BODY(name)(ClutRepetitionInfo input)

#define PARAM_TEST(name, Type, ...)                                                                                                                                                                                                                                \
  static void CLUT_TEST_BODY(name)(Type input);                                                                                                                                                                                                                    \
  static void clut_param_thunk_##name(const void *p) { CLUT_TEST_BODY(name)(*(Type const *)p); }                                                                                                                                                                   \
  CLUT_API void CLUT_TEST_WRAPPER(name)(void) { ClutRunParamTest(clut_param_thunk_##name, Type, __VA_ARGS__); }                                                                                                                                                    \
  static void CLUT_TEST_BODY(name)(Type input)

/* Test Runner lifecycle macros */
#define RUNNER_BEGIN() ClutRunnerBegin()
#define RUNNER_END() ClutRunnerEnd()

/* Test Suite lifecycle macros */
#define SUITE_BEGIN() ClutSuiteBegin()
#define RUN_TEST(test_name) ClutSuiteRegisterTest(CLUT_TEST_WRAPPER(test_name), #test_name)
#define SUITE_END() ClutSuiteEnd()

/* Hook definition / Hook registration macros */
#define CLUT_HOOK_SYMBOL(name, suffix) clut_##name##_##suffix
#define CLUT_HOOK_PROTOTYPE(name, suffix) CLUT_API void CLUT_HOOK_SYMBOL(name, suffix)(void)

#define CLUT_HOOK_DEFINITION(name, suffix)                                                                                                                                                                                                                         \
  CLUT_HOOK_PROTOTYPE(name, suffix);                                                                                                                                                                                                                               \
  CLUT_HOOK_PROTOTYPE(name, suffix)

#define HOOK_BEFORE_ALL(name) CLUT_HOOK_DEFINITION(name, before_all)
#define HOOK_BEFORE_EACH(name) CLUT_HOOK_DEFINITION(name, before_each)
#define HOOK_AFTER_ALL(name) CLUT_HOOK_DEFINITION(name, after_all)
#define HOOK_AFTER_EACH(name) CLUT_HOOK_DEFINITION(name, after_each)

#define USE_HOOK_BEFORE_ALL(hook) ClutSetHookBeforeAll(CLUT_HOOK_SYMBOL(hook, before_all))
#define USE_HOOK_BEFORE_EACH(hook) ClutSetHookBeforeEach(CLUT_HOOK_SYMBOL(hook, before_each))
#define USE_HOOK_AFTER_ALL(hook) ClutSetHookAfterAll(CLUT_HOOK_SYMBOL(hook, after_all))
#define USE_HOOK_AFTER_EACH(hook) ClutSetHookAfterEach(CLUT_HOOK_SYMBOL(hook, after_each))

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

#define TEST_ASSERT_GREATER_THAN_CHAR(threshold, actual) ClutAssertGreaterThanChar((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_CHAR(threshold, actual) ClutAssertGreaterOrEqualChar((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_CHAR(threshold, actual) ClutAssertLessThanChar((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_CHAR(threshold, actual) ClutAssertLessOrEqualChar((threshold), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_INT(threshold, actual) ClutAssertGreaterThanInt((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT(threshold, actual) ClutAssertGreaterOrEqualInt((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT(threshold, actual) ClutAssertLessThanInt((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT(threshold, actual) ClutAssertLessOrEqualInt((threshold), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_UINT(threshold, actual) ClutAssertGreaterThanUint((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT(threshold, actual) ClutAssertGreaterOrEqualUint((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT(threshold, actual) ClutAssertLessThanUint((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT(threshold, actual) ClutAssertLessOrEqualUint((threshold), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_FLOAT(threshold, actual) ClutAssertGreaterThanFloat((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(threshold, actual) ClutAssertGreaterOrEqualFloat((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_FLOAT(threshold, actual) ClutAssertLessThanFloat((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_FLOAT(threshold, actual) ClutAssertLessOrEqualFloat((threshold), (actual), __FILE__, __LINE__, NULL)

#define TEST_ASSERT_GREATER_THAN_DOUBLE(threshold, actual) ClutAssertGreaterThanDouble((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(threshold, actual) ClutAssertGreaterOrEqualDouble((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_DOUBLE(threshold, actual) ClutAssertLessThanDouble((threshold), (actual), __FILE__, __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(threshold, actual) ClutAssertLessOrEqualDouble((threshold), (actual), __FILE__, __LINE__, NULL)

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
#define TEST_ASSERT_EQUAL_PTR_MESSAGE(expected, actual, msg) ClutAssertEqualPtr((expected), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_NOT_EQUAL_CHAR_MESSAGE(expected, actual, msg) ClutAssertNotEqualChar((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_INT_MESSAGE(expected, actual, msg) ClutAssertNotEqualInt((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_UINT_MESSAGE(expected, actual, msg) ClutAssertNotEqualUint((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_FLOAT_MESSAGE(expected, actual, msg) ClutAssertNotEqualFloat((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_DOUBLE_MESSAGE(expected, actual, msg) ClutAssertNotEqualDouble((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_STRING_MESSAGE(expected, actual, msg) ClutAssertNotEqualString((expected), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_STRING_LEN_MESSAGE(expected, actual, len, msg) ClutAssertNotEqualStringLen((expected), (actual), (len), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_NOT_EQUAL_PTR_MESSAGE(expected, actual, msg) ClutAssertNotEqualPtr((expected), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_CHAR_MESSAGE(threshold, actual, msg) ClutAssertGreaterThanChar((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_CHAR_MESSAGE(threshold, actual, msg) ClutAssertGreaterOrEqualChar((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_CHAR_MESSAGE(threshold, actual, msg) ClutAssertLessThanChar((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_CHAR_MESSAGE(threshold, actual, msg) ClutAssertLessOrEqualChar((threshold), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_INT_MESSAGE(threshold, actual, msg) ClutAssertGreaterThanInt((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(threshold, actual, msg) ClutAssertGreaterOrEqualInt((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_INT_MESSAGE(threshold, actual, msg) ClutAssertLessThanInt((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_INT_MESSAGE(threshold, actual, msg) ClutAssertLessOrEqualInt((threshold), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(threshold, actual, msg) ClutAssertGreaterThanUint((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT_MESSAGE(threshold, actual, msg) ClutAssertGreaterOrEqualUint((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_UINT_MESSAGE(threshold, actual, msg) ClutAssertLessThanUint((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT_MESSAGE(threshold, actual, msg) ClutAssertLessOrEqualUint((threshold), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_FLOAT_MESSAGE(threshold, actual, msg) ClutAssertGreaterThanFloat((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_FLOAT_MESSAGE(threshold, actual, msg) ClutAssertGreaterOrEqualFloat((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_FLOAT_MESSAGE(threshold, actual, msg) ClutAssertLessThanFloat((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_FLOAT_MESSAGE(threshold, actual, msg) ClutAssertLessOrEqualFloat((threshold), (actual), __FILE__, __LINE__, (msg))

#define TEST_ASSERT_GREATER_THAN_DOUBLE_MESSAGE(threshold, actual, msg) ClutAssertGreaterThanDouble((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE_MESSAGE(threshold, actual, msg) ClutAssertGreaterOrEqualDouble((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_THAN_DOUBLE_MESSAGE(threshold, actual, msg) ClutAssertLessThanDouble((threshold), (actual), __FILE__, __LINE__, (msg))
#define TEST_ASSERT_LESS_OR_EQUAL_DOUBLE_MESSAGE(threshold, actual, msg) ClutAssertLessOrEqualDouble((threshold), (actual), __FILE__, __LINE__, (msg))

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
CLUT_API void ClutSetReporter(const ClutReporter *reporter);

CLUT_API void ClutRunnerBegin();
CLUT_API int ClutRunnerEnd();

CLUT_API void ClutSuiteBegin();
CLUT_API void ClutSuiteRegisterTest(ClutTestFn test_fn, const char *test_name);
CLUT_API void ClutSuiteEnd();

CLUT_API void ClutSetHookBeforeAll(ClutHookFn hook_fn);
CLUT_API void ClutSetHookBeforeEach(ClutHookFn hook_fn);
CLUT_API void ClutSetHookAfterAll(ClutHookFn hook_fn);
CLUT_API void ClutSetHookAfterEach(ClutHookFn hook_fn);

CLUT_API void ClutRunTest(ClutTestFn test_fn);
CLUT_API void ClutRunRepeated(ClutRepeatedTestFn test_fn, size_t repetitions);
CLUT_API void ClutRunRepeatedWithThreshold(ClutRepeatedTestFn test_fn, size_t repetitions, size_t failure_threshold);
CLUT_API void ClutRunParamTestImpl(ClutParamTestFn test_fn, const void *items, size_t item_size, size_t count);

#define ClutRunParamTest(test_fn, Type, ...)                                                                                                                                                                                                                       \
  do {                                                                                                                                                                                                                                                             \
    static Type const clut_params_[] = __VA_ARGS__;                                                                                                                                                                                                                \
    ClutRunParamTestImpl((test_fn), clut_params_, sizeof(Type), sizeof(clut_params_) / sizeof(Type));                                                                                                                                                              \
  } while (0)

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

CLUT_API void ClutAssertGreaterThanInt(int expected, int actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertGreaterOrEqualInt(int expected, int actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertLessThanInt(int expected, int actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertLessOrEqualInt(int expected, int actual, const char *file, int line, const char *msg);

CLUT_API void ClutAssertGreaterThanUint(size_t expected, size_t actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertGreaterOrEqualUint(size_t expected, size_t actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertLessThanUint(size_t expected, size_t actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertLessOrEqualUint(size_t expected, size_t actual, const char *file, int line, const char *msg);

CLUT_API void ClutAssertGreaterThanFloat(float expected, float actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertGreaterOrEqualFloat(float expected, float actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertLessThanFloat(float expected, float actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertLessOrEqualFloat(float expected, float actual, const char *file, int line, const char *msg);

CLUT_API void ClutAssertGreaterThanDouble(double expected, double actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertGreaterOrEqualDouble(double expected, double actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertLessThanDouble(double expected, double actual, const char *file, int line, const char *msg);
CLUT_API void ClutAssertLessOrEqualDouble(double expected, double actual, const char *file, int line, const char *msg);

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

/* Dynamic Array */
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
static void clut_string_builder_append(ClutStringBuilder *sb, const char *str) {
  if (!str)
    str = "(null)";
  size_t n = strlen(str);
  clut_da_append_many(sb, str, n);
}

static void clut_string_builder_append_char(ClutStringBuilder *sb, char c) { clut_da_append(sb, c); }

static void clut_string_builder_append_format(ClutStringBuilder *sb, const char *fmt, ...) {
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

CLUT_STATE_LINKAGE ClutContext Clut = {0};
CLUT_STATE_LINKAGE const ClutReporter *ClutCustomReporter = NULL;

/* Abort Helpers */
#ifndef CLUT_ABORT_TEST
#ifdef CLUT_HAS_LONGJMP
#define CLUT_ABORT_TEST longjmp(Clut.current_test.jmp_buffer, 1)
#else
#define CLUT_ABORT_TEST return
#endif // CLUT_HAS_LONGJMP
#endif // CLUT_ABORT_TEST

#define CLUT_ABORT_IF_FAILED_OR_SKIPPED                                                                                                                                                                                                                            \
  do {                                                                                                                                                                                                                                                             \
    if (Clut.current_test.failed || Clut.current_test.skipped) {                                                                                                                                                                                                   \
      CLUT_ABORT_TEST;                                                                                                                                                                                                                                             \
    }                                                                                                                                                                                                                                                              \
  } while (0)

#ifdef CLUT_HAS_LONGJMP
#define CLUT_RUN_GUARDED(call)                                                                                                                                                                                                                                     \
  if (setjmp(Clut.current_test.jmp_buffer) == 0) {                                                                                                                                                                                                                 \
    call;                                                                                                                                                                                                                                                          \
  }
#else
#define CLUT_RUN_GUARDED(call)                                                                                                                                                                                                                                     \
  {                                                                                                                                                                                                                                                                \
    call;                                                                                                                                                                                                                                                          \
  }
#endif

/* Reporters implementation */
#if defined(CLUT_OUTPUT_GITHUB)
static void clut_report_pass(void *context, const ClutTestResult *result) {
  (void)context;
  fprintf(CLUT_STREAM_DEFAULT, "::notice title=PASS::%s (%.3fs)\n", result->test_name, result->duration);
}

static void clut_github_print_escaped(FILE *stream, const char *text) {
  for (const char *p = text; *p; ++p) {
    switch (*p) {
    case '%':
      fputs("%25", stream);
      break;
    case '\r':
      fputs("%0D", stream);
      break;
    case '\n':
      fputs("%0A", stream);
      break;
    default:
      fputc(*p, stream);
    }
  }
}

static void clut_report_fail(void *context, const ClutTestResult *result) {
  (void)context;
  fprintf(CLUT_STREAM_FAIL, "::notice title=FAIL::%s (%.3fs)\n", result->test_name, result->duration);
  for (size_t i = 0; i < result->failure_count; ++i) {
    const ClutTestFailure *failure = &result->failures[i];
    fprintf(CLUT_STREAM_FAIL, "::error file=%s,line=%d,title=%s", failure->file, failure->line, result->test_name);
    if (failure->iteration >= 0) {
      fprintf(CLUT_STREAM_FAIL, "[%d]", failure->iteration);
    }
    fputs("::", CLUT_STREAM_FAIL);
    clut_github_print_escaped(CLUT_STREAM_FAIL, failure->message);
    fputc('\n', CLUT_STREAM_FAIL);
  }
}

static void clut_report_skip(void *context, const ClutTestResult *result) {
  (void)context;
  fprintf(CLUT_STREAM_DEFAULT, "::notice title=SKIP::%s\n", result->test_name);
}

static void clut_report_suite_end(void *context, const ClutSummary *summary) {
  (void)context;
  fprintf(CLUT_STREAM_DEFAULT, "::notice title=Suite Summary::Tests=%zu Passed=%zu Failed=%zu Skipped=%zu Time=%.3fs\n", summary->total_tests, summary->passed, summary->failed, summary->skipped, summary->total_seconds);
  fprintf(CLUT_STREAM_DEFAULT, "\n");
}

static void clut_report_runner_end(void *context, const ClutSummary *summary) {
  (void)context;
  fprintf(CLUT_STREAM_DEFAULT, "::notice title=Runner Summary::Total Tests=%zu Passed=%zu Failed=%zu Skipped=%zu Time=%.3fs\n", summary->total_tests, summary->passed, summary->failed, summary->skipped, summary->total_seconds);
  fprintf(CLUT_STREAM_DEFAULT, "\n");
}

static const ClutReporter ClutGithubReporter = {
    .on_test_pass = clut_report_pass,
    .on_test_fail = clut_report_fail,
    .on_test_skip = clut_report_skip,
    .on_suite_end = clut_report_suite_end,
    .on_runner_end = clut_report_runner_end,
};

#define CLUT_REPORTER ClutGithubReporter
#elif defined(CLUT_OUTPUT_TAP)

typedef struct {
  size_t clut_tap_test_index;
  bool clut_tap_plan_printed;
} ClutTapContext;

static ClutTapContext clut_tap_context = {0};

static void clut_report_tap_plan(ClutTapContext *context) {
  if (!context->clut_tap_plan_printed) {
    fprintf(CLUT_STREAM_DEFAULT, "1..%zu\n", Clut.suite.total_tests);
    context->clut_tap_plan_printed = true;
  }
}

static void clut_report_pass(void *context, const ClutTestResult *result) {
  clut_report_tap_plan(context);
  ClutTapContext *ctx = (ClutTapContext *)context;
  ctx->clut_tap_test_index++;
  fprintf(CLUT_STREAM_DEFAULT, "ok %zu - %s # (%.3fs)\n", ctx->clut_tap_test_index, result->test_name, result->duration);
}

static void clut_tap_print_failure(const ClutTestFailure *failure) {
  FILE *stream = CLUT_STREAM_DEFAULT;
  fprintf(stream, "# %s:%d: ", failure->file, failure->line);
  for (const char *p = failure->message; *p; ++p) {
    fputc(*p, stream);
    if (*p == '\n') {
      fputs("# ", stream);
    }
  }
  if (failure->iteration >= 0) {
    fprintf(stream, " [iteration %d]", failure->iteration);
  }
  fputc('\n', stream);
}

static void clut_report_fail(void *context, const ClutTestResult *result) {
  clut_report_tap_plan(context);
  ClutTapContext *ctx = (ClutTapContext *)context;
  ctx->clut_tap_test_index++;
  fprintf(CLUT_STREAM_DEFAULT, "not ok %zu - %s # (%.3fs)\n", ctx->clut_tap_test_index, result->test_name, result->duration);
  for (size_t i = 0; i < result->failure_count; ++i) {
    clut_tap_print_failure(&result->failures[i]);
  }
}

static void clut_report_skip(void *context, const ClutTestResult *result) {
  clut_report_tap_plan(context);
  ClutTapContext *ctx = (ClutTapContext *)context;
  ctx->clut_tap_test_index++;
  fprintf(CLUT_STREAM_DEFAULT, "ok %zu - %s # SKIP\n", ctx->clut_tap_test_index, result->test_name);
}

static void clut_report_suite_end(void *context, const ClutSummary *summary) {
  clut_report_tap_plan(context);
  fprintf(CLUT_STREAM_DEFAULT, "# Suite Summary: Tests: %zu, Passed: %zu, Failed: %zu, Skipped: %zu, Time: %.3fs\n", summary->total_tests, summary->passed, summary->failed, summary->skipped, summary->total_seconds);
  fprintf(CLUT_STREAM_DEFAULT, "\n");

  ClutTapContext *ctx = (ClutTapContext *)context;
  ctx->clut_tap_test_index = 0;
  ctx->clut_tap_plan_printed = false;
}

static void clut_report_runner_end(void *context, const ClutSummary *summary) {
  (void)context;
  fprintf(CLUT_STREAM_DEFAULT, "# Runner Summary: Tests: %zu, Passed: %zu, Failed: %zu, Skipped: %zu, Time: %.3fs\n", summary->total_tests, summary->passed, summary->failed, summary->skipped, summary->total_seconds);
  fprintf(CLUT_STREAM_DEFAULT, "\n");
}

static const ClutReporter ClutTapReporter = {
    .on_test_pass = clut_report_pass,
    .on_test_fail = clut_report_fail,
    .on_test_skip = clut_report_skip,
    .on_suite_end = clut_report_suite_end,
    .on_runner_end = clut_report_runner_end,
    .context = &clut_tap_context,
};

#define CLUT_REPORTER ClutTapReporter

#else /* default */

static void clut_report_pass(void *context, const ClutTestResult *result) {
  (void)context;
  fprintf(CLUT_STREAM_DEFAULT, "%s%-50s%.3fs\n", CLUT_GREEN_TEXT("[ PASS ] "), result->test_name, result->duration);
}

static void clut_report_fail(void *context, const ClutTestResult *result) {
  (void)context;
  fprintf(CLUT_STREAM_FAIL, "%s%s%-50s%.3fs%s\n", CLUT_RED_TEXT("[ FAIL ] "), CLUT_MSG_BEGIN_RED_TEXT, result->test_name, result->duration, CLUT_MSG_END_COLOR_TEXT);
  for (size_t i = 0; i < result->failure_count; ++i) {
    const ClutTestFailure *f = &result->failures[i];
    fprintf(CLUT_STREAM_FAIL, "%s%s:%d:%s", CLUT_MSG_BEGIN_RED_TEXT, f->file, f->line, result->test_name);
    if (f->iteration >= 0)
      fprintf(CLUT_STREAM_FAIL, "[%d]", f->iteration);
    fprintf(CLUT_STREAM_FAIL, ":%s%s\n", f->message, CLUT_MSG_END_COLOR_TEXT);
  }
}

static void clut_report_skip(void *context, const ClutTestResult *result) {
  (void)context;
  fprintf(CLUT_STREAM_DEFAULT, "%s%-50s\n", CLUT_YELLOW_TEXT("[ SKIP ] "), result->test_name);
}

static void clut_report_suite_end(void *context, const ClutSummary *summary) {
  (void)context;
  fprintf(CLUT_STREAM_DEFAULT, "--------------------------------\n");
  fprintf(CLUT_STREAM_DEFAULT, CLUT_BLUE_TEXT("Tests:      ") CLUT_GREEN_TEXT("%zu\n"), summary->total_tests);
  fprintf(CLUT_STREAM_DEFAULT, CLUT_BLUE_TEXT("Passed:     ") CLUT_GREEN_TEXT("%zu\n"), summary->passed);
  fprintf(CLUT_STREAM_DEFAULT, CLUT_BLUE_TEXT("Failed:     ") CLUT_RED_TEXT("%zu\n"), summary->failed);
  fprintf(CLUT_STREAM_DEFAULT, CLUT_BLUE_TEXT("Skipped:    ") CLUT_YELLOW_TEXT("%zu\n"), summary->skipped);
  fprintf(CLUT_STREAM_DEFAULT, "--------------------------------\n");
  fprintf(CLUT_STREAM_DEFAULT, CLUT_BLUE_TEXT("Total time: %.3fs\n"), summary->total_seconds);
  fprintf(CLUT_STREAM_DEFAULT, "\n");
}

static void clut_report_runner_end(void *context, const ClutSummary *summary) {
  (void)context;
  fprintf(CLUT_STREAM_DEFAULT, "================================\n");
  fprintf(CLUT_STREAM_DEFAULT, CLUT_BLUE_TEXT("RUNNER SUMMARY\n"));
  fprintf(CLUT_STREAM_DEFAULT, "================================\n");
  fprintf(CLUT_STREAM_DEFAULT, CLUT_BLUE_TEXT("Tests:      ") CLUT_GREEN_TEXT("%zu\n"), summary->total_tests);
  fprintf(CLUT_STREAM_DEFAULT, CLUT_BLUE_TEXT("Passed:     ") CLUT_GREEN_TEXT("%zu\n"), summary->passed);
  fprintf(CLUT_STREAM_DEFAULT, CLUT_BLUE_TEXT("Failed:     ") CLUT_RED_TEXT("%zu\n"), summary->failed);
  fprintf(CLUT_STREAM_DEFAULT, CLUT_BLUE_TEXT("Skipped:    ") CLUT_YELLOW_TEXT("%zu\n"), summary->skipped);
  fprintf(CLUT_STREAM_DEFAULT, "--------------------------------\n");
  fprintf(CLUT_STREAM_DEFAULT, CLUT_BLUE_TEXT("Total time: %.3fs\n"), summary->total_seconds);
  fprintf(CLUT_STREAM_DEFAULT, "\n");
}

static const ClutReporter ClutDefaultReporter = {
    .on_test_pass = clut_report_pass,
    .on_test_fail = clut_report_fail,
    .on_test_skip = clut_report_skip,
    .on_suite_end = clut_report_suite_end,
    .on_runner_end = clut_report_runner_end,
};

#define CLUT_REPORTER ClutDefaultReporter

#endif /* CLUT_REPORTER */

static const ClutReporter *clut_get_reporter(void) {
  if (ClutCustomReporter != NULL) {
    return ClutCustomReporter;
  }

  return &CLUT_REPORTER;
}

static void clut_register_failure(const char *file, int line) {
  Clut.current_test.failed = true;
  clut_string_builder_append_char(&Clut.runner.failure_message, '\0');
  ClutTestFailure failure = {file, line, Clut.current_test.iteration, NULL, Clut.runner.next_message_offset};
  Clut.runner.next_message_offset = Clut.runner.failure_message.count;
  clut_da_append(&Clut.runner.failures, failure);
  CLUT_ABORT_TEST;
}

static void clut_message_append(const char *msg) { clut_string_builder_append(&Clut.runner.failure_message, msg); }
static void clut_message_append_user_text(const char *msg) {
  if (msg) {
    clut_message_append(msg);
    clut_message_append(CLUT_MSG_USER_SEPARATOR);
  }
}
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
  clut_message_append_format("[%zu]: ", index);
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

#define CLUT_ASSERT_COMPARE(append_fn, condition, lhs, op_str, rhs, file, line, msg)                                                                                                                                                                               \
  do {                                                                                                                                                                                                                                                             \
    CLUT_ABORT_IF_FAILED_OR_SKIPPED;                                                                                                                                                                                                                               \
    if (condition)                                                                                                                                                                                                                                                 \
      return;                                                                                                                                                                                                                                                      \
    clut_message_append_user_text(msg);                                                                                                                                                                                                                            \
    CLUT_FORMAT_COMPARISON_MESSAGE(append_fn, (lhs), (op_str), (rhs));                                                                                                                                                                                             \
    clut_register_failure(file, line);                                                                                                                                                                                                                             \
  } while (0)

#define CLUT_ASSERT_WITHIN(append_fn_val, append_fn_num, diff_val, expected, delta, file, line, msg)                                                                                                                                                               \
  do {                                                                                                                                                                                                                                                             \
    CLUT_ABORT_IF_FAILED_OR_SKIPPED;                                                                                                                                                                                                                               \
    if ((diff_val) <= (delta))                                                                                                                                                                                                                                     \
      return;                                                                                                                                                                                                                                                      \
    clut_message_append_user_text(msg);                                                                                                                                                                                                                            \
    CLUT_FORMAT_WITHIN_MESSAGE(append_fn_val, append_fn_num, (expected), (delta), (diff_val));                                                                                                                                                                     \
    clut_register_failure(file, line);                                                                                                                                                                                                                             \
  } while (0)

#define CLUT_CHECK_BUFFER_ARGS                                                                                                                                                                                                                                     \
  do {                                                                                                                                                                                                                                                             \
    if (num_elements == 0)                                                                                                                                                                                                                                         \
      return;                                                                                                                                                                                                                                                      \
    if (expected == actual)                                                                                                                                                                                                                                        \
      return;                                                                                                                                                                                                                                                      \
    if (expected == NULL || actual == NULL) {                                                                                                                                                                                                                      \
      clut_message_append_user_text(msg);                                                                                                                                                                                                                          \
      clut_message_append(CLUT_MSG_MEMORY_NULL);                                                                                                                                                                                                                   \
      clut_register_failure(file, line);                                                                                                                                                                                                                           \
      return;                                                                                                                                                                                                                                                      \
    }                                                                                                                                                                                                                                                              \
  } while (0)

CLUT_API void ClutSetReporter(const ClutReporter *reporter) { ClutCustomReporter = reporter; }

CLUT_API void ClutRunnerBegin() {
  Clut.runner = (ClutRunnerState){0};

  const ClutReporter *reporter = clut_get_reporter();
  if (reporter->on_runner_start) {
    reporter->on_runner_start(reporter->context);
  }
}

CLUT_API int ClutRunnerEnd() {
  clut_da_free(&Clut.runner.failures);
  clut_da_free(&Clut.runner.failure_message);
  clut_da_free(&Clut.suite.registered_tests);

  const ClutReporter *reporter = clut_get_reporter();
  if (reporter->on_runner_end) {
    reporter->on_runner_end(reporter->context, &Clut.runner.summary);
  }

  return Clut.runner.summary.failed > 255 ? 255 : (int)Clut.runner.summary.failed;
}

static void clut_reset_suite(void) {
  Clut.suite.total_tests = 0;
  Clut.suite.passed = 0;
  Clut.suite.failed = 0;
  Clut.suite.skipped = 0;
  Clut.suite.start_time = 0;
  clut_da_clear(&Clut.suite.registered_tests);
}

CLUT_API void ClutSuiteBegin(void) {
  clut_reset_suite();
  Clut.hooks = (ClutHooks){0};
}

CLUT_API void ClutSuiteRegisterTest(ClutTestFn run_test_fn, const char *test_name) {
  ClutRegisteredTest pending_test = (ClutRegisteredTest){run_test_fn, test_name};
  clut_da_append(&Clut.suite.registered_tests, pending_test);
  Clut.suite.total_tests = Clut.suite.registered_tests.count;
}

static void clut_reset_current_test(void) {
  Clut.current_test.name = NULL;
  Clut.current_test.failed = false;
  Clut.current_test.skipped = false;
  Clut.current_test.iteration = -1;
  Clut.runner.next_message_offset = 0;
  clut_da_clear(&Clut.runner.failures);
  clut_da_clear(&Clut.runner.failure_message);
}

static void clut_run_hook(ClutHookFn hook) {
  if (hook == NULL)
    return;
  CLUT_RUN_GUARDED(hook());
}

static void clut_run_after_each(void) {
  if (Clut.hooks.after_each == NULL)
    return;

  bool test_failed = Clut.current_test.failed;
  bool test_skipped = Clut.current_test.skipped;

  Clut.current_test.failed = false;
  Clut.current_test.skipped = false;
  Clut.current_test.iteration = -1;

  clut_run_hook(Clut.hooks.after_each);

  Clut.current_test.failed = test_failed || Clut.current_test.failed;
  Clut.current_test.skipped = test_skipped || Clut.current_test.skipped;
}

static void clut_print_hook_failure(const char *hook_name) {
  if (Clut.current_test.failed) {
    const ClutTestFailure *failure = &Clut.runner.failures.items[0];
    fprintf(CLUT_STREAM_FAIL, "%s:%d: %s hook failed: %s\n", failure->file, failure->line, hook_name, Clut.runner.failure_message.items + failure->message_offset);
  }
  clut_reset_current_test();
}

static void clut_execute_test(ClutTestFn run_test_fn, const char *test_name) {
  clut_reset_current_test();
  const ClutReporter *reporter = clut_get_reporter();

  if (reporter->on_test_start) {
    reporter->on_test_start(reporter->context, test_name);
  }

  clut_run_hook(Clut.hooks.before_each);

  Clut.current_test.name = test_name;
  Clut.current_test.start_time = clock();

  if (!Clut.current_test.failed && !Clut.current_test.skipped) {
    run_test_fn();
  }

  double duration = clut_elapsed_seconds(Clut.current_test.start_time);

  clut_run_after_each();

  clut_da_foreach(ClutTestFailure, f, &Clut.runner.failures) { f->message = Clut.runner.failure_message.items + f->message_offset; }
  ClutTestResult result = {test_name, Clut.runner.failures.items, Clut.runner.failures.count, duration};

  if (Clut.current_test.failed) {
    Clut.suite.failed++;
    if (reporter->on_test_fail) {
      reporter->on_test_fail(reporter->context, &result);
    }
  } else if (Clut.current_test.skipped) {
    Clut.suite.skipped++;
    if (reporter->on_test_skip) {
      reporter->on_test_skip(reporter->context, &result);
    }
  } else {
    Clut.suite.passed++;
    if (reporter->on_test_pass) {
      reporter->on_test_pass(reporter->context, &result);
    }
  }
}

CLUT_API void ClutSuiteEnd(void) {
  if (Clut.suite.total_tests == 0) {
    return;
  }

  Clut.suite.start_time = clock();

  clut_reset_current_test();
  clut_run_hook(Clut.hooks.before_all);
  clut_print_hook_failure("before_all");

  clut_da_foreach(ClutRegisteredTest, test, &Clut.suite.registered_tests) { clut_execute_test(test->fn, test->name); }

  double total_time = clut_elapsed_seconds(Clut.suite.start_time);

  clut_reset_current_test();
  clut_run_hook(Clut.hooks.after_all);
  clut_print_hook_failure("after_all");

  const ClutSummary suite_summary = {Clut.suite.total_tests, Clut.suite.passed, Clut.suite.failed, Clut.suite.skipped, total_time};
  const ClutReporter *reporter = clut_get_reporter();

  if (reporter->on_suite_end) {
    reporter->on_suite_end(reporter->context, &suite_summary);
  }

  Clut.runner.summary.passed += Clut.suite.passed;
  Clut.runner.summary.failed += Clut.suite.failed;
  Clut.runner.summary.skipped += Clut.suite.skipped;
  Clut.runner.summary.total_tests += Clut.suite.total_tests;
  Clut.runner.summary.total_seconds += total_time;
  clut_reset_suite();
}

CLUT_API void ClutSetHookBeforeAll(ClutHookFn hook_fn) { Clut.hooks.before_all = hook_fn; }
CLUT_API void ClutSetHookBeforeEach(ClutHookFn hook_fn) { Clut.hooks.before_each = hook_fn; }
CLUT_API void ClutSetHookAfterAll(ClutHookFn hook_fn) { Clut.hooks.after_all = hook_fn; }
CLUT_API void ClutSetHookAfterEach(ClutHookFn hook_fn) { Clut.hooks.after_each = hook_fn; }

CLUT_API void ClutRunTest(ClutTestFn test_fn) { CLUT_RUN_GUARDED(test_fn()); }

static bool clut_tally_iteration(volatile size_t *failed, volatile size_t *skipped) {
  bool iteration_failed = Clut.current_test.failed;

  if (iteration_failed)
    (*failed)++;
  else if (Clut.current_test.skipped)
    (*skipped)++;

  Clut.current_test.failed = false;
  Clut.current_test.skipped = false;
  return iteration_failed;
}

static void clut_set_test_outcome(size_t failed, size_t skipped, size_t ran) {
  Clut.current_test.failed = failed > 0;
  Clut.current_test.skipped = failed == 0 && ran > 0 && skipped == ran;
}

CLUT_API void ClutRunRepeated(ClutRepeatedTestFn test_fn, size_t repetitions) {
  volatile ClutRepetitionInfo input = (ClutRepetitionInfo){.total_repetitions = repetitions};
  volatile size_t failed = 0;
  volatile size_t skipped = 0;
  for (volatile size_t i = 1; i <= repetitions; ++i) {
    Clut.current_test.iteration = (int)i;
    input.current_repetition = i;
    CLUT_RUN_GUARDED(test_fn(input));
    clut_tally_iteration(&failed, &skipped);
  }
  clut_set_test_outcome(failed, skipped, repetitions);
}

CLUT_API void ClutRunRepeatedWithThreshold(ClutRepeatedTestFn test_fn, size_t repetitions, size_t failure_threshold) {
  volatile ClutRepetitionInfo input = (ClutRepetitionInfo){.total_repetitions = repetitions};
  volatile size_t failed = 0;
  volatile size_t skipped = 0;
  volatile size_t ran = 0;
  for (volatile size_t i = 1; i <= repetitions; ++i) {
    Clut.current_test.iteration = (int)i;
    input.current_repetition = i;
    CLUT_RUN_GUARDED(test_fn(input));
    ran++;
    if (clut_tally_iteration(&failed, &skipped) && failed >= failure_threshold) {
      break;
    }
  }
  size_t counted_failures = failed >= failure_threshold ? failed : 0;
  clut_set_test_outcome(counted_failures, skipped, ran);
}

CLUT_API void ClutRunParamTestImpl(ClutParamTestFn test_fn, const void *items, size_t item_size, size_t count) {
  const unsigned char *base = (const unsigned char *)items;
  volatile size_t failed = 0;
  volatile size_t skipped = 0;
  for (volatile size_t i = 0; i < count; ++i) {
    Clut.current_test.iteration = (int)i;
    CLUT_RUN_GUARDED(test_fn(base + i * item_size));
    clut_tally_iteration(&failed, &skipped);
  }
  clut_set_test_outcome(failed, skipped, count);
}

CLUT_API void ClutSkipTest() {
  Clut.current_test.skipped = true;
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
}

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
  clut_message_append_user_text(msg);
  clut_message_append(CLUT_MSG_EXPECTED);
  clut_message_append_str(actual);
  clut_message_append(CLUT_MSG_EQUAL);
  clut_message_append_str(expected);
  clut_message_append(CLUT_MSG_UNTIL_LEN);
  clut_message_append_uint(len);
  clut_register_failure(file, line);
}

CLUT_API void ClutAssertEqualPtr(const void *expected, const void *actual, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  if (expected == actual)
    return;
  clut_message_append_user_text(msg);
  CLUT_FORMAT_COMPARISON_MESSAGE(clut_message_append_ptr, expected, CLUT_MSG_RECEIVED, actual);
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
  clut_message_append_user_text(msg);
  clut_message_append(CLUT_MSG_EXPECTED);
  clut_message_append_str(actual);
  clut_message_append(CLUT_MSG_NOT_EQUAL);
  clut_message_append_str(expected);
  clut_message_append(CLUT_MSG_UNTIL_LEN);
  clut_message_append_uint(len);
  clut_register_failure(file, line);
}

CLUT_API void ClutAssertNotEqualPtr(const void *expected, const void *actual, const char *file, int line, const char *msg) {
  CLUT_ABORT_IF_FAILED_OR_SKIPPED;
  if (expected != actual)
    return;
  clut_message_append_user_text(msg);
  CLUT_FORMAT_COMPARISON_MESSAGE(clut_message_append_ptr, actual, CLUT_MSG_NOT_EQUAL, expected);
  clut_register_failure(file, line);
}

CLUT_API void ClutAssertGreaterThanChar(char threshold, char actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_char, actual > threshold, actual, CLUT_MSG_GREATER_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertGreaterOrEqualChar(char threshold, char actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_char, actual >= threshold, actual, CLUT_MSG_GREATER_OR_EQUAL, threshold, file, line, msg); }
CLUT_API void ClutAssertLessThanChar(char threshold, char actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_char, actual < threshold, actual, CLUT_MSG_LESS_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertLessOrEqualChar(char threshold, char actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_char, actual <= threshold, actual, CLUT_MSG_LESS_OR_EQUAL, threshold, file, line, msg); }

CLUT_API void ClutAssertGreaterThanInt(int threshold, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_int, actual > threshold, actual, CLUT_MSG_GREATER_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertGreaterOrEqualInt(int threshold, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_int, actual >= threshold, actual, CLUT_MSG_GREATER_OR_EQUAL, threshold, file, line, msg); }
CLUT_API void ClutAssertLessThanInt(int threshold, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_int, actual < threshold, actual, CLUT_MSG_LESS_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertLessOrEqualInt(int threshold, int actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_int, actual <= threshold, actual, CLUT_MSG_LESS_OR_EQUAL, threshold, file, line, msg); }

CLUT_API void ClutAssertGreaterThanUint(size_t threshold, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_uint, actual > threshold, actual, CLUT_MSG_GREATER_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertGreaterOrEqualUint(size_t threshold, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_uint, actual >= threshold, actual, CLUT_MSG_GREATER_OR_EQUAL, threshold, file, line, msg); }
CLUT_API void ClutAssertLessThanUint(size_t threshold, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_uint, actual < threshold, actual, CLUT_MSG_LESS_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertLessOrEqualUint(size_t threshold, size_t actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_uint, actual <= threshold, actual, CLUT_MSG_LESS_OR_EQUAL, threshold, file, line, msg); }

CLUT_API void ClutAssertGreaterThanFloat(float threshold, float actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_float, actual > threshold, actual, CLUT_MSG_GREATER_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertGreaterOrEqualFloat(float threshold, float actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_message_append_float, actual > threshold || clut_floats_are_close(threshold, actual), actual, CLUT_MSG_GREATER_OR_EQUAL, threshold, file, line, msg);
}
CLUT_API void ClutAssertLessThanFloat(float threshold, float actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_float, actual < threshold, actual, CLUT_MSG_LESS_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertLessOrEqualFloat(float threshold, float actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_message_append_float, actual < threshold || clut_floats_are_close(threshold, actual), actual, CLUT_MSG_LESS_OR_EQUAL, threshold, file, line, msg);
}

CLUT_API void ClutAssertGreaterThanDouble(double threshold, double actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_double, actual > threshold, actual, CLUT_MSG_GREATER_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertGreaterOrEqualDouble(double threshold, double actual, const char *file, int line, const char *msg) {
  CLUT_ASSERT_COMPARE(clut_message_append_double, actual > threshold || clut_doubles_are_close(threshold, actual), actual, CLUT_MSG_GREATER_OR_EQUAL, threshold, file, line, msg);
}
CLUT_API void ClutAssertLessThanDouble(double threshold, double actual, const char *file, int line, const char *msg) { CLUT_ASSERT_COMPARE(clut_message_append_double, actual < threshold, actual, CLUT_MSG_LESS_THAN, threshold, file, line, msg); }
CLUT_API void ClutAssertLessOrEqualDouble(double threshold, double actual, const char *file, int line, const char *msg) {
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

      clut_message_append_user_text(msg);
      if (num_elements > 1) {
        clut_message_append(CLUT_MSG_ARRAY_MISMATCH_AT);
        clut_message_append_format("[%zu]", element_index);
        clut_message_append(CLUT_MSG_BYTE_OFFSET);
        clut_message_append_format("[%zu]: ", byte_index);
      } else {
        clut_message_append(CLUT_MSG_MISMATCH_AT);
        clut_message_append_format("%zu: ", byte_index);
      }
      CLUT_FORMAT_COMPARISON_MESSAGE(clut_message_append_hex, expected_bytes[offset], CLUT_MSG_WAS, actual_bytes[offset]);
      clut_register_failure(file, line);
      return;
    }
  }
}

#define CLUT_ARRAY_MISMATCH_RECORD(file, line, msg, index, append_fn, exp_val, act_val)                                                                                                                                                                            \
  do {                                                                                                                                                                                                                                                             \
    clut_message_append_user_text(msg);                                                                                                                                                                                                                            \
    clut_message_append_array_prefix((index));                                                                                                                                                                                                                     \
    CLUT_FORMAT_COMPARISON_MESSAGE(append_fn, (exp_val), CLUT_MSG_WAS, (act_val));                                                                                                                                                                                 \
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
      is_equal = true;
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
    clut_message_append_user_text(msg);                                                                                                                                                                                                                            \
    clut_message_append_array_prefix((index));                                                                                                                                                                                                                     \
    CLUT_FORMAT_WITHIN_MESSAGE(append_fn_val, append_fn_num, (exp_val), (delta_val), (diff_val));                                                                                                                                                                  \
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
