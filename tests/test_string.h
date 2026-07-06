#ifndef CLUT_TEST_STRING_H
#define CLUT_TEST_STRING_H

#include "test_common.h"

TEST(test_strings) {
  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING("clut", "clut"));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING("clut", "different"), "Expected \"different\" to be equal to \"clut\"");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING("clut", NULL), "Expected (null) to be equal to \"clut\"");

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_STRING("clut", "different"));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_EQUAL_STRING("clut", "clut"), "Expected \"clut\" to not be equal to \"clut\"");

  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_LEN("clut_framework", "clut_testing", 5));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_LEN("clut_framework", "clut_testing", 10), "Expected \"clut_testing\" to be equal to \"clut_framework\" until length 10");

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_STRING_LEN("clut_framework", "clut_testing", 10));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_EQUAL_STRING_LEN("clut_framework", "clut_testing", 5), "Expected \"clut_testing\" to not be equal to \"clut_framework\" until length 5");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_STRING_MESSAGE("clut", "different", "custom eq string"), "custom eq string");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_EQUAL_STRING_MESSAGE("clut", "clut", "custom neq string"), "custom neq string");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE("clut_framework", "clut_testing", 10, "custom eq string len"), "custom eq string len");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_EQUAL_STRING_LEN_MESSAGE("clut_framework", "clut_testing", 5, "custom neq string len"), "custom neq string len");
}

#endif
