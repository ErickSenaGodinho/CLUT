#ifndef CLUT_TEST_ARRAYS_H
#define CLUT_TEST_ARRAYS_H

#include "test_common.h"

TEST(char_array) {
  char exp[] = "ClutTest";
  char act_ok[] = "ClutTest";
  char act_diff_end[] = "ClutTesX";
  char act_diff_start[] = "XlutTest";
  char act_diff_mid[] = "ClutXest";

  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_ok, 8));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, exp, 8));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_end, 7));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_mid, 4));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_start, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(NULL, NULL, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_end, 0));

  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_end, 8), "Array mismatch at Element [7] Expected 't' (0x74) but was 'X' (0x58)");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_start, 8), "Array mismatch at Element [0] Expected 'C' (0x43) but was 'X' (0x58)");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, act_diff_mid, 8), "Array mismatch at Element [4] Expected 'T' (0x54) but was 'X' (0x58)");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_CHAR_ARRAY(exp, NULL, 8), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_CHAR_ARRAY(NULL, act_ok, 8), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE(exp, act_diff_end, 8, "custom char array"), "custom char array");
}

TEST(int_array) {
  int arr1[] = {-10, 20, -30, 40, -50};
  int arr2[] = {-10, 20, -30, 40, -50};
  int arr3[] = {-10, -99, -30, 40, -50};
  int arr4[] = {-10, 20, -30, 40, 99};
  int arr5[] = {99, 20, -30, 40, -50};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr2, 5));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr1, 5));

  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr3, 5), "Array mismatch at Element [1] Expected 20 but was -99");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr4, 5), "Array mismatch at Element [4] Expected -50 but was 99");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr5, 5), "Array mismatch at Element [0] Expected -10 but was 99");

  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr3, 1));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr4, 4));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(&arr1[2], &arr2[2], 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_INT_ARRAY(&arr1[1], &arr3[1], 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, arr3, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_INT_ARRAY(NULL, NULL, 0));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_INT_ARRAY(arr1, NULL, 5), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_INT_ARRAY(NULL, arr2, 5), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(arr1, arr3, 5, "custom int array"), "custom int array");
}

TEST(uint_array) {
  size_t arr1[] = {10, 20, 30, 40, 50};
  size_t arr2[] = {10, 20, 30, 40, 50};
  size_t arr3[] = {10, 99, 30, 40, 50};
  size_t arr4[] = {10, 20, 30, 40, 99};
  size_t arr5[] = {99, 20, 30, 40, 50};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr2, 5));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr1, 5));

  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr3, 5), "Array mismatch at Element [1] Expected 20 but was 99");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr4, 5), "Array mismatch at Element [4] Expected 50 but was 99");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr5, 5), "Array mismatch at Element [0] Expected 10 but was 99");

  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr3, 1));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr4, 4));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(&arr1[2], &arr2[2], 3));
  VALIDATE_FAIL(TEST_ASSERT_EQUAL_UINT_ARRAY(&arr1[1], &arr3[1], 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, arr3, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_UINT_ARRAY(NULL, NULL, 0));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_UINT_ARRAY(arr1, NULL, 5), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_UINT_ARRAY(NULL, arr2, 5), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_UINT_ARRAY_MESSAGE(arr1, arr3, 5, "custom uint array"), "custom uint array");
}

TEST(float_array) {
  float sum = 0.0f;
  for (int i = 0; i < 10; i++)
    sum += 0.1f;

  float exp[] = {1.0f, 5.5f, 10.123f};
  float act_ok[] = {1.0f, 5.5f, 10.123f};
  float act_accumulated[] = {sum, 5.5f, 10.123f};
  float act_diff_start[] = {1.1f, 5.5f, 10.123f};
  float act_diff_mid[] = {1.0f, 5.501f, 10.123f};
  float act_diff_end[] = {1.0f, 5.5f, 10.124f};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_accumulated, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_mid, 1));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_end, 2));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(NULL, NULL, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_start, 0));

  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_start, 3), "Array mismatch at Element [0] Expected 1.00000 but was 1.10000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_mid, 3), "Array mismatch at Element [1] Expected 5.50000 but was 5.50100");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, act_diff_end, 3), "Array mismatch at Element [2] Expected 10.12300 but was 10.12400");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_FLOAT_ARRAY(exp, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_FLOAT_ARRAY(NULL, act_ok, 3), "One of the pointers is NULL");

  float zero_exp[] = {0.0f, 0.0f};
  float zero_act[] = {0.0f, 0.0f};
  VALIDATE_PASS(TEST_ASSERT_EQUAL_FLOAT_ARRAY(zero_exp, zero_act, 2));

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(exp, act_diff_start, 3, "custom float array"), "custom float array");
}

