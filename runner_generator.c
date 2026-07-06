#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

static char *read_entire_file(const char *path, size_t *out_size) {
  FILE *f = fopen(path, "rb");
  if (f == NULL)
    return NULL;

  if (fseek(f, 0, SEEK_END) < 0)
    goto error;

  long long file_size = 0;
#ifndef _WIN32
  file_size = ftell(f);
#else
  file_size = _telli64(_fileno(f));
#endif
  if (file_size < 0)
    goto error;

  if (fseek(f, 0, SEEK_SET) < 0)
    goto error;

  char *buffer = malloc(file_size + 1);
  if (buffer == NULL)
    goto error;

  size_t read_bytes = fread(buffer, 1, file_size, f);
  buffer[read_bytes] = '\0';
  if (out_size)
    *out_size = read_bytes;

  fclose(f);
  return buffer;

error:
  if (f)
    fclose(f);
  return NULL;
}

static bool is_symbol_start(char x) { return isalpha(x) || x == '_'; }
static bool is_symbol(char x) { return isalnum(x) || x == '_'; }

typedef enum {
  TOKEN_END = 0,
  TOKEN_SYMBOL = 1 << 0,
  TOKEN_UNKNOWN = 1 << 1,
  TOKEN_HOOK = 1 << 2,

  TOKEN_BEFORE_ALL_HOOK = TOKEN_HOOK | (1 << 3),
  TOKEN_BEFORE_EACH_HOOK = TOKEN_HOOK | (1 << 4),
  TOKEN_AFTER_ALL_HOOK = TOKEN_HOOK | (1 << 5),
  TOKEN_AFTER_EACH_HOOK = TOKEN_HOOK | (1 << 6),

  TOKEN_TEST = 1 << 7,

  TOKEN_PREPROC = 1 << 8,
  TOKEN_COMMENT = 1 << 9,
  TOKEN_STRING = 1 << 10,
  TOKEN_OPEN_PAREN = 1 << 11,
  TOKEN_CLOSE_PAREN = 1 << 12,
  TOKEN_OPEN_BRACE = 1 << 13,
  TOKEN_CLOSE_BRACE = 1 << 14,
} TokenKind;

typedef struct {
  const char *text;
  TokenKind kind;
} LiteralToken;

static const LiteralToken test_tokens[] = {
    {"REPEATED_TEST_WITH_THRESHOLD", TOKEN_TEST},
    {"REPEATED_TEST", TOKEN_TEST},
    {"PARAM_TEST", TOKEN_TEST},
    {"TEST", TOKEN_TEST},
};

static const LiteralToken literal_tokens[] = {
    {"(", TOKEN_OPEN_PAREN},
    {")", TOKEN_CLOSE_PAREN},
    {"{", TOKEN_OPEN_BRACE},
    {"}", TOKEN_CLOSE_BRACE},
};

#define HOOK_TOKEN_LIST(X)                                                                                                                                                                                                                                         \
  X(BEFORE_ALL)                                                                                                                                                                                                                                                    \
  X(BEFORE_EACH)                                                                                                                                                                                                                                                   \
  X(AFTER_ALL)                                                                                                                                                                                                                                                     \
  X(AFTER_EACH)

static const LiteralToken hook_tokens[] = {
#define X(name) {#name "_HOOK", TOKEN_##name##_HOOK},
    HOOK_TOKEN_LIST(X)
#undef X
};

static const char *get_macro_set_hook(TokenKind kind) {
  switch (kind) {
#define X(name)                                                                                                                                                                                                                                                    \
  case TOKEN_##name##_HOOK:                                                                                                                                                                                                                                        \
    return "SET_" #name;
    HOOK_TOKEN_LIST(X)
#undef X
  default:
    return NULL;
  }
}

