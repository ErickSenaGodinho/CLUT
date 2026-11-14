#ifndef RUNNER_GENERATOR_TEST_LEXER_H
#define RUNNER_GENERATOR_TEST_LEXER_H

#include "test_common.h"
#include <string.h>

TEST(plain_symbol) {
  const char *src = "hello_world";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, t.kind);
  TEST_ASSERT_EQUAL_INT((int)strlen(src), (int)t.text_len);
  TEST_ASSERT_EQUAL_STRING_LEN(src, t.text, t.text_len);
}

TEST(underscore_symbol) {
  const char *src = "_private42";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, t.kind);
  TEST_ASSERT_EQUAL_INT((int)strlen(src), (int)t.text_len);
}

TEST(empty_input_returns_end) {
  Lexer lx = lexer_new("f.c", "", 0);
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_END, t.kind);
  TEST_ASSERT_EQUAL_INT(0, (int)t.text_len);
}

TEST(trailing_ws_returns_end) {
  const char *src = "   \n\t  ";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_END, t.kind);
}

TEST(unknown_char_returns_token) {
  const char *src = "@";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_UNKNOWN, t.kind);
  TEST_ASSERT_EQUAL_INT(1, (int)t.text_len);
}

TEST(string_literal_quotes) {
  const char *src = "\"a string\" rest";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_STRING, t.kind);
  TEST_ASSERT_EQUAL_INT((int)strlen("\"a string\""), (int)t.text_len);
  TEST_ASSERT_EQUAL_STRING_LEN("\"a string\"", t.text, t.text_len);
}

TEST(string_with_test_macro_text) {
  const char *src = "\"TEST(fake)\"";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_STRING, t.kind);
}

TEST(line_comment) {
  const char *src = "// TEST(fake) comment\nnext_symbol";
  Lexer lx = lexer_new("f.c", src, strlen(src));

  Token comment = lexer_next(&lx);
  TEST_ASSERT_EQUAL_INT(TOKEN_COMMENT, comment.kind);

  Token after = lexer_next(&lx);
  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, after.kind);
  TEST_ASSERT_EQUAL_STRING_LEN("next_symbol", after.text, after.text_len);
}

TEST(block_comment_multiline) {
  const char *src = "/* line1\nline2\nline3 */ after";
  Lexer lx = lexer_new("f.c", src, strlen(src));

  Token comment = lexer_next(&lx);
  TEST_ASSERT_EQUAL_INT(TOKEN_COMMENT, comment.kind);

  TEST_ASSERT_EQUAL_INT(2, (int)lx.line);

  Token after = lexer_next(&lx);
  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, after.kind);
  TEST_ASSERT_EQUAL_STRING_LEN("after", after.text, after.text_len);
}

TEST(directive_is_single_token) {
  const char *src = "#include <stdio.h>\nint x;";
  Lexer lx = lexer_new("f.c", src, strlen(src));

  Token preproc = lexer_next(&lx);
  TEST_ASSERT_EQUAL_INT(TOKEN_PREPROC, preproc.kind);

  Token after = lexer_next(&lx);
  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, after.kind);
  TEST_ASSERT_EQUAL_STRING_LEN("int", after.text, after.text_len);
}

TEST(parens_braces_order) {
  const char *src = "(){}";
  Lexer lx = lexer_new("f.c", src, strlen(src));

  TEST_ASSERT_EQUAL_INT(TOKEN_OPEN_PAREN, lexer_next(&lx).kind);
  TEST_ASSERT_EQUAL_INT(TOKEN_CLOSE_PAREN, lexer_next(&lx).kind);
  TEST_ASSERT_EQUAL_INT(TOKEN_OPEN_BRACE, lexer_next(&lx).kind);
  TEST_ASSERT_EQUAL_INT(TOKEN_CLOSE_BRACE, lexer_next(&lx).kind);
}

TEST(nested_braces_track_depth) {
  const char *src = "{{}}";
  Lexer lx = lexer_new("f.c", src, strlen(src));

  lexer_next(&lx); /* { */
  TEST_ASSERT_EQUAL_INT(1, (int)lx.brace_depth);
  lexer_next(&lx); /* { */
  TEST_ASSERT_EQUAL_INT(2, (int)lx.brace_depth);
  lexer_next(&lx); /* } */
  TEST_ASSERT_EQUAL_INT(1, (int)lx.brace_depth);
  lexer_next(&lx); /* } */
  TEST_ASSERT_EQUAL_INT(0, (int)lx.brace_depth);
}

TEST(test_macro_detected_top_level) {
  const char *src = "TEST(my_test)";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_TEST, t.kind);
  TEST_ASSERT_EQUAL_STRING_LEN("TEST", t.text, t.text_len);
}

typedef struct {
  const char *src;
  const char *expected_word;
} LexerCase;

