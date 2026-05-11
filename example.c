#define CLUT_IMPLEMENTATION
#define CLUT_OUTPUT_COLOR

#include "clut.h"

void teste1() { TEST_ASSERT_FALSE(1 == 1); }
void teste2() { TEST_ASSERT_EQUALS_INT(1, 0); }
void teste3() { TEST_ASSERT_EQUALS_FLOAT_MESSAGE(0.3f, 0.2f + 0.1f, "Must be 0.3f"); }
void teste4() {
  TEST_ASSERT_EQUALS_STRING("Teste", "Teste");
  TEST_ASSERT_NULL(NULL);
  TEST_ASSERT_NOT_NULL(NULL);
}

int main() {
  TEST_BEGIN();
  TEST_RUN(teste1);
  TEST_RUN(teste2);
  TEST_RUN(teste3);
  TEST_RUN(teste4);
  return TEST_END();
}