TEST(double_array) {
  double sum = 0.0;
  for (int i = 0; i < 10000; i++)
    sum += 0.1;

  double exp[] = {1000.0, 0.123456789, 99.99};
  double act_ok[] = {1000.0, 0.123456789, 99.99};
  double act_accumulated[] = {sum, 0.123456789, 99.99};
  double act_diff_start[] = {1000.00001, 0.123456789, 99.99};
  double act_diff_mid[] = {1000.0, 0.123456780, 99.99};
  double act_diff_end[] = {1000.0, 0.123456789, 100.0};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_accumulated, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_mid, 1));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_end, 2));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(NULL, NULL, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_start, 0));

  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_start, 3), "Array mismatch at Element [0] Expected 1000.000000000 but was 1000.000010000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_mid, 3), "Array mismatch at Element [1] Expected 0.123456789 but was 0.123456780");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, act_diff_end, 3), "Array mismatch at Element [2] Expected 99.990000000 but was 100.000000000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(exp, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(NULL, act_ok, 3), "One of the pointers is NULL");

  double zero_exp[] = {0.0, 0.0};
  double zero_act[] = {0.0, 0.0};
  VALIDATE_PASS(TEST_ASSERT_EQUAL_DOUBLE_ARRAY(zero_exp, zero_act, 2));

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_DOUBLE_ARRAY_MESSAGE(exp, act_diff_start, 3, "custom double array"), "custom double array");
}

TEST(string_array) {
  const char *exp[] = {"clut", "test", "framework"};
  const char *act_ok[] = {"clut", "test", "framework"};
  const char *act_diff_start[] = {"XXXX", "test", "framework"};
  const char *act_diff_mid[] = {"clut", "XXXX", "framework"};
  const char *act_diff_end[] = {"clut", "test", "XXXXXXXXX"};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_mid, 1));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_end, 2));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_start, 0));
  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(NULL, NULL, 0));

  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_start, 3), "Array mismatch at Element [0] Expected \"clut\" but was \"XXXX\"");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_mid, 3), "Array mismatch at Element [1] Expected \"test\" but was \"XXXX\"");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, act_diff_end, 3), "Array mismatch at Element [2] Expected \"framework\" but was \"XXXXXXXXX\"");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(exp, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(NULL, act_ok, 3), "One of the pointers is NULL");

  const char *exp_null[] = {"clut", NULL, "framework"};
  const char *act_null[] = {"clut", NULL, "framework"};
  const char *act_no_null[] = {"clut", "test", "framework"};

  VALIDATE_PASS(TEST_ASSERT_EQUAL_STRING_ARRAY(exp_null, act_null, 3));
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(exp_null, act_no_null, 3), "Array mismatch at Element [1] Expected (null) but was \"test\"");
  VALIDATE_FAIL_MSG(TEST_ASSERT_EQUAL_STRING_ARRAY(act_no_null, exp_null, 3), "Array mismatch at Element [1] Expected \"test\" but was (null)");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_EQUAL_STRING_ARRAY_MESSAGE(exp, act_diff_start, 3, "custom string array"), "custom string array");
}

TEST(within_char_array) {
  char exp[] = {'2', 'd', 20};
  char act_ok[] = {'4', 'a', 23};
  char act_fail[] = {'2', 'd', 26};

  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 5, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 5, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 5, act_fail, 2));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 0, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_CHAR_ARRAY(NULL, 5, NULL, 0));

  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 5, act_fail, 3), "Array mismatch at Element [2] Expected '\\x14' (0x14) within +/- 5 but actual diff was 6");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 0, act_ok, 3), "Array mismatch at Element [0] Expected '2' (0x32) within +/- 0 but actual diff was 2");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_CHAR_ARRAY(exp, 5, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_CHAR_ARRAY(NULL, 5, act_ok, 3), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_CHAR_ARRAY_MESSAGE(exp, 5, act_fail, 3, "custom within char array"), "custom within char array");
}

