#ifndef RUNNER_GENERATOR_TEST_STRING_LIST_H
#define RUNNER_GENERATOR_TEST_STRING_LIST_H
#include "test_common.h"

TEST(str_dup_copies_content) {
  char *copy = str_dup("hello");
  TEST_ASSERT_EQUAL_STRING("hello", copy);
  free(copy);
}

TEST(str_dup_independent_buffer) {
  char original[] = "hello";
  char *copy = str_dup(original);
  original[0] = 'x';
  TEST_ASSERT_EQUAL_STRING("hello", copy);
  free(copy);
}

TEST(string_list_append_single) {
  StringList list = {0};
  string_list_append(&list, "foo.h");
  TEST_ASSERT_EQUAL_INT(1, (int)list.count);
  TEST_ASSERT_EQUAL_STRING("foo.h", list.items[0]);
  string_list_free(&list);
}

TEST(string_list_append_many_preserves_order) {
  StringList list = {0};
  string_list_append(&list, "a.h");
  string_list_append(&list, "b.h");
  string_list_append(&list, "c.h");
  TEST_ASSERT_EQUAL_INT(3, (int)list.count);
  TEST_ASSERT_EQUAL_STRING("a.h", list.items[0]);
  TEST_ASSERT_EQUAL_STRING("b.h", list.items[1]);
  TEST_ASSERT_EQUAL_STRING("c.h", list.items[2]);
  string_list_free(&list);
}

TEST(string_list_append_grows_past_init_cap) {
  StringList list = {0};
  char name[32];
  int total = DA_INIT_CAP + 10;
  for (int i = 0; i < total; ++i) {
    snprintf(name, sizeof(name), "file_%d.h", i);
    string_list_append(&list, name);
  }
  TEST_ASSERT_EQUAL_INT(total, (int)list.count);
  TEST_ASSERT_EQUAL_STRING("file_0.h", list.items[0]);
  snprintf(name, sizeof(name), "file_%d.h", total - 1);
  TEST_ASSERT_EQUAL_STRING(name, list.items[list.count - 1]);
  string_list_free(&list);
}

TEST(string_list_free_resets_state) {
  StringList list = {0};
  string_list_append(&list, "a.h");
  string_list_free(&list);
  TEST_ASSERT_TRUE(list.items == NULL);
  TEST_ASSERT_EQUAL_INT(0, (int)list.count);
  TEST_ASSERT_EQUAL_INT(0, (int)list.capacity);
}

TEST(string_list_free_empty_list_is_safe) {
  StringList list = {0};
  string_list_free(&list);
  TEST_ASSERT_TRUE(list.items == NULL);
}
#endif