static const char *token_kind_name(TokenKind kind) {
  switch (kind) {
  case TOKEN_END:
    return "end of file";
  case TOKEN_SYMBOL:
    return "an symbol";
  case TOKEN_UNKNOWN:
    return "an unrecognized token";
  case TOKEN_TEST:
    return "a test macro (TEST/REPEATED_TEST/PARAM_TEST)";
  case TOKEN_PREPROC:
    return "a preprocessor directive";
  case TOKEN_COMMENT:
    return "a comment";
  case TOKEN_STRING:
    return "a string literal";
  case TOKEN_OPEN_PAREN:
    return "'('";
  case TOKEN_CLOSE_PAREN:
    return "')'";
  case TOKEN_OPEN_BRACE:
    return "'{'";
  case TOKEN_CLOSE_BRACE:
    return "'}'";
  default:
    if (kind & TOKEN_HOOK)
      return "a hook macro (BEFORE_ALL_HOOK/BEFORE_EACH_HOOK/AFTER_ALL_HOOK/AFTER_EACH_HOOK)";
    return "an unknown token kind";
  }
}

typedef struct {
  TokenKind kind;
  const char *text;
  size_t text_len;
} Token;

typedef struct {
  const char *file_name;
  const char *content;
  size_t content_len;
  size_t cursor;
  size_t line;
  size_t begin_of_line;
  size_t brace_depth;
} Lexer;

static Lexer lexer_new(const char *file_name, const char *content, size_t content_len) {
  Lexer lexer = {0};
  lexer.file_name = file_name;
  lexer.content = content;
  lexer.content_len = content_len;
  return lexer;
}

static void lexer_advance(Lexer *lexer, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    char x = lexer->content[lexer->cursor];
    lexer->cursor += 1;
    if (x == '\n') {
      lexer->line += 1;
      lexer->begin_of_line = lexer->cursor;
    }
  }
}

static void lexer_trim_left(Lexer *lexer) {
  while (lexer->cursor < lexer->content_len && isspace(lexer->content[lexer->cursor])) {
    lexer_advance(lexer, 1);
  }
}

static bool lexer_starts_with(Lexer *lexer, const char *prefix) {
  size_t prefix_len = strlen(prefix);
  if (prefix_len == 0)
    return true;
  if (lexer->cursor + prefix_len > lexer->content_len)
    return false;
  for (size_t i = 0; i < prefix_len; ++i) {
    if (prefix[i] != lexer->content[lexer->cursor + i])
      return false;
  }
  return true;
}

static bool lexer_matches_word(Lexer *lexer, const char *prefix) {
  if (!lexer_starts_with(lexer, prefix))
    return false;
  size_t next = lexer->cursor + strlen(prefix);
  return next >= lexer->content_len || !is_symbol(lexer->content[next]);
}

static void lexer_consume_until_char(Lexer *lexer, Token *token, char end) {
  while (lexer->cursor < lexer->content_len && lexer->content[lexer->cursor] != end) {
    token->text_len++;
    lexer_advance(lexer, 1);
  }
  token->text_len++;
  lexer_advance(lexer, 1);
}

static void lexer_consume_until_str(Lexer *lexer, Token *token, const char *end) {
  while (lexer->cursor < lexer->content_len && !lexer_starts_with(lexer, end)) {
    token->text_len++;
    lexer_advance(lexer, 1);
  }
  size_t end_str_len = strlen(end);
  token->text_len += end_str_len;
  lexer_advance(lexer, end_str_len);
}

static Token lexer_read_line_token(Lexer *lexer, TokenKind kind) {
  Token token = {
      .kind = kind,
      .text = &lexer->content[lexer->cursor],
      .text_len = 0,
  };
  lexer_consume_until_char(lexer, &token, '\n');
  return token;
}

static bool lexer_match_keyword(Lexer *lexer, const LiteralToken *table, size_t count, Token *token) {
  for (size_t i = 0; i < count; i++) {
    if (lexer_matches_word(lexer, table[i].text)) {
      size_t text_len = strlen(table[i].text);
      lexer_advance(lexer, text_len);
      token->text_len = text_len;
      token->kind = table[i].kind;
      return true;
    }
  }
  return false;
}