PARAM_TEST(param_and_repeated_test_macros, LexerCase,
           {
               {"PARAM_TEST(foo)", "PARAM_TEST"},
               {"REPEATED_TEST(foo)", "REPEATED_TEST"},
               {"REPEATED_TEST_WITH_THRESHOLD(foo)", "REPEATED_TEST_WITH_THRESHOLD"},
           }) {
  Lexer lx = lexer_new("f.c", input.src, strlen(input.src));
  Token t = lexer_next(&lx);
  TEST_ASSERT_EQUAL_INT(TOKEN_TEST, t.kind);
  TEST_ASSERT_EQUAL_INT((int)strlen(input.expected_word), (int)t.text_len);
  TEST_ASSERT_EQUAL_STRING_LEN(input.expected_word, t.text, t.text_len);
}

TEST(identifier_prefix_not_keyword) {
  const char *src = "TESTER(x)";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, t.kind);
  TEST_ASSERT_EQUAL_STRING_LEN("TESTER", t.text, t.text_len);
}

typedef struct {
  const char *src;
  TokenKind expected;
} HookMacroCase;

PARAM_TEST(all_hook_macros, HookMacroCase,
           {
               {"HOOK_BEFORE_ALL(x)", TOKEN_HOOK_BEFORE_ALL},
               {"HOOK_BEFORE_EACH(x)", TOKEN_HOOK_BEFORE_EACH},
               {"HOOK_AFTER_ALL(x)", TOKEN_HOOK_AFTER_ALL},
               {"HOOK_AFTER_EACH(x)", TOKEN_HOOK_AFTER_EACH},
           }) {
  Lexer lx = lexer_new("f.c", input.src, strlen(input.src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT((int)input.expected, (int)t.kind);
}

TEST(keywords_inside_braces_ignored) {
  const char *src = "{ TEST(x); HOOK_BEFORE_ALL(y); }";
  Lexer lx = lexer_new("f.c", src, strlen(src));

  TEST_ASSERT_EQUAL_INT(TOKEN_OPEN_BRACE, lexer_next(&lx).kind);  /* { */
  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, lexer_next(&lx).kind);      /* TEST (not TOKEN_TEST) */
  TEST_ASSERT_EQUAL_INT(TOKEN_OPEN_PAREN, lexer_next(&lx).kind);  /* ( */
  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, lexer_next(&lx).kind);      /* x */
  TEST_ASSERT_EQUAL_INT(TOKEN_CLOSE_PAREN, lexer_next(&lx).kind); /* ) */
  TEST_ASSERT_EQUAL_INT(TOKEN_UNKNOWN, lexer_next(&lx).kind);     /* ; */
  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, lexer_next(&lx).kind);      /* BEFORE_ALL_HOOK (not TOKEN_BEFORE_ALL_HOOK) */
}

typedef struct {
  const char *src;
  int expected_len;
} QuotedLiteralCase;

PARAM_TEST(quoted_literal_is_single_token_and_keeps_depth, QuotedLiteralCase,
           {
               {"'{' TEST(x)", 3},
               {"'}' TEST(x)", 3},
               {"'\"' TEST(x)", 3},
               {"'\\'' TEST(x)", 4},
               {"'\\\\' TEST(x)", 4},
               {"\"{\" TEST(x)", 3},
               {"\"\\\"}\" TEST(x)", 5},
               {"\"\\\\\" TEST(x)", 4},
           }) {
  Lexer lx = lexer_new("f.c", input.src, strlen(input.src));
  Token lit = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_STRING, lit.kind);
  TEST_ASSERT_EQUAL_INT(input.expected_len, (int)lit.text_len);
  TEST_ASSERT_EQUAL_INT(0, (int)lx.brace_depth);
  TEST_ASSERT_EQUAL_INT(TOKEN_TEST, lexer_next(&lx).kind);
}

TEST(unterminated_string_stops_at_newline) {
  const char *src = "\"abc\nTEST(x)";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token lit = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_STRING, lit.kind);
  TEST_ASSERT_EQUAL_INT(4, (int)lit.text_len);
  TEST_ASSERT_EQUAL_INT(TOKEN_TEST, lexer_next(&lx).kind);
}

TEST(stray_apostrophe_only_costs_one_line) {
  const char *src = "don't\nTEST(x)";
  Lexer lx = lexer_new("f.c", src, strlen(src));

  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, lexer_next(&lx).kind);
  TEST_ASSERT_EQUAL_INT(TOKEN_STRING, lexer_next(&lx).kind);
  TEST_ASSERT_EQUAL_INT(TOKEN_TEST, lexer_next(&lx).kind);
}

