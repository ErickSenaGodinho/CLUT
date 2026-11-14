#ifndef RUNNER_GENERATOR_TEST_PARSER_H
#define RUNNER_GENERATOR_TEST_PARSER_H

#include "test_common.h"

TEST(get_macro_set_hook_maps_all_known_hooks) {
  TEST_ASSERT_EQUAL_STRING("USE_HOOK_BEFORE_ALL", get_macro_use_hook(TOKEN_HOOK_BEFORE_ALL));
  TEST_ASSERT_EQUAL_STRING("USE_HOOK_BEFORE_EACH", get_macro_use_hook(TOKEN_HOOK_BEFORE_EACH));
  TEST_ASSERT_EQUAL_STRING("USE_HOOK_AFTER_ALL", get_macro_use_hook(TOKEN_HOOK_AFTER_ALL));
  TEST_ASSERT_EQUAL_STRING("USE_HOOK_AFTER_EACH", get_macro_use_hook(TOKEN_HOOK_AFTER_EACH));
}

TEST(get_macro_set_hook_returns_null_for_non_hook_kind) { TEST_ASSERT_NULL(get_macro_use_hook(TOKEN_TEST)); }

#define PARSER_FIXTURE_FILE "clut_parser_fixture.c"

static void parser_fixture_write(const char *content) {
  FILE *f = fopen(PARSER_FIXTURE_FILE, "wb");
  TEST_ASSERT_TRUE(f != NULL);
  fwrite(content, 1, strlen(content), f);
  fclose(f);
}

static void parser_assert_entry(const RegistrationList *list, size_t index, const char *name) {
  TEST_ASSERT_TRUE(index < list->count);
  TEST_ASSERT_EQUAL_INT((int)strlen(name), (int)list->items[index].name_len);
  TEST_ASSERT_EQUAL_STRING_LEN(name, list->items[index].name, list->items[index].name_len);
}

HOOK_AFTER_ALL(parser_fixture_teardown) { remove(PARSER_FIXTURE_FILE); }

TEST(parse_registers_tests_after_char_and_string_literals) {
  parser_fixture_write("TEST(a) { char c = '{'; }\n"
                       "TEST(b) { char q = '\"'; const char *s = \"\\\"}\"; }\n"
                       "TEST(c) { char e = '\\''; }\n"
                       "TEST(d) { }\n");

  ParsedFile parsed = {0};
  int rc = parse_test_file(PARSER_FIXTURE_FILE, &parsed);

  TEST_ASSERT_EQUAL_INT(0, rc);
  TEST_ASSERT_EQUAL_INT(4, (int)parsed.tests.count);
  parser_assert_entry(&parsed.tests, 0, "a");
  parser_assert_entry(&parsed.tests, 1, "b");
  parser_assert_entry(&parsed.tests, 2, "c");
  parser_assert_entry(&parsed.tests, 3, "d");
  parsed_file_free(&parsed);
}

TEST(parse_registers_reporter_with_designated_initializers) {
  parser_fixture_write("static void on_fail(void *c, const ClutTestResult *r) { (void)c; (void)r; }\n"
                       "REPORTER(my_rep, .on_test_fail = on_fail);\n"
                       "TEST(a) { }\n");

  ParsedFile parsed = {0};
  int rc = parse_test_file(PARSER_FIXTURE_FILE, &parsed);

  TEST_ASSERT_EQUAL_INT(0, rc);
  TEST_ASSERT_EQUAL_INT(1, (int)parsed.reporters.count);
  parser_assert_entry(&parsed.reporters, 0, "my_rep");
  TEST_ASSERT_EQUAL_INT(1, (int)parsed.tests.count);
  parsed_file_free(&parsed);
}

TEST(parse_ignores_reporter_inside_function_body) {
  parser_fixture_write("TEST(a) { REPORTER(x); }\n");

  ParsedFile parsed = {0};
  int rc = parse_test_file(PARSER_FIXTURE_FILE, &parsed);

  TEST_ASSERT_EQUAL_INT(0, rc);
  TEST_ASSERT_EQUAL_INT(0, (int)parsed.reporters.count);
  parsed_file_free(&parsed);
}

TEST(parse_collects_hooks_and_tests_separately) {
  parser_fixture_write("HOOK_BEFORE_ALL(setup) { }\n"
                       "HOOK_AFTER_EACH(cleanup) { }\n"
                       "TEST(a) { }\n");

  ParsedFile parsed = {0};
  int rc = parse_test_file(PARSER_FIXTURE_FILE, &parsed);

  TEST_ASSERT_EQUAL_INT(0, rc);
  TEST_ASSERT_EQUAL_INT(2, (int)parsed.hooks.count);
  TEST_ASSERT_EQUAL_INT((int)TOKEN_HOOK_BEFORE_ALL, (int)parsed.hooks.items[0].kind);
  TEST_ASSERT_EQUAL_INT((int)TOKEN_HOOK_AFTER_EACH, (int)parsed.hooks.items[1].kind);
  TEST_ASSERT_EQUAL_INT(1, (int)parsed.tests.count);
  parsed_file_free(&parsed);
}

PARAM_TEST(parse_malformed_macro_returns_error, const char *,
           {
               "TEST()\n",
               "TEST\n",
               "TEST(a, (\n",
               "REPORTER()\n",
               "HOOK_BEFORE_ALL(\n",
           }) {
  parser_fixture_write(input);

  ParsedFile parsed = {0};
  int rc = parse_test_file(PARSER_FIXTURE_FILE, &parsed);

  TEST_ASSERT_EQUAL_INT(1, rc);
  parsed_file_free(&parsed);
}

TEST(parse_missing_file_returns_error) {
  ParsedFile parsed = {0};
  int rc = parse_test_file("clut_parser_fixture_does_not_exist.c", &parsed);

  TEST_ASSERT_EQUAL_INT(1, rc);
  TEST_ASSERT_TRUE(parsed.content == NULL);
  parsed_file_free(&parsed);
}

#endif /* RUNNER_GENERATOR_TEST_PARSER_H */
