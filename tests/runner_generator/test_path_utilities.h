#ifndef RUNNER_GENERATOR_TEST_PATH_UTILITIES_H
#define RUNNER_GENERATOR_TEST_PATH_UTILITIES_H

#include "test_common.h"

TEST(path_is_separator_slash) {
  TEST_ASSERT_TRUE(path_is_separator('/'));
  TEST_ASSERT_FALSE(path_is_separator('a'));
}

TEST(path_is_absolute_root) {
  TEST_ASSERT_TRUE(path_is_absolute("/a/b/c"));
  TEST_ASSERT_FALSE(path_is_absolute("a/b/c"));
  TEST_ASSERT_FALSE(path_is_absolute("./a/b"));
}

TEST(path_normalize_separators_backslashes) {
  char buf[] = "a\\b\\c";
  path_normalize_separators(buf);
  TEST_ASSERT_EQUAL_STRING("a/b/c", buf);
}

TEST(path_basename_nested) { TEST_ASSERT_EQUAL_STRING("c.txt", path_basename("/a/b/c.txt")); }

TEST(path_basename_no_separator) { TEST_ASSERT_EQUAL_STRING("file.txt", path_basename("file.txt")); }

TEST(path_basename_trailing_slash) { TEST_ASSERT_EQUAL_STRING("", path_basename("/a/b/")); }

TEST(path_basename_root_file) { TEST_ASSERT_EQUAL_STRING("file.txt", path_basename("/file.txt")); }

TEST(path_dirname_nested) {
  char out[CLUT_PATH_MAX];
  bool ok = path_dirname("/a/b/c.txt", out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("/a/b", out);
}

TEST(path_dirname_no_separator) {
  char out[CLUT_PATH_MAX];
  bool ok = path_dirname("file.txt", out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING(".", out);
}

TEST(path_dirname_root) {
  char out[CLUT_PATH_MAX];
  bool ok = path_dirname("/file.txt", out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("/", out);
}

TEST(path_collapse_dots_basic) {
  char out[CLUT_PATH_MAX];
  bool ok = path_collapse_dots("/a/./b/../c", out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("/a/c", out);
}

TEST(path_collapse_dots_past_root) {
  char out[CLUT_PATH_MAX];
  bool ok = path_collapse_dots("/../../a", out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("/a", out);
}

TEST(path_same_root_absolute) { TEST_ASSERT_TRUE(path_same_root("/a/b", "/c/d")); }

TEST(path_same_root_mixed) { TEST_ASSERT_FALSE(path_same_root("/a/b", "c/d")); }

TEST(path_common_boundary_shared_prefix) {
  size_t from_end, to_end;
  path_common_boundary("/a/b/c", "/a/b/d/e", &from_end, &to_end);
  TEST_ASSERT_EQUAL_INT(5, (int)from_end);
  TEST_ASSERT_EQUAL_INT(5, (int)to_end);
}

TEST(path_common_boundary_identical) {
  size_t from_end, to_end;
  path_common_boundary("/a/b/c", "/a/b/c", &from_end, &to_end);
  TEST_ASSERT_EQUAL_INT((int)strlen("/a/b/c"), (int)from_end);
  TEST_ASSERT_EQUAL_INT((int)strlen("/a/b/c"), (int)to_end);
}

TEST(path_count_remaining_segments_basic) {
  TEST_ASSERT_EQUAL_INT(0, (int)path_count_remaining_segments(""));
  TEST_ASSERT_EQUAL_INT(1, (int)path_count_remaining_segments("x"));
  TEST_ASSERT_EQUAL_INT(3, (int)path_count_remaining_segments("x/y/z"));
}

TEST(path_relative_nested) {
  char out[CLUT_PATH_MAX];
  bool ok = path_relative("/home/user/project/build", "/home/user/project/src/test.c", out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("../src/test.c", out);
}

TEST(path_relative_same_directory) {
  char out[CLUT_PATH_MAX];
  bool ok = path_relative("/a/b", "/a/b/file.c", out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("file.c", out);
}

TEST(path_relative_different_roots) {
  char out[CLUT_PATH_MAX];
  bool ok = path_relative("/a/b", "relative/path.c", out, sizeof(out));
  TEST_ASSERT_FALSE(ok);
}

#endif
