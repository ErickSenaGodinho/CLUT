#ifndef RUNNER_GENERATOR_TEST_WILDCARD_EXPANSION_H
#define RUNNER_GENERATOR_TEST_WILDCARD_EXPANSION_H
#include "test_common.h"

#ifdef _WIN32
#include <direct.h>
#define WILDCARD_FIXTURE_MKDIR(path) _mkdir(path)
#define WILDCARD_FIXTURE_RMDIR(path) _rmdir(path)
#else
#include <sys/stat.h>
#include <unistd.h>
#define WILDCARD_FIXTURE_MKDIR(path) mkdir(path, 0755)
#define WILDCARD_FIXTURE_RMDIR(path) rmdir(path)
#endif

#define WILDCARD_FIXTURE_DIR "clut_wildcard_fixture"

static bool string_list_contains(const StringList *list, const char *needle) {
  da_foreach(char *, it, list) {
    if (strcmp(*it, needle) == 0)
      return true;
  }
  return false;
}

static void wildcard_fixture_write_file(const char *name) {
  char path[CLUT_PATH_MAX];
  snprintf(path, sizeof(path), WILDCARD_FIXTURE_DIR "/%s", name);
  FILE *f = fopen(path, "wb");
  TEST_ASSERT_TRUE(f != NULL);
  fclose(f);
}

BEFORE_ALL_HOOK(wildcard_expansion_setup) {
  WILDCARD_FIXTURE_MKDIR(WILDCARD_FIXTURE_DIR);
  wildcard_fixture_write_file("a_test.h");
  wildcard_fixture_write_file("b_test.h");
  wildcard_fixture_write_file("notes.txt");
}

AFTER_ALL_HOOK(wildcard_expansion_teardown) {
  remove(WILDCARD_FIXTURE_DIR "/a_test.h");
  remove(WILDCARD_FIXTURE_DIR "/b_test.h");
  remove(WILDCARD_FIXTURE_DIR "/notes.txt");
  WILDCARD_FIXTURE_RMDIR(WILDCARD_FIXTURE_DIR);
}

TEST(list_directory_lists_all_entries) {
  StringList entries = {0};
  bool ok = list_directory(WILDCARD_FIXTURE_DIR, &entries);
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_INT(3, (int)entries.count);
  TEST_ASSERT_TRUE(string_list_contains(&entries, "a_test.h"));
  TEST_ASSERT_TRUE(string_list_contains(&entries, "b_test.h"));
  TEST_ASSERT_TRUE(string_list_contains(&entries, "notes.txt"));
  string_list_free(&entries);
}

TEST(list_directory_missing_dir_fails) {
  StringList entries = {0};
  bool ok = list_directory(WILDCARD_FIXTURE_DIR "/does_not_exist", &entries);
  TEST_ASSERT_FALSE(ok);
  string_list_free(&entries);
}

TEST(expand_wildcard_matches_subset) {
  StringList out = {0};
  expand_wildcard(WILDCARD_FIXTURE_DIR "/*_test.h", &out);
  TEST_ASSERT_EQUAL_INT(2, (int)out.count);
  TEST_ASSERT_TRUE(string_list_contains(&out, WILDCARD_FIXTURE_DIR "/a_test.h"));
  TEST_ASSERT_TRUE(string_list_contains(&out, WILDCARD_FIXTURE_DIR "/b_test.h"));
  TEST_ASSERT_FALSE(string_list_contains(&out, WILDCARD_FIXTURE_DIR "/notes.txt"));
  string_list_free(&out);
}

TEST(expand_wildcard_matches_all_with_star) {
  StringList out = {0};
  expand_wildcard(WILDCARD_FIXTURE_DIR "/*", &out);
  TEST_ASSERT_EQUAL_INT(3, (int)out.count);
  string_list_free(&out);
}

TEST(expand_wildcard_no_match_appends_nothing) {
  StringList out = {0};
  expand_wildcard(WILDCARD_FIXTURE_DIR "/*.nonexistent", &out);
  TEST_ASSERT_EQUAL_INT(0, (int)out.count);
  string_list_free(&out);
}
#endif
