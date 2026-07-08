#define CLUT_IMPLEMENTATION
#include "../clut.h"

typedef struct {
  float x;
  float y;
} Vec2;

Vec2 vec2_add(Vec2 a, Vec2 b) { return (Vec2){a.x + b.x, a.y + b.y}; }
float vec2_dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }

TEST(vec2_add) {
  Vec2 result = vec2_add((Vec2){2.0f, 3.0f}, (Vec2){4.0f, 5.0f});
  TEST_ASSERT_EQUAL_FLOAT(6.0f, result.x);
  TEST_ASSERT_EQUAL_FLOAT(8.0f, result.y);
}

TEST(vec2_dot) {
  float result = vec2_dot((Vec2){2.0f, 3.0f}, (Vec2){4.0f, 5.0f});
  TEST_ASSERT_EQUAL_FLOAT(23.0f, result);
}

TEST(vec2_fields) {
  Vec2 expected = {6.0f, 8.0f};
  Vec2 result = vec2_add((Vec2){2.0f, 3.0f}, (Vec2){4.0f, 5.0f});
  TEST_ASSERT_EQUAL_FLOAT(expected.x, result.x);
  TEST_ASSERT_EQUAL_FLOAT(expected.y, result.y);
}

int main(void) {
  RUNNER_BEGIN();
  SUITE_BEGIN();
  RUN_TEST(vec2_add);
  RUN_TEST(vec2_dot);
  RUN_TEST(vec2_fields);
  SUITE_END();
  return RUNNER_END();
}
