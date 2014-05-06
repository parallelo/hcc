// RUN: %amp_device -D__GPU__ %s -m32 -emit-llvm -c -S -O2 -o %t.ll && mkdir -p %t
// RUN: %clamp-device %t.ll %t/kernel.cl
// RUN: pushd %t && %embed_kernel kernel.cl %t/kernel.o && popd
// RUN: %cxxamp %link %t/kernel.o %s -o %t.out && %t.out
#include <amp.h>
#include <stdlib.h>
#include <iostream>
#include <amp_math.h>

using namespace concurrency;

int main(void) {
  const int vecSize = 1000;

  // Alloc & init input data
  extent<1> e(vecSize);
  array<float, 1> a(vecSize);
  array<float, 1> b(vecSize);
  array<float, 1> c(vecSize);
  array<float, 1> d(vecSize);

  for (index<1> i(0); i[0] < vecSize; i++) {
    a[i] = rand() / 1000.0f;
    b[i] = rand() / 1000.0f;
  }

  array_view<float> ga(a);
  array_view<float> gb(b);
  array_view<float> gc(c);
  array_view<float> gd(d);
  array_view<float> ge(e);

  parallel_for_each(
    e,
    [=](index<1> idx) restrict(amp) {
          gd[idx] = precise_math::fmaf(ga[idx], gb[idx], gc[idx]);
  });

  for(unsigned i = 0; i < vecSize; i++) {
    ge[i] = precise_math::fmaf(ga[i], gb[i], gc[i]);
  }

  float sum = 0;
  for(unsigned i = 0; i < vecSize; i++) {
    sum += fast_math::fabs(fast_math::fabs(ge[i]) - fast_math::fabs(gd[i]));
  }
  return (sum > 0.1f);
}