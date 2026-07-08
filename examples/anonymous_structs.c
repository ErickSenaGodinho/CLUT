#define CLUT_IMPLEMENTATION
#include "../clut.h"

typedef struct {
  float x;
  float y;
} Vec2;

typedef struct {
  Vec2 a;
  Vec2 b;
  float expected;
} Vec2DotCase;

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

PARAM_TEST(vec2_dot_cases, Vec2DotCase,
           {
               {{2.0f, 3.0f}, {4.0f, 5.0f}, 23.0f},
               {{1.0f, 0.0f}, {0.0f, 1.0f}, 0.0f},
               {{1.0f, 0.0f}, {1.0f, 0.0f}, 1.0f},
               {{-1.0f, -1.0f}, {1.0f, 1.0f}, -2.0f},
           }) {
  float result = vec2_dot(input.a, input.b);
  TEST_ASSERT_EQUAL_FLOAT(input.expected, result);
}

int main(void) {
  RUNNER_BEGIN();
  SUITE_BEGIN();
  RUN_TEST(vec2_add);
  RUN_TEST(vec2_dot);
  RUN_TEST(vec2_fields);
  RUN_TEST(vec2_dot_cases);
  SUITE_END();
  return RUNNER_END();
}