static bool is_token_test(Lexer *lexer, Token *token) { return lexer->brace_depth == 0 && lexer_match_keyword(lexer, test_tokens, ARRAY_LEN(test_tokens), token); }

static bool is_token_hook(Lexer *lexer, Token *token) { return lexer->brace_depth == 0 && lexer_match_keyword(lexer, hook_tokens, ARRAY_LEN(hook_tokens), token); }

static bool is_token_literal(Lexer *lexer, Token *token) {
  for (size_t i = 0; i < ARRAY_LEN(literal_tokens); i++) {
    const LiteralToken *literal = &literal_tokens[i];

    if (lexer_starts_with(lexer, literal->text)) {
      size_t text_len = strlen(literal->text);

      lexer_advance(lexer, text_len);
      token->text_len = text_len;
      token->kind = literal->kind;

      return true;
    }
  }
  return false;
}

static Token lexer_next(Lexer *lexer) {
  lexer_trim_left(lexer);

  Token token = {.kind = TOKEN_END, .text = &lexer->content[lexer->cursor], .text_len = 0};

  if (lexer->cursor >= lexer->content_len)
    return token;

  if (lexer->content[lexer->cursor] == '"') {
    token.kind = TOKEN_STRING;
    token.text_len += 1;
    lexer_advance(lexer, 1);
    lexer_consume_until_char(lexer, &token, '"');
    return token;
  }

  if (lexer_starts_with(lexer, "//"))
    return lexer_read_line_token(lexer, TOKEN_COMMENT);

  if (lexer_starts_with(lexer, "/*")) {
    token.kind = TOKEN_COMMENT;
    token.text_len += 2;
    lexer_advance(lexer, 2);
    lexer_consume_until_str(lexer, &token, "*/");
    return token;
  }

  if (lexer->content[lexer->cursor] == '#')
    return lexer_read_line_token(lexer, TOKEN_PREPROC);

  if (is_token_test(lexer, &token))
    return token;

  if (is_token_hook(lexer, &token))
    return token;

  if (is_token_literal(lexer, &token)) {
    if (token.kind == TOKEN_OPEN_BRACE)
      lexer->brace_depth++;
    else if (token.kind == TOKEN_CLOSE_BRACE)
      lexer->brace_depth--;
    return token;
  }

  if (is_symbol_start(lexer->content[lexer->cursor])) {
    token.kind = TOKEN_SYMBOL;
    while (lexer->cursor < lexer->content_len && is_symbol(lexer->content[lexer->cursor])) {
      token.text_len += 1;
      lexer_advance(lexer, 1);
    }
    return token;
  }

  token.text_len = 1;
  lexer_advance(lexer, 1);
  token.kind = TOKEN_UNKNOWN;
  return token;
}

static Token lexer_expect_kind(Lexer *lexer, TokenKind kind, const char *what) {
  Token token = lexer_next(lexer);
  if (token.kind != kind) {
    fprintf(stderr, "%s:%zu: error: expected %s, but got %s", lexer->file_name, lexer->line + 1, what, token_kind_name(token.kind));
    if (token.kind != TOKEN_END && token.text_len > 0 && token.text_len < 40)
      fprintf(stderr, " ('%.*s')", (int)token.text_len, token.text);
    fprintf(stderr, "\n");
    exit(1);
  }
  return token;
}

static void lexer_skip_balanced_parens(Lexer *lexer) {
  size_t depth = 1;
  while (depth > 0) {
    Token token = lexer_next(lexer);
    if (token.kind == TOKEN_END) {
      fprintf(stderr, "%s:%zu: error: unexpected end of file inside argument list, unterminated '('\n", lexer->file_name, lexer->line + 1);
      exit(1);
    }
    if (token.kind == TOKEN_OPEN_PAREN)
      depth++;
    else if (token.kind == TOKEN_CLOSE_PAREN)
      depth--;
  }
}

