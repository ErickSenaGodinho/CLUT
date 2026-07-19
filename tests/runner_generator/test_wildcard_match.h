#ifndef RUNNER_GENERATOR_TEST_WILDCARD_MATCH_H
#define RUNNER_GENERATOR_TEST_WILDCARD_MATCH_H
#include "test_common.h"

TEST(wildcard_match_exact) {
  TEST_ASSERT_TRUE(wildcard_match("foo.h", "foo.h"));
  TEST_ASSERT_FALSE(wildcard_match("foo.h", "bar.h"));
}

TEST(wildcard_match_star_matches_all) {
  TEST_ASSERT_TRUE(wildcard_match("*", "anything.h"));
  TEST_ASSERT_TRUE(wildcard_match("*", ""));
}

TEST(wildcard_match_star_suffix) {
  TEST_ASSERT_TRUE(wildcard_match("*.h", "test_foo.h"));
  TEST_ASSERT_TRUE(wildcard_match("*.h", ".h"));
  TEST_ASSERT_FALSE(wildcard_match("*.h", "test_foo.c"));
}

TEST(wildcard_match_star_prefix) {
  TEST_ASSERT_TRUE(wildcard_match("test_*", "test_foo.h"));
  TEST_ASSERT_FALSE(wildcard_match("test_*", "foo_test.h"));
}

TEST(wildcard_match_star_middle) {
  TEST_ASSERT_TRUE(wildcard_match("test_*.h", "test_foo_bar.h"));
  TEST_ASSERT_FALSE(wildcard_match("test_*.h", "test_foo_bar.c"));
}

TEST(wildcard_match_question_mark) {
  TEST_ASSERT_TRUE(wildcard_match("a?c", "abc"));
  TEST_ASSERT_FALSE(wildcard_match("a?c", "ac"));
  TEST_ASSERT_FALSE(wildcard_match("a?c", "abbc"));
}

TEST(wildcard_match_empty_pattern_empty_string) { TEST_ASSERT_TRUE(wildcard_match("", "")); }

TEST(wildcard_match_empty_pattern_nonempty_string) { TEST_ASSERT_FALSE(wildcard_match("", "abc")); }

TEST(wildcard_match_pattern_longer_than_string) { TEST_ASSERT_FALSE(wildcard_match("abc?", "abc")); }

TEST(wildcard_match_multiple_stars) { TEST_ASSERT_TRUE(wildcard_match("**.h", "foo.h")); }
#endif
