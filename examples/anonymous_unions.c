#define CLUT_IMPLEMENTATION
#include "../clut.h"

typedef union {
  struct {
    float x, y, z, w;
  };
  struct {
    float r, g, b, a;
  };
} Vec4;

Vec4 vec4_add(Vec4 a, Vec4 b) { return (Vec4){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z, .w = a.w + b.w}; }
Vec4 vec4_scale(Vec4 v, float s) { return (Vec4){.x = v.x * s, .y = v.y * s, .z = v.z * s, .w = v.w * s}; }

TEST(vec4_add_as_position) {
  Vec4 a = {.x = 1.0f, .y = 2.0f, .z = 3.0f, .w = 1.0f};
  Vec4 b = {.x = 4.0f, .y = 5.0f, .z = 6.0f, .w = 1.0f};
  Vec4 result = vec4_add(a, b);
  TEST_ASSERT_EQUAL_FLOAT(5.0f, result.x);
  TEST_ASSERT_EQUAL_FLOAT(7.0f, result.y);
  TEST_ASSERT_EQUAL_FLOAT(9.0f, result.z);
  TEST_ASSERT_EQUAL_FLOAT(2.0f, result.w);
}

TEST(vec4_scale_as_color) {
  Vec4 color = {.r = 1.0f, .g = 0.5f, .b = 0.25f, .a = 1.0f};
  Vec4 result = vec4_scale(color, 0.5f);
  TEST_ASSERT_EQUAL_FLOAT(0.5f, result.r);
  TEST_ASSERT_EQUAL_FLOAT(0.25f, result.g);
  TEST_ASSERT_EQUAL_FLOAT(0.125f, result.b);
  TEST_ASSERT_EQUAL_FLOAT(0.5f, result.a);
}

TEST(vec4_shared_memory) {
  Vec4 v = {.r = 0.2f, .g = 0.4f, .b = 0.6f, .a = 1.0f};
  TEST_ASSERT_EQUAL_FLOAT(v.r, v.x);
  TEST_ASSERT_EQUAL_FLOAT(v.g, v.y);
  TEST_ASSERT_EQUAL_FLOAT(v.b, v.z);
  TEST_ASSERT_EQUAL_FLOAT(v.a, v.w);
}

int main(void) {
  RUNNER_BEGIN();
  SUITE_BEGIN();
  RUN_TEST(vec4_add_as_position);
  RUN_TEST(vec4_scale_as_color);
  RUN_TEST(vec4_shared_memory);
  SUITE_END();
  return RUNNER_END();
}
