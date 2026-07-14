#ifndef CLUT_TEST_COMMON_H
#define CLUT_TEST_COMMON_H

#define CLUT_IMPLEMENTATION
#include "../../clut.h"

ClutSB test_message;
ClutSB output;

BEFORE_ALL_HOOK(setup) {
  test_message = (ClutSB){0};
  output = (ClutSB){0};
}

AFTER_EACH_HOOK(dispatch_fail_flush) {
  if (Clut.current.failed) {
    clut_dispatch_fail_flush(&output);
    clut_da_clear(&output);
  }
}

AFTER_ALL_HOOK(teardown) {
  clut_da_free(&output);
  clut_da_free(&test_message);
}

static void record_failure(const char *file, int line) {
  ClutLogRecord record;
  clut_log_record_capture(&record, file, line);
  clut_dispatch_fail_append(&record, &output, &test_message);
  clut_da_clear(&test_message);
}

static bool clut_validate_pass(bool failed, const char *expr, const char *file, int line) {
  if (failed) {
    const char *clut_msg = clut_sb_cstr(&Clut.runner.test_message);
    clut_sb_append(&test_message, "FALSE-POSITIVE (expected PASS, got FAIL)\n  expr: ");
    clut_sb_append(&test_message, expr);
    clut_sb_append(&test_message, "\n  msg : ");
    clut_sb_append(&test_message, clut_msg);
    record_failure(file, line);
    clut_da_clear(&Clut.runner.output);
    clut_da_clear(&Clut.runner.test_message);
    return true;
  }
  return false;
}

static bool clut_validate_fail(bool failed, const char *expr, const char *expected_msg, const char *file, int line) {
  if (!failed) {
    clut_sb_append(&test_message, "FALSE-NEGATIVE (expected FAIL, got PASS)\n  expr: ");
    clut_sb_append(&test_message, expr);
    record_failure(file, line);
    return true;
  }
  if (expected_msg != NULL) {
    const char *actual_msg = clut_sb_cstr(&Clut.runner.test_message);
    if (strcmp(actual_msg, expected_msg) != 0) {
      clut_sb_append(&test_message, "WRONG MESSAGE\n  expr    : ");
      clut_sb_append(&test_message, expr);
      clut_sb_append(&test_message, "\n  expected: ");
      clut_sb_append(&test_message, expected_msg);
      clut_sb_append(&test_message, "\n  actual  : ");
      clut_sb_append(&test_message, actual_msg);
      record_failure(file, line);
      clut_da_clear(&Clut.runner.output);
      clut_da_clear(&Clut.runner.test_message);
      return true;
    }
  }
  clut_da_clear(&Clut.runner.output);
  clut_da_clear(&Clut.runner.test_message);
  return false;
}

static inline bool clut_preserve_begin(void) {
  bool prev = Clut.current.failed;
  if (prev) {
    Clut.current.failed = false;
  }
  return prev;
}

static inline void clut_preserve_end(bool prev_failed, bool validation_failed) { Clut.current.failed = prev_failed || validation_failed; }

#define VALIDATE_PASS(assertion_expr)                                                                                                                                                                                                                              \
  do {                                                                                                                                                                                                                                                             \
    bool __prev_failed = clut_preserve_begin();                                                                                                                                                                                                                    \
    assertion_expr;                                                                                                                                                                                                                                                \
    bool __validation_failed = clut_validate_pass(Clut.current.failed, #assertion_expr, __FILE__, __LINE__);                                                                                                                                                       \
    clut_preserve_end(__prev_failed, __validation_failed);                                                                                                                                                                                                         \
  } while (0)

#define VALIDATE_FAIL(assertion_expr)                                                                                                                                                                                                                              \
  do {                                                                                                                                                                                                                                                             \
    bool __prev_failed = clut_preserve_begin();                                                                                                                                                                                                                    \
    assertion_expr;                                                                                                                                                                                                                                                \
    bool __validation_failed = clut_validate_fail(Clut.current.failed, #assertion_expr, NULL, __FILE__, __LINE__);                                                                                                                                                 \
    clut_preserve_end(__prev_failed, __validation_failed);                                                                                                                                                                                                         \
  } while (0)

#define VALIDATE_FAIL_MSG(assertion_expr, expected_msg)                                                                                                                                                                                                            \
  do {                                                                                                                                                                                                                                                             \
    bool __prev_failed = clut_preserve_begin();                                                                                                                                                                                                                    \
    assertion_expr;                                                                                                                                                                                                                                                \
    bool __validation_failed = clut_validate_fail(Clut.current.failed, #assertion_expr, (expected_msg), __FILE__, __LINE__);                                                                                                                                       \
    clut_preserve_end(__prev_failed, __validation_failed);                                                                                                                                                                                                         \
  } while (0)

#define VALIDATE_CUSTOM_MESSAGE(assertion_expr, expected_msg)                                                                                                                                                                                                      \
  do {                                                                                                                                                                                                                                                             \
    bool __prev_failed = clut_preserve_begin();                                                                                                                                                                                                                    \
    assertion_expr;                                                                                                                                                                                                                                                \
    bool __validation_failed = clut_validate_fail(Clut.current.failed, #assertion_expr, (expected_msg), __FILE__, __LINE__);                                                                                                                                       \
    clut_preserve_end(__prev_failed, __validation_failed);                                                                                                                                                                                                         \
  } while (0)

#endif /* CLUT_TEST_COMMON_H */
