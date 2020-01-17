#include "dev.hpp"
#include "xorshift32_16bit_simd.h"
#include <chrono>

using namespace std;
int constexpr N = 1024;
int constexpr iterations = 1e5;

struct State
{
  uint16_t x = 1, y = 1;
};

void
test_int()
{
  cout << "int test\n\n";
  State states[4] = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
  uint16_t state[8] = { states[0].y, states[1].y, states[2].y, states[3].y,
                        states[0].x, states[1].x, states[2].x, states[3].x };
  int* buf = (int*)aligned_malloc(4 * N * sizeof(int));
  int* buf2 = (int*)aligned_malloc(4 * N * sizeof(int));
  auto start = chrono::high_resolution_clock::now();
  for (int j = 0; j < iterations; ++j) {
    xorshift32_16bit_simd_i4(state, buf, N);
  }
  auto end = chrono::high_resolution_clock::now();
  auto elapsed_time = chrono::duration_cast<chrono::nanoseconds>(end - start);
  cout << "simd time = " << (1.0 / iterations) * elapsed_time.count()
       << " ns\n";

  start = chrono::high_resolution_clock::now();
  for (int j = 0; j < iterations; ++j) {
    for (int c = 0; c < 4; ++c) {
      auto& s = states[c];
      for (int i = 0; i < 4 * N; i += 4) {
        uint16_t t = (s.x ^ (s.x << 5));
        s.x = s.y;
        s.y = (s.y ^ (s.y >> 1)) ^ (t ^ (t >> 3));
        buf2[i + c] = s.y;
      }
    }
  }
  end = chrono::high_resolution_clock::now();
  elapsed_time = chrono::duration_cast<chrono::nanoseconds>(end - start);
  cout << "ref time = " << (1.0 / iterations) * elapsed_time.count() << " ns\n";

  bool ok = true;
  for (int c = 0; c < 4; ++c) {
    for (int i = 0; i < 4 * N; i += 4) {
      if (buf[i + c] != buf2[i + c]) {
        ok = false;
      }
    }
  }
  if (ok) {
    cout << "test passed!\n\n";
  }
  else {
    cout << "test failed! results:\n";
    for (int c = 0; c < 4; ++c) {
      std::cout << "channel " << c << "\n\n";
      for (int i = 0; i < 4 * N; i += 4) {
        if (buf[i + c] != buf2[i + c]) {
          ok = false;
        }
        std::cout << "simd: " << buf[i + c] << "| ";
        std::cout << "ref: " << buf2[i + c] << "\n";
      }
      std::cout << "\n";
    }
  }

  aligned_free(buf);
  aligned_free(buf2);
}

void
test_float()
{
  cout << "float test\n\n";
  State states[4] = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
  uint16_t state[8] = { states[0].y, states[1].y, states[2].y, states[3].y,
                        states[0].x, states[1].x, states[2].x, states[3].x };
  float* buf = (float*)aligned_malloc(4 * N * sizeof(float));
  float* buf2 = (float*)aligned_malloc(4 * N * sizeof(float));
  auto start = chrono::high_resolution_clock::now();
  for (int j = 0; j < iterations; ++j) {
    xorshift32_16bit_simd_f4(state, buf, N);
  }
  auto end = chrono::high_resolution_clock::now();
  auto elapsed_time = chrono::duration_cast<chrono::nanoseconds>(end - start);
  cout << "simd time = " << (1.0 / iterations) * elapsed_time.count()
       << " ns\n";

  start = chrono::high_resolution_clock::now();
  for (int j = 0; j < iterations; ++j) {
    for (int c = 0; c < 4; ++c) {
      auto& s = states[c];
      for (int i = 0; i < 4 * N; i += 4) {
        uint16_t t = (s.x ^ (s.x << 5));
        s.x = s.y;
        s.y = (s.y ^ (s.y >> 1)) ^ (t ^ (t >> 3));
        buf2[i + c] = (2.0f / 65535.f) * s.y - 1.f;
      }
    }
  }
  end = chrono::high_resolution_clock::now();
  elapsed_time = chrono::duration_cast<chrono::nanoseconds>(end - start);
  cout << "ref time = " << (1.0 / iterations) * elapsed_time.count() << " ns\n";

  bool ok = true;
  for (int c = 0; c < 4; ++c) {
    for (int i = 0; i < 4 * N; i += 4) {
      if (buf[i + c] != buf2[i + c]) {
        ok = false;
      }
    }
  }
  if (ok) {
    cout << "test passed!\n\n";
  }
  else {
    cout << "test failed! results:\n";
    for (int c = 0; c < 4; ++c) {
      std::cout << "channel " << c << "\n\n";
      for (int i = 0; i < 4 * N; i += 4) {
        if (buf[i + c] != buf2[i + c]) {
          ok = false;
        }
        std::cout << "simd: " << buf[i + c] << "| ";
        std::cout << "ref: " << buf2[i + c] << "\n";
      }
      std::cout << "\n";
    }
  }

  aligned_free(buf);
  aligned_free(buf2);
}

int
main(int argc, char** argv)
{
  // dev();
  cout << "Testing generation of 4 interleaved channels of random numbers, "
          "each with "
       << N << " samples\n";
  cout << "Number of iterations " << iterations << "\n";
  test_int();
  test_float();
  return 0;
}