TEST(digit_separator_is_not_a_char_literal) {
  const char *src = "1'000'000 TEST(x)";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token num = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_UNKNOWN, num.kind);
  TEST_ASSERT_EQUAL_INT(9, (int)num.text_len);
  TEST_ASSERT_EQUAL_INT(TOKEN_TEST, lexer_next(&lx).kind);
}

TEST(unterminated_block_comment_does_not_overrun) {
  const char *src = "/* never closed";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_COMMENT, t.kind);
  TEST_ASSERT_EQUAL_INT((int)strlen(src), (int)t.text_len);
  TEST_ASSERT_EQUAL_INT((int)strlen(src), (int)lx.cursor);
  TEST_ASSERT_EQUAL_INT(TOKEN_END, lexer_next(&lx).kind);
}

TEST(line_comment_at_eof_without_newline_does_not_overrun) {
  const char *src = "// last line";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_COMMENT, t.kind);
  TEST_ASSERT_EQUAL_INT((int)strlen(src), (int)t.text_len);
  TEST_ASSERT_EQUAL_INT((int)strlen(src), (int)lx.cursor);
}

TEST(directive_at_eof_without_newline_does_not_overrun) {
  const char *src = "#endif";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_PREPROC, t.kind);
  TEST_ASSERT_EQUAL_INT((int)strlen(src), (int)t.text_len);
  TEST_ASSERT_EQUAL_INT((int)strlen(src), (int)lx.cursor);
}

TEST(unterminated_string_at_eof_does_not_overrun) {
  const char *src = "\"abc";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_STRING, t.kind);
  TEST_ASSERT_EQUAL_INT((int)strlen(src), (int)t.text_len);
  TEST_ASSERT_EQUAL_INT((int)strlen(src), (int)lx.cursor);
}

TEST(directive_continuation_swallows_next_line) {
  const char *src = "#define M \\\n  TEST(fake)\nTEST(real)";
  Lexer lx = lexer_new("f.c", src, strlen(src));

  TEST_ASSERT_EQUAL_INT(TOKEN_PREPROC, lexer_next(&lx).kind);
  TEST_ASSERT_EQUAL_INT(TOKEN_TEST, lexer_next(&lx).kind);
  TEST_ASSERT_EQUAL_INT(TOKEN_OPEN_PAREN, lexer_next(&lx).kind);

  Token name = lexer_next(&lx);
  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, name.kind);
  TEST_ASSERT_EQUAL_STRING_LEN("real", name.text, name.text_len);
}

TEST(directive_continuation_with_crlf) {
  const char *src = "#define M \\\r\n  TEST(fake)\r\nTEST(real)";
  Lexer lx = lexer_new("f.c", src, strlen(src));

  TEST_ASSERT_EQUAL_INT(TOKEN_PREPROC, lexer_next(&lx).kind);
  TEST_ASSERT_EQUAL_INT(TOKEN_TEST, lexer_next(&lx).kind);
  TEST_ASSERT_EQUAL_INT(TOKEN_OPEN_PAREN, lexer_next(&lx).kind);

  Token name = lexer_next(&lx);
  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, name.kind);
  TEST_ASSERT_EQUAL_STRING_LEN("real", name.text, name.text_len);
}

TEST(stray_close_brace_does_not_underflow_depth) {
  const char *src = "} TEST(x)";
  Lexer lx = lexer_new("f.c", src, strlen(src));

  TEST_ASSERT_EQUAL_INT(TOKEN_CLOSE_BRACE, lexer_next(&lx).kind);
  TEST_ASSERT_EQUAL_INT(0, (int)lx.brace_depth);
  TEST_ASSERT_EQUAL_INT(TOKEN_TEST, lexer_next(&lx).kind);
}

TEST(reporter_macro_detected_top_level) {
  const char *src = "REPORTER(r)";
  Lexer lx = lexer_new("f.c", src, strlen(src));
  Token t = lexer_next(&lx);

  TEST_ASSERT_EQUAL_INT(TOKEN_REPORTER, t.kind);
  TEST_ASSERT_EQUAL_STRING_LEN("REPORTER", t.text, t.text_len);
}

PARAM_TEST(reporter_lookalikes_are_symbols, const char *,
           {
               "USE_REPORTER(r)",
               "REPORTER_X(r)",
               "MY_REPORTER(r)",
           }) {
  Lexer lx = lexer_new("f.c", input, strlen(input));
  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, lexer_next(&lx).kind);
}

TEST(reporter_inside_braces_ignored) {
  const char *src = "{ REPORTER(r) }";
  Lexer lx = lexer_new("f.c", src, strlen(src));

  TEST_ASSERT_EQUAL_INT(TOKEN_OPEN_BRACE, lexer_next(&lx).kind);
  TEST_ASSERT_EQUAL_INT(TOKEN_SYMBOL, lexer_next(&lx).kind);
}

#endif /* RUNNER_GENERATOR_TEST_LEXER_H */
