#ifndef RUNNER_GENERATOR_TEST_PARSER_H
#define RUNNER_GENERATOR_TEST_PARSER_H

#include "test_common.h"

TEST(get_macro_set_hook_maps_all_known_hooks) {
  TEST_ASSERT_EQUAL_STRING("SET_BEFORE_ALL", get_macro_set_hook(TOKEN_BEFORE_ALL_HOOK));
  TEST_ASSERT_EQUAL_STRING("SET_BEFORE_EACH", get_macro_set_hook(TOKEN_BEFORE_EACH_HOOK));
  TEST_ASSERT_EQUAL_STRING("SET_AFTER_ALL", get_macro_set_hook(TOKEN_AFTER_ALL_HOOK));
  TEST_ASSERT_EQUAL_STRING("SET_AFTER_EACH", get_macro_set_hook(TOKEN_AFTER_EACH_HOOK));
}

TEST(get_macro_set_hook_returns_null_for_non_hook_kind) { TEST_ASSERT_NULL(get_macro_set_hook(TOKEN_TEST)); }

#endif /* RUNNER_GENERATOR_TEST_PARSER_H */
