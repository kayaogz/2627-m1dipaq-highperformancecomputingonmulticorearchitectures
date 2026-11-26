// Sessions 2 and 3 - Sequential code optimization
// 01-dot-sums.cpp: the dot product with one sum and with three sums, in cycles.
//   Compile and run it on the lab server (MyDocker), or on a PC with an Intel
//   or AMD processor:
//     g++ -O2 -fno-tree-vectorize 01-dot-sums.cpp -o 01-dot-sums && ./01-dot-sums
//   To read its instructions: Compiler Explorer (https://godbolt.org), compiler
//   x86-64 gcc 16.2, with the same options.
#include <algorithm> // std::min
#include <chrono>    // timing
#include <cpuid.h>   // name of the processor
#include <cstdio>    // printf
#include <vector>    // std::vector

const int N             = 1200;   // elements per array, a multiple of 3: 9.6 KB, in L1
const int NREPS         = 100000; // calls of the function in one measurement
const int NMEASUREMENTS = 5;      // measurements; the smallest time is kept

// The current time, in seconds.
double now()
{
  using namespace std::chrono;
  return duration<double>(steady_clock::now().time_since_epoch()).count();
}

// The name of the processor that runs the program.
void print_processor()
{
  unsigned int r[13] = {0};
  for (unsigned int k = 0; k < 3; k++)
    __get_cpuid(0x80000002 + k, &r[4 * k], &r[4 * k + 1], &r[4 * k + 2], &r[4 * k + 3]);
  printf("processor: %s\n", (const char *) r);
}

// The frequency of the core, in GHz. The loop in assembly multiplies x by 3,
// COUNT times; each multiplication needs the result of the previous one, and a
// multiplication of integers takes 3 cycles: the loop lasts 3 * COUNT cycles.
double frequency()
{
  const long COUNT = 50000000;
  double best = 1e30;
  for (int t = 0; t < 3; t++) {
    long x = 1, three = 3, count = COUNT;
    double t0 = now();
    __asm__ volatile("1: imul %[three], %[x]\n dec %[count]\n jnz 1b"
                     : [x] "+r"(x), [count] "+r"(count) : [three] "r"(three) : "cc");
    double elapsed = now() - t0;
    best = std::min(best, elapsed);
  }
  return 3.0 * COUNT / best / 1e9;
}

// The dot product of chapter 1: one sum.
// "noinline, noipa": the compiler must really call the function each time (it
// has no side effect, so the compiler could otherwise call it once for all).
__attribute__((noinline, noipa))
double dot1(const double *a, const double *b, int n)
{
  double s = 0.0;
  for (int i = 0; i < n; i++)
    s = s + a[i] * b[i];
  return s;
}

// The same with three sums; n is a multiple of 3 here.
__attribute__((noinline, noipa))
double dot3(const double *a, const double *b, int n)
{
  double s0 = 0.0, s1 = 0.0, s2 = 0.0;
  for (int i = 0; i < n; i += 3) {
    s0 = s0 + a[i] * b[i];
    s1 = s1 + a[i + 1] * b[i + 1];
    s2 = s2 + a[i + 2] * b[i + 2];
  }
  return s0 + s1 + s2;
}

// Measures one version: NREPS calls in a row, NMEASUREMENTS times; the smallest time is
// converted into nanoseconds per element, then into cycles per element.
void measure(const char *name,
             double (*dot)(const double *, const double *, int),
             const double *a, const double *b, double ghz)
{
  double best = 1e30, result = 0.0;
  for (int t = 0; t < NMEASUREMENTS; t++) {
    double t0 = now();
    for (int r = 0; r < NREPS; r++)
      result = dot(a, b, N);
    double elapsed = now() - t0;
    best = std::min(best, elapsed);
  }
  // the time per element, in nanoseconds, then in cycles
  double ns_elem = best * 1e9 / ((double) NREPS * N);
  double cycles_elem = ns_elem * ghz; // GHz: cycles/ns
  printf("%s: %.4f s, %.3f ns and %.2f cycles per element, result %.15f\n",
         name, best, ns_elem, cycles_elem, result);
}

int main()
{
  print_processor();
  double ghz = frequency();
  printf("frequency: %.2f GHz\n", ghz);

  // numbers between 0 and 1, without any pattern
  std::vector<double> a(N), b(N);
  unsigned int seed = 2026;
  for (int i = 0; i < N; i++) {
    seed = seed * 1664525u + 1013904223u;
    a[i] = (seed >> 8) / 16777216.0;
    seed = seed * 1664525u + 1013904223u;
    b[i] = (seed >> 8) / 16777216.0;
  }

  measure("one sum   ", dot1, a.data(), b.data(), ghz);
  measure("three sums", dot3, a.data(), b.data(), ghz);
  return 0;
}