typedef struct {
  TokenKind kind;
  const char *name;
  size_t name_len;
} RegistrationEntry;

typedef struct {
  RegistrationEntry *items;
  size_t count;
  size_t capacity;
} RegistrationList;

#define REGISTRATION_LIST_INIT_CAP 16

static void registration_list_append(RegistrationList *list, TokenKind kind, const char *name, size_t name_len) {
  if (list->count == list->capacity) {
    list->capacity = list->capacity == 0 ? REGISTRATION_LIST_INIT_CAP : list->capacity * 2;
    list->items = realloc(list->items, list->capacity * sizeof(*list->items));
  }
  list->items[list->count++] = (RegistrationEntry){
      .kind = kind,
      .name = name,
      .name_len = name_len,
  };
}

static void registration_list_free(RegistrationList *list) {
  free(list->items);
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
}

static void emit_hooks(FILE *out, const RegistrationList *hooks) {
  for (size_t i = 0; i < hooks->count; ++i) {
    const RegistrationEntry *entry = &hooks->items[i];
    fprintf(out, "\t\t%s(%.*s);\n", get_macro_set_hook(entry->kind), (int)entry->name_len, entry->name);
  }

  if (hooks->count)
    fputc('\n', out);
}

static void emit_suite(FILE *out, const RegistrationList *hooks, const RegistrationList *tests) {
  if (tests->count == 0)
    return;

  fputs("\tSUITE_BEGIN();\n", out);
  emit_hooks(out, hooks);
  for (size_t i = 0; i < tests->count; ++i) {
    const RegistrationEntry *entry = &tests->items[i];
    fprintf(out, "\t\tRUN_TEST(%.*s);\n", (int)entry->name_len, entry->name);
  }
  fputs("\tSUITE_END();\n\n", out);
}

static int parse_test_file(const char *file_name, FILE *out) {
  size_t content_len;
  const char *content = read_entire_file(file_name, &content_len);
  if (!content) {
    fprintf(stderr, "error: could not read '%s'\n", file_name);
    return 1;
  }

  Lexer lexer = lexer_new(file_name, content, content_len);
  Token token = lexer_next(&lexer);

  RegistrationList tests = {0};
  RegistrationList hooks = {0};

  while (token.kind != TOKEN_END) {
    RegistrationList *target = NULL;

    if (token.kind == TOKEN_TEST)
      target = &tests;
    else if (token.kind & TOKEN_HOOK)
      target = &hooks;

    if (target) {
      lexer_expect_kind(&lexer, TOKEN_OPEN_PAREN, "'(' after macro name");
      Token name = lexer_expect_kind(&lexer, TOKEN_SYMBOL, "an identifier (test/hook name)");
      lexer_skip_balanced_parens(&lexer);
      registration_list_append(target, token.kind, name.text, name.text_len);
    }

    token = lexer_next(&lexer);
  }

  emit_suite(out, &hooks, &tests);

  registration_list_free(&tests);
  registration_list_free(&hooks);
  free((void *)content);
  return 0;
}

#define CLUT_PATH_MAX 4096

static bool path_is_separator(char c) { return c == '/' || c == '\\'; }

static size_t path_get_prefix_len(const char *path) {
#ifdef _WIN32
  if (isalpha((unsigned char)path[0]) && path[1] == ':' && path_is_separator(path[2]))
    return 3;
#endif
  if (path_is_separator(path[0]))
    return 1;
  return 0;
}

static bool path_is_absolute(const char *path) { return path_get_prefix_len(path) > 0; }

static void path_normalize_separators(char *path) {
  for (char *p = path; *p != '\0'; ++p) {
    if (*p == '\\')
      *p = '/';
  }
}

