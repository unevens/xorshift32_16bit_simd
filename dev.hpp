#pragma once
#include <immintrin.h>
#include <iostream>
#include <stdint.h>

static inline void*
aligned_malloc(size_t nb_bytes)
{
  constexpr int ALIGNMENT = 64;
  void* ptr;
  void* storage = malloc(nb_bytes + ALIGNMENT);
  if (!storage) {
    return (void*)0;
  }
  ptr = (void*)(((size_t)storage + ALIGNMENT) & (~((size_t)(ALIGNMENT - 1))));
  *((void**)ptr - 1) = storage;
  return ptr;
}

static inline void
aligned_free(void* ptr)
{
  if (ptr) {
    free(*((void**)ptr - 1));
  }
}

static inline inline void
print(std::string txt, uint16_t ref, __m128i simd)
{
  uint16_t s[8];
  _mm_store_si128((__m128i*)s, simd);
  std::cout << txt << " ref =" << ref << "| simd = ";
  for (int i = 0; i < 8; ++i) {
    std::cout << s[i] << ", ";
  }
  std::cout << "\n";
}

static inline void
dev()
{
  using namespace std;
  int* buf = (int*)aligned_malloc(4 * sizeof(int));
  uint16_t state[8] = { 1, 2, 3, 4, 1, 2, 3, 4 }; // 4 y 4 x

  __m128i x = _mm_load_si128((__m128i const*)state);
  __m128i* xs = (__m128i*)buf;

  uint16_t x_ = 1, y_ = 1;

  for (int i = 0; i < 4; ++i) {
    cout << "iter " << i << "\n";
    print("state", y_, x);
    print("state", x_, x);

    uint16_t t_ = (x_ << 5);
    __m128i t = _mm_sll_epi16(x, _mm_set1_epi64x(5));
    print("t shift", t_, t);

    t_ = (x_ ^ t_);
    t = _mm_xor_si128(x, t);
    print("t xor", t_, t);

    t = _mm_shuffle_epi32(t, _MM_SHUFFLE(3, 2, 3, 2));
    print("get t shuffled ", t_, t);

    x_ = y_;
    x = _mm_shuffle_epi32(x, _MM_SHUFFLE(1, 0, 1, 0));
    print("x_ = y_", x_, x);

    y_ = (y_ ^ (y_ >> 1));
    __m128i y = _mm_xor_si128(x, _mm_srl_epi16(x, _mm_set1_epi64x(1)));
    print("y pass 1", y_, y);

    y_ = y_ ^ (t_ ^ (t_ >> 3));
    y =
      _mm_xor_si128(y, _mm_xor_si128(t, _mm_srl_epi16(t, _mm_set1_epi64x(3))));
    print("y pass 2", y_, y);

    /*store state*/
    {
      __m128 xf = _mm_castsi128_ps(x);
      __m128 yf = _mm_castsi128_ps(y);
      xf = _mm_shuffle_ps(yf, xf, _MM_SHUFFLE(3, 2, 1, 0));
      x = _mm_castps_si128(xf);
      print("store state x_", x_, x);
      print("store state y_", y_, x);
    }
    /*convert to int*/
    *xs = _mm_cvtepu16_epi32(x);
    std::cout << "result int32 ";
    for (int i = 0; i < 4; ++i) {
      std::cout << buf[i] << ", ";
    }
    std::cout << "\n\n";
  }
  aligned_free(buf);
}