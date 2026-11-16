#ifndef CLUT_TEST_COMMON_H
#define CLUT_TEST_COMMON_H

#define CLUT_IMPLEMENTATION
#include "../../clut.h"

static ClutStringBuilder captured_message;

HOOK_AFTER_ALL(test_common) { clut_da_free(&captured_message); }

static const char *clut_string_builder_to_cstr(ClutStringBuilder *sb) {
  if (!sb->items)
    return "";
  clut_da_reserve(sb, sb->count + 1);
  sb->items[sb->count] = '\0';
  return sb->items;
}

static bool clut_take_assert_failure(void) {
  clut_da_clear(&captured_message);
  if (!Clut.current_test.failed || Clut.runner.failures.count == 0) {
    return false;
  }

  ClutTestFailure *last = &Clut.runner.failures.items[Clut.runner.failures.count - 1];
  clut_string_builder_append(&captured_message, Clut.runner.failure_message.items + last->message_offset);

  Clut.runner.failure_message.count = last->message_offset;
  Clut.runner.next_message_offset = last->message_offset;
  Clut.runner.failures.count--;
  Clut.current_test.failed = false;
  return true;
}

static void clut_validate_pass(bool assert_failed, const char *expr, const char *file, int line) {
  if (!assert_failed) {
    return;
  }
  clut_message_append("FALSE-POSITIVE (expected PASS, got FAIL)\n  expr: ");
  clut_message_append(expr);
  clut_message_append("\n  msg : ");
  clut_message_append(clut_string_builder_to_cstr(&captured_message));
  clut_register_failure(file, line);
}

static void clut_validate_fail(bool assert_failed, const char *expr, const char *expected_msg, const char *file, int line) {
  if (!assert_failed) {
    clut_message_append("FALSE-NEGATIVE (expected FAIL, got PASS)\n  expr: ");
    clut_message_append(expr);
    clut_register_failure(file, line);
    return;
  }
  if (expected_msg == NULL) {
    return;
  }

  const char *actual_msg = clut_string_builder_to_cstr(&captured_message);
  if (strcmp(actual_msg, expected_msg) == 0) {
    return;
  }
  clut_message_append("WRONG MESSAGE\n  expr    : ");
  clut_message_append(expr);
  clut_message_append("\n  expected: ");
  clut_message_append(expected_msg);
  clut_message_append("\n  actual  : ");
  clut_message_append(actual_msg);
  clut_register_failure(file, line);
}

#define CLUT_VALIDATE(assertion_expr, validate_call)                                                                                                                                                                                                               \
  do {                                                                                                                                                                                                                                                             \
    bool clut_prev_failed = Clut.current_test.failed;                                                                                                                                                                                                              \
    Clut.current_test.failed = false;                                                                                                                                                                                                                              \
    assertion_expr;                                                                                                                                                                                                                                                \
    bool clut_assert_failed = clut_take_assert_failure();                                                                                                                                                                                                          \
    validate_call;                                                                                                                                                                                                                                                 \
    if (clut_prev_failed) {                                                                                                                                                                                                                                        \
      Clut.current_test.failed = true;                                                                                                                                                                                                                             \
    }                                                                                                                                                                                                                                                              \
  } while (0)

#define VALIDATE_PASS(assertion_expr) CLUT_VALIDATE(assertion_expr, clut_validate_pass(clut_assert_failed, #assertion_expr, __FILE__, __LINE__))
#define VALIDATE_FAIL(assertion_expr) CLUT_VALIDATE(assertion_expr, clut_validate_fail(clut_assert_failed, #assertion_expr, NULL, __FILE__, __LINE__))
#define VALIDATE_FAIL_MSG(assertion_expr, expected_msg) CLUT_VALIDATE(assertion_expr, clut_validate_fail(clut_assert_failed, #assertion_expr, (expected_msg), __FILE__, __LINE__))
#define VALIDATE_CUSTOM_MESSAGE(assertion_expr, expected_msg) VALIDATE_FAIL_MSG(assertion_expr, expected_msg)

#endif /* CLUT_TEST_COMMON_H */
