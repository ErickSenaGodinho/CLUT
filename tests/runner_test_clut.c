#define CLUT_IMPLEMENTATION

#include "test_basic.h"
#include "test_numeric.h"
#include "test_string.h"
#include "test_arrays.h"

int main(void){
	RUNNER_BEGIN();
	SUITE_BEGIN();
		RUN_TEST(test_boolean_and_basic);
		RUN_TEST(test_pointers_and_nullness);
		RUN_TEST(test_memory);
	SUITE_END();

	SUITE_BEGIN();
		RUN_TEST(test_char_comparisons);
		RUN_TEST(test_integer_comparisons);
		RUN_TEST(test_uint_comparisons);
		RUN_TEST(test_floating_point_numbers);
		RUN_TEST(test_floating_point_zero);
		RUN_TEST(test_floating_point_precision_accumulation);
		RUN_TEST(test_within_comparisons);
	SUITE_END();

	SUITE_BEGIN();
		RUN_TEST(test_strings);
	SUITE_END();

	SUITE_BEGIN();
		RUN_TEST(test_char_array);
		RUN_TEST(test_int_array);
		RUN_TEST(test_uint_array);
		RUN_TEST(test_float_array);
		RUN_TEST(test_double_array);
		RUN_TEST(test_string_array);
		RUN_TEST(test_within_char_array);
		RUN_TEST(test_within_int_array);
		RUN_TEST(test_within_uint_array);
		RUN_TEST(test_within_float_array);
		RUN_TEST(test_within_double_array);
	SUITE_END();

	return RUNNER_END();
}
