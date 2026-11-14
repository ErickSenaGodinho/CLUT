#define CLUT_IMPLEMENTATION

#include "test_common.h"
#include "test_lexer.h"
#include "test_parser.h"
#include "test_path_utilities.h"
#include "test_string_list.h"
#include "test_wildcard_expansion.h"
#include "test_wildcard_match.h"

int main(void){
	RUNNER_BEGIN();

	SUITE_BEGIN();
		RUN_TEST(plain_symbol);
		RUN_TEST(underscore_symbol);
		RUN_TEST(empty_input_returns_end);
		RUN_TEST(trailing_ws_returns_end);
		RUN_TEST(unknown_char_returns_token);
		RUN_TEST(string_literal_quotes);
		RUN_TEST(string_with_test_macro_text);
		RUN_TEST(line_comment);
		RUN_TEST(block_comment_multiline);
		RUN_TEST(directive_is_single_token);
		RUN_TEST(parens_braces_order);
		RUN_TEST(nested_braces_track_depth);
		RUN_TEST(test_macro_detected_top_level);
		RUN_TEST(param_and_repeated_test_macros);
		RUN_TEST(identifier_prefix_not_keyword);
		RUN_TEST(all_hook_macros);
		RUN_TEST(keywords_inside_braces_ignored);
		RUN_TEST(quoted_literal_is_single_token_and_keeps_depth);
		RUN_TEST(unterminated_string_stops_at_newline);
		RUN_TEST(stray_apostrophe_only_costs_one_line);
		RUN_TEST(digit_separator_is_not_a_char_literal);
		RUN_TEST(unterminated_block_comment_does_not_overrun);
		RUN_TEST(line_comment_at_eof_without_newline_does_not_overrun);
		RUN_TEST(directive_at_eof_without_newline_does_not_overrun);
		RUN_TEST(unterminated_string_at_eof_does_not_overrun);
		RUN_TEST(directive_continuation_swallows_next_line);
		RUN_TEST(directive_continuation_with_crlf);
		RUN_TEST(stray_close_brace_does_not_underflow_depth);
		RUN_TEST(reporter_macro_detected_top_level);
		RUN_TEST(reporter_lookalikes_are_symbols);
		RUN_TEST(reporter_inside_braces_ignored);
	SUITE_END();

	SUITE_BEGIN();
		USE_HOOK_AFTER_ALL(parser_fixture_teardown);

		RUN_TEST(get_macro_set_hook_maps_all_known_hooks);
		RUN_TEST(get_macro_set_hook_returns_null_for_non_hook_kind);
		RUN_TEST(parse_registers_tests_after_char_and_string_literals);
		RUN_TEST(parse_registers_reporter_with_designated_initializers);
		RUN_TEST(parse_ignores_reporter_inside_function_body);
		RUN_TEST(parse_collects_hooks_and_tests_separately);
		RUN_TEST(parse_malformed_macro_returns_error);
		RUN_TEST(parse_missing_file_returns_error);
	SUITE_END();

	SUITE_BEGIN();
		RUN_TEST(path_is_separator_slash);
		RUN_TEST(path_is_absolute_root);
		RUN_TEST(path_normalize_separators_backslashes);
		RUN_TEST(path_basename_nested);
		RUN_TEST(path_basename_no_separator);
		RUN_TEST(path_basename_trailing_slash);
		RUN_TEST(path_basename_root_file);
		RUN_TEST(path_dirname_nested);
		RUN_TEST(path_dirname_no_separator);
		RUN_TEST(path_dirname_root);
		RUN_TEST(path_collapse_dots_basic);
		RUN_TEST(path_collapse_dots_past_root);
		RUN_TEST(path_same_root_absolute);
		RUN_TEST(path_same_root_mixed);
		RUN_TEST(path_common_boundary_shared_prefix);
		RUN_TEST(path_common_boundary_identical);
		RUN_TEST(path_count_remaining_segments_basic);
		RUN_TEST(path_relative_nested);
		RUN_TEST(path_relative_same_directory);
		RUN_TEST(path_relative_different_roots);
	SUITE_END();

	SUITE_BEGIN();
		RUN_TEST(str_dup_copies_content);
		RUN_TEST(str_dup_independent_buffer);
		RUN_TEST(string_list_append_single);
		RUN_TEST(string_list_append_many_preserves_order);
		RUN_TEST(string_list_append_grows_past_init_cap);
		RUN_TEST(string_list_free_resets_state);
		RUN_TEST(string_list_free_empty_list_is_safe);
	SUITE_END();

	SUITE_BEGIN();
		USE_HOOK_BEFORE_ALL(wildcard_expansion_setup);
		USE_HOOK_AFTER_ALL(wildcard_expansion_teardown);

		RUN_TEST(list_directory_lists_all_entries);
		RUN_TEST(list_directory_missing_dir_fails);
		RUN_TEST(expand_wildcard_matches_subset);
		RUN_TEST(expand_wildcard_matches_all_with_star);
		RUN_TEST(expand_wildcard_no_match_appends_nothing);
	SUITE_END();

	SUITE_BEGIN();
		RUN_TEST(wildcard_match_exact);
		RUN_TEST(wildcard_match_star_matches_all);
		RUN_TEST(wildcard_match_star_suffix);
		RUN_TEST(wildcard_match_star_prefix);
		RUN_TEST(wildcard_match_star_middle);
		RUN_TEST(wildcard_match_question_mark);
		RUN_TEST(wildcard_match_empty_pattern_empty_string);
		RUN_TEST(wildcard_match_empty_pattern_nonempty_string);
		RUN_TEST(wildcard_match_pattern_longer_than_string);
		RUN_TEST(wildcard_match_multiple_stars);
	SUITE_END();

	return RUNNER_END();
}
