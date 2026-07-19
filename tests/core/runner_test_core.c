#define CLUT_IMPLEMENTATION

#include "test_arrays.h"
#include "test_basic.h"
#include "test_common.h"
#include "test_numeric.h"
#include "test_string.h"

int main(void){
	RUNNER_BEGIN();
	SUITE_BEGIN();
		RUN_TEST(char_array);
		RUN_TEST(int_array);
		RUN_TEST(uint_array);
		RUN_TEST(float_array);
		RUN_TEST(double_array);
		RUN_TEST(string_array);
		RUN_TEST(within_char_array);
		RUN_TEST(within_int_array);
		RUN_TEST(within_uint_array);
		RUN_TEST(within_float_array);
		RUN_TEST(within_double_array);
	SUITE_END();

	SUITE_BEGIN();
		RUN_TEST(boolean_and_basic);
		RUN_TEST(pointers_and_nullness);
		RUN_TEST(memory);
	SUITE_END();

	SUITE_BEGIN();
		RUN_TEST(char_comparisons);
		RUN_TEST(integer_comparisons);
		RUN_TEST(uint_comparisons);
		RUN_TEST(floating_point_numbers);
		RUN_TEST(floating_point_zero);
		RUN_TEST(floating_point_precision_accumulation);
		RUN_TEST(within_comparisons);
	SUITE_END();

	SUITE_BEGIN();
		RUN_TEST(strings);
	SUITE_END();

	return RUNNER_END();
}