TEST(within_int_array) {
  int exp[] = {100, -50, 0, 200};
  int act_ok[] = {105, -45, 10, 195};
  int act_fail[] = {105, -45, 11, 195};

  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 10, act_ok, 4));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 10, exp, 4));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 10, act_fail, 2));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 0, exp, 4));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_INT_ARRAY(NULL, 10, NULL, 0));

  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 10, act_fail, 4), "Array mismatch at Element [2] Expected 0 within +/- 10 but actual diff was 11");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 0, act_ok, 4), "Array mismatch at Element [0] Expected 100 within +/- 0 but actual diff was 5");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_INT_ARRAY(exp, 10, NULL, 4), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_INT_ARRAY(NULL, 10, act_ok, 4), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_INT_ARRAY_MESSAGE(exp, 10, act_fail, 4, "custom within int array"), "custom within int array");
}

TEST(within_uint_array) {
  size_t exp[] = {1000, 500, 250};
  size_t act_ok[] = {1040, 550, 250};
  size_t act_fail[] = {1040, 551, 250};

  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 50, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 50, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 50, act_fail, 1));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 0, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_UINT_ARRAY(NULL, 50, NULL, 0));

  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 50, act_fail, 3), "Array mismatch at Element [1] Expected 500 within +/- 50 but actual diff was 51");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 0, act_ok, 3), "Array mismatch at Element [0] Expected 1000 within +/- 0 but actual diff was 40");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_UINT_ARRAY(exp, 50, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_UINT_ARRAY(NULL, 50, act_ok, 3), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_UINT_ARRAY_MESSAGE(exp, 50, act_fail, 3, "custom within uint array"), "custom within uint array");
}

TEST(within_float_array) {
  float exp[] = {1.0f, 5.5f, 10.0f};
  float act_ok[] = {1.05f, 5.45f, 10.09f};
  float act_fail[] = {1.05f, 5.45f, 10.11f};

  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.1f, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.1f, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.1f, act_fail, 2));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.0f, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_FLOAT_ARRAY(NULL, 0.1f, NULL, 0));

  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.1f, act_fail, 3), "Array mismatch at Element [2] Expected 10.00000 within +/- 0.10000 but actual diff was 0.11000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.0f, act_ok, 3), "Array mismatch at Element [0] Expected 1.00000 within +/- 0.00000 but actual diff was 0.05000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_FLOAT_ARRAY(exp, 0.1f, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_FLOAT_ARRAY(NULL, 0.1f, act_ok, 3), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_FLOAT_ARRAY_MESSAGE(exp, 0.1f, act_fail, 3, "custom within float array"), "custom within float array");
}

TEST(within_double_array) {
  double exp[] = {1.0, 5.5, 10.0};
  double act_ok[] = {1.05, 5.45, 10.09};
  double act_fail[] = {1.05, 5.45, 10.11};

  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.1, act_ok, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.1, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.1, act_fail, 2));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.0, exp, 3));
  VALIDATE_PASS(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(NULL, 0.1, NULL, 0));

  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.1, act_fail, 3), "Array mismatch at Element [2] Expected 10.000000000 within +/- 0.100000000 but actual diff was 0.110000000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.0, act_ok, 3), "Array mismatch at Element [0] Expected 1.000000000 within +/- 0.000000000 but actual diff was 0.050000000");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(exp, 0.1, NULL, 3), "One of the pointers is NULL");
  VALIDATE_FAIL_MSG(TEST_ASSERT_WITHIN_DOUBLE_ARRAY(NULL, 0.1, act_ok, 3), "One of the pointers is NULL");

  VALIDATE_CUSTOM_MESSAGE(TEST_ASSERT_WITHIN_DOUBLE_ARRAY_MESSAGE(exp, 0.1, act_fail, 3, "custom within double array"), "custom within double array");
}

#endif
