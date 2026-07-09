#ifndef CLUT_TEST_BASIC_H
#define CLUT_TEST_BASIC_H

#include "test_common.h"

TEST(test_boolean_and_basic) {
  int val = 1;
  VALIDATE_PASS(TEST_ASSERT(val == 1));
  VALIDATE_FAIL_MSG(TEST_ASSERT(val == 0), "Expression Is False -> val == 0");

  VALIDATE_PASS(TEST_ASSERT_TRUE(true));
  VALIDATE_FAIL_MSG(TEST_ASSERT_TRUE(false), "Expected True But Was False -> false");

  VALIDATE_PASS(TEST_ASSERT_FALSE(false));
  VALIDATE_FAIL_MSG(TEST_ASSERT_FALSE(true), "Expected False But Was True -> true");

  VALIDATE_PASS(TEST_ASSERT_UNLESS(false));
  VALIDATE_FAIL_MSG(TEST_ASSERT_UNLESS(true), "Expression Is True -> true");

  VALIDATE_FAIL_MSG(TEST_FAIL("Test Failed"), "Test Failed");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_MESSAGE(false, "custom assert"), "custom assert");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_TRUE_MESSAGE(false, "custom true"), "custom true");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_FALSE_MESSAGE(true, "custom false"), "custom false");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_UNLESS_MESSAGE(true, "custom unless"), "custom unless");
}

TEST(test_pointers_and_nullness) {
  int dummy = 42;
  int *ptr = &dummy;

  VALIDATE_PASS(TEST_ASSERT_NULL(NULL));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NULL(ptr), "Expected NULL");

  VALIDATE_PASS(TEST_ASSERT_NOT_NULL(ptr));
  VALIDATE_FAIL_MSG(TEST_ASSERT_NOT_NULL(NULL), "Expected Non-NULL");

  VALIDATE_PASS(TEST_ASSERT_EQUAL_PTR(ptr, ptr));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_PTR(ptr, NULL));

  VALIDATE_PASS(TEST_ASSERT_NOT_EQUAL_PTR(ptr, NULL));
  VALIDATE_FAIL(TEST_ASSERT_NOT_EQUAL_PTR(ptr, ptr));

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NULL_MESSAGE(ptr, "custom null"), "custom null");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_NULL_MESSAGE(NULL, "custom not null"), "custom not null");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_NOT_EQUAL_PTR_MESSAGE(ptr, ptr, "custom ptr"), "custom ptr");
}

TEST(test_memory) {
  typedef struct {
    int x;
    int y;
    char c;
  } DummyStruct;

  DummyStruct s1 = {0}, s2 = {0}, s3 = {0}, s4 = {0};
  s1 = (DummyStruct){10, 20, 'A'};
  s2 = (DummyStruct){10, 20, 'A'};
  s3 = (DummyStruct){10, 99, 'A'};
  s4 = (DummyStruct){10, 20, 'Z'};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(&s1, &s2, sizeof(DummyStruct)));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(&s1, &s1, sizeof(DummyStruct)));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_MEMORY(&s1, &s3, sizeof(DummyStruct)));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_MEMORY(&s1, &s4, sizeof(DummyStruct)));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(NULL, NULL, 0));

  DummyStruct buf1[3], buf2[3], buf3[3];
  memset(buf1, 0, sizeof(buf1));
  memset(buf2, 0, sizeof(buf2));
  memset(buf3, 0, sizeof(buf3));

  buf1[0] = (DummyStruct){10, 20, 'A'};
  buf1[1] = (DummyStruct){30, 40, 'B'};
  buf1[2] = (DummyStruct){50, 60, 'C'};

  buf2[0] = (DummyStruct){10, 20, 'A'};
  buf2[1] = (DummyStruct){30, 40, 'B'};
  buf2[2] = (DummyStruct){50, 60, 'C'};

  buf3[0] = (DummyStruct){10, 20, 'A'};
  buf3[1] = (DummyStruct){30, 99, 'B'};
  buf3[2] = (DummyStruct){50, 60, 'C'};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(buf1, buf2, sizeof(buf1)));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_MEMORY(buf1, buf3, sizeof(buf1)));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(buf1, buf3, sizeof(DummyStruct)));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(&buf1[1], &buf2[1], sizeof(DummyStruct)));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(NULL, NULL, 0));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_MEMORY(buf1, NULL, sizeof(buf1)), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_MEMORY(NULL, buf2, sizeof(buf2)), "One of the pointers is NULL");
  VALIDATE_PASS(TEST_ASSERT_EQUAL_MEMORY(&s1, &s2, 0));

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_MEMORY_MESSAGE(buf1, NULL, sizeof(buf1), "custom memory"), "custom memory");
  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_MEMORY_ARRAY_MESSAGE(buf1, buf3, sizeof(DummyStruct), 3, "custom memory array"), "custom memory array");
}

#endif