static bool path_dirname(const char *path, char *out, size_t out_size) {
  const char *last_sep = NULL;
  for (const char *p = path; *p != '\0'; ++p) {
    if (path_is_separator(*p))
      last_sep = p;
  }

  if (last_sep == NULL)
    return (size_t)snprintf(out, out_size, ".") < out_size;

  size_t len = (size_t)(last_sep - path);
  if (len == 0)
    len = 1;

  if (len >= out_size)
    return false;

  memcpy(out, path, len);
  out[len] = '\0';
  return true;
}

static bool path_join_absolute(const char *path, char *out, size_t out_size) {
  if (path_is_absolute(path)) {
    size_t len = strlen(path);
    if (len >= out_size)
      return false;
    memcpy(out, path, len + 1);
    return true;
  }

  char cwd[CLUT_PATH_MAX];
#ifdef _WIN32
  if (_getcwd(cwd, sizeof(cwd)) == NULL)
    return false;
#else
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    return false;
#endif

  size_t cwd_len = strlen(cwd);
  size_t path_len = strlen(path);
  bool need_sep = cwd_len > 0 && !path_is_separator(cwd[cwd_len - 1]);
  size_t total_len = cwd_len + (need_sep ? 1 : 0) + path_len;
  if (total_len >= out_size)
    return false;

  memcpy(out, cwd, cwd_len);
  size_t pos = cwd_len;
  if (need_sep)
    out[pos++] = '/';
  memcpy(out + pos, path, path_len + 1);
  return true;
}

#define PATH_MAX_SEGMENTS 256

static bool path_collapse_dots(const char *in, char *out, size_t out_size) {
  size_t prefix_len = path_get_prefix_len(in);
  if (prefix_len >= out_size)
    return false;
  memcpy(out, in, prefix_len);

  const char *segment_start[PATH_MAX_SEGMENTS];
  size_t segment_len[PATH_MAX_SEGMENTS];
  size_t segment_count = 0;

  const char *p = in + prefix_len;
  while (*p != '\0') {
    while (*p == '/')
      p++;
    if (*p == '\0')
      break;

    const char *start = p;
    while (*p != '\0' && *p != '/')
      p++;
    size_t len = (size_t)(p - start);

    bool is_dot = len == 1 && start[0] == '.';
    bool is_dotdot = len == 2 && start[0] == '.' && start[1] == '.';

    if (is_dot)
      continue;

    if (is_dotdot) {
      if (segment_count > 0)
        segment_count--;
      continue;
    }

    if (segment_count >= PATH_MAX_SEGMENTS)
      return false;
    segment_start[segment_count] = start;
    segment_len[segment_count] = len;
    segment_count++;
  }

  size_t pos = prefix_len;
  for (size_t i = 0; i < segment_count; ++i) {
    if (i > 0) {
      if (pos + 1 >= out_size)
        return false;
      out[pos++] = '/';
    }
    if (pos + segment_len[i] >= out_size)
      return false;
    memcpy(out + pos, segment_start[i], segment_len[i]);
    pos += segment_len[i];
  }

  out[pos] = '\0';
  return true;
}

static bool path_get_absolute(const char *path, char *out, size_t out_size) {
  char joined[CLUT_PATH_MAX];
  if (!path_join_absolute(path, joined, sizeof(joined)))
    return false;

  path_normalize_separators(joined);
  return path_collapse_dots(joined, out, out_size);
}

static bool path_same_root(const char *from_path, const char *to_path) {
  size_t from_prefix_len = path_get_prefix_len(from_path);
  size_t to_prefix_len = path_get_prefix_len(to_path);
  if (from_prefix_len != to_prefix_len)
    return false;

#ifdef _WIN32
  for (size_t i = 0; i < from_prefix_len; ++i) {
    if (tolower((unsigned char)from_path[i]) != tolower((unsigned char)to_path[i]))
      return false;
  }
  return true;
#else
  return memcmp(from_path, to_path, from_prefix_len) == 0;
#endif
}

static void path_common_boundary(const char *from_path, const char *to_path, size_t *from_end, size_t *to_end) {
  size_t i = 0;
  size_t last_sep = 0;

  while (from_path[i] != '\0' && to_path[i] != '\0' && from_path[i] == to_path[i]) {
    if (from_path[i] == '/')
      last_sep = i + 1;
    i++;
  }

  if (from_path[i] == '\0' && to_path[i] == '\0') {
    *from_end = i;
    *to_end = i;
  } else if (from_path[i] == '\0' && to_path[i] == '/') {
    *from_end = i;
    *to_end = i + 1;
  } else if (from_path[i] == '/' && to_path[i] == '\0') {
    *from_end = i + 1;
    *to_end = i;
  } else {
    *from_end = last_sep;
    *to_end = last_sep;
  }
}

static size_t path_count_remaining_segments(const char *remainder) {
  if (*remainder == '\0')
    return 0;

  size_t count = 1;
  for (const char *p = remainder; *p != '\0'; ++p) {
    if (*p == '/')
      count++;
  }
  return count;
}

static bool path_relative(const char *from_dir_abs, const char *to_file_abs, char *out, size_t out_size) {
  if (!path_same_root(from_dir_abs, to_file_abs))
    return false;

  size_t from_end, to_end;
  path_common_boundary(from_dir_abs, to_file_abs, &from_end, &to_end);

  size_t ups = path_count_remaining_segments(from_dir_abs + from_end);
  const char *suffix = to_file_abs + to_end;

  size_t pos = 0;
  for (size_t i = 0; i < ups; ++i) {
    if (pos + 3 >= out_size)
      return false;
    memcpy(out + pos, "../", 3);
    pos += 3;
  }

  size_t suffix_len = strlen(suffix);
  if (pos + suffix_len >= out_size)
    return false;

  memcpy(out + pos, suffix, suffix_len + 1);
  return true;
}

static bool compute_include_path(const char *test_file, const char *output_dir_abs, char *out, size_t out_size) {
  char test_file_abs[CLUT_PATH_MAX];
  if (!path_get_absolute(test_file, test_file_abs, sizeof(test_file_abs)))
    return false;

  if (path_relative(output_dir_abs, test_file_abs, out, out_size))
    return true;

  size_t len = strlen(test_file_abs);
  if (len >= out_size)
    return false;

  memcpy(out, test_file_abs, len + 1);
  return true;
}

typedef struct {
  bool help;
  const char *output_file;
  const char **test_files;
  int test_files_count;
} CliArgs;

static void print_usage(const char *prog_name) {
  fprintf(stderr,
          "Usage: %s -o <output_file> <test_file> [test_file...]\n"
          "       %s -h | --help\n\n"
          "Generates a CLUT test runner at <output_file> from one or more test files.\n\n"
          "Options:\n"
          "  -o, --output <file>   Path of the generated runner (required)\n"
          "  -h, --help            Show this help message and exit\n",
          prog_name, prog_name);
}

static CliArgs parse_args(int argc, char **argv) {
  CliArgs args = {0};
  const char **test_files = malloc(sizeof(char *) * (size_t)argc);

  if (test_files == NULL) {
    fprintf(stderr, "error: failed to allocate memory for test file list\n");
    exit(1);
  }

  int test_files_count = 0;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      args.help = true;
      free(test_files);
      return args;
    }

    if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
      if (i + 1 >= argc) {
        fprintf(stderr, "error: '%s' requires an argument\n", argv[i]);
        print_usage(argv[0]);
        exit(1);
      }
      args.output_file = argv[++i];
      continue;
    }

    if (strcmp(argv[i], "--") == 0) {
      for (i++; i < argc; i++)
        test_files[test_files_count++] = argv[i];
      break;
    }

    if (argv[i][0] == '-' && argv[i][1] != '\0') {
      fprintf(stderr, "error: unknown option '%s'\n", argv[i]);
      print_usage(argv[0]);
      exit(1);
    }

    test_files[test_files_count++] = argv[i];
  }

  if (args.output_file == NULL) {
    fprintf(stderr, "error: missing required option -o <output_file>\n");
    print_usage(argv[0]);
    exit(1);
  }

  if (test_files_count == 0) {
    fprintf(stderr, "error: no test files provided\n");
    print_usage(argv[0]);
    exit(1);
  }

  args.test_files = test_files;
  args.test_files_count = test_files_count;
  return args;
}

static bool resolve_output_dir_abs(const char *output_file, char *out, size_t out_size) {
  char output_dir[CLUT_PATH_MAX];
  if (!path_dirname(output_file, output_dir, sizeof(output_dir))) {
    out[0] = '\0';
    return false;
  }

  if (!path_get_absolute(output_dir, out, out_size)) {
    out[0] = '\0';
    return false;
  }

  return true;
}

static void emit_include(FILE *out, const char *test_file, const char *output_dir_abs, bool have_output_dir_abs) {
  char include_path[CLUT_PATH_MAX];

  if (have_output_dir_abs && compute_include_path(test_file, output_dir_abs, include_path, sizeof(include_path))) {
    fprintf(out, "#include \"%s\"\n", include_path);
    return;
  }

  fprintf(out, "#include \"%s\"\n", test_file);
}

int main(int argc, char *argv[]) {
  CliArgs args = parse_args(argc, argv);

  if (args.help) {
    print_usage(argv[0]);
    return 0;
  }

  char temp_file_name[1024];
  int n = snprintf(temp_file_name, sizeof(temp_file_name), "%s.tmp", args.output_file);
  if (n < 0 || (size_t)n >= sizeof(temp_file_name)) {
    fprintf(stderr, "error: output path too long\n");
    free((void *)args.test_files);
    return 1;
  }

  FILE *out = fopen(temp_file_name, "wb");
  if (out == NULL) {
    fprintf(stderr, "error: could not open '%s' for writing\n", temp_file_name);
    free((void *)args.test_files);
    return 1;
  }

  char output_dir_abs[CLUT_PATH_MAX];
  bool have_output_dir_abs = resolve_output_dir_abs(args.output_file, output_dir_abs, sizeof(output_dir_abs));
  if (!have_output_dir_abs) {
    fprintf(stderr,
            "warning: could not resolve the absolute path of '%s'; "
            "#include paths will be emitted exactly as given on the command line\n",
            args.output_file);
  }

  fputs("#define CLUT_IMPLEMENTATION\n\n", out);

  for (int i = 0; i < args.test_files_count; ++i)
    emit_include(out, args.test_files[i], output_dir_abs, have_output_dir_abs);

  fputs("\nint main(void){\n", out);
  fputs("\tRUNNER_BEGIN();\n", out);

  bool had_parse_error = false;
  for (int i = 0; i < args.test_files_count; ++i) {
    if (parse_test_file(args.test_files[i], out) != 0)
      had_parse_error = true;
  }

  fputs("\treturn RUNNER_END();\n", out);
  fputs("}\n", out);

  if (fclose(out) != 0) {
    fprintf(stderr, "error: failed writing '%s'\n", temp_file_name);
    remove(temp_file_name);
    free((void *)args.test_files);
    return 1;
  }

  if (had_parse_error) {
    remove(temp_file_name);
    free((void *)args.test_files);
    return 1;
  }

#ifndef _WIN32
  if (rename(temp_file_name, args.output_file) != 0) {
    fprintf(stderr, "error: could not replace '%s'\n", args.output_file);
    remove(temp_file_name);
    free((void *)args.test_files);
    return 1;
  }
#else
  remove(args.output_file);
  if (rename(temp_file_name, args.output_file) != 0) {
    fprintf(stderr, "error: could not replace '%s'\n", args.output_file);
    free((void *)args.test_files);
    return 1;
  }
#endif

  free((void *)args.test_files);
  return 0;
}
