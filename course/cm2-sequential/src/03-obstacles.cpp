// Sessions 2 and 3 - Sequential code optimization
// 03-obstacles.cpp: four things that the compiler cannot do for us, each measured
// before and after the programmer does it: a division by the same number, two
// results written through pointers, a power, a call that it cannot see into.
//   Compile and run it on the lab server (MyDocker), or on a PC with an Intel
//   or AMD processor:
//     g++ -O2 -fno-tree-vectorize 03-obstacles.cpp -o 03-obstacles && ./03-obstacles
//   To read its instructions: Compiler Explorer (https://godbolt.org), compiler
//   x86-64 gcc 16.2, with the same options.
#include <algorithm> // std::min
#include <chrono>    // timing
#include <cmath>     // std::pow
#include <cpuid.h>   // name of the processor
#include <cstdio>    // printf
#include <vector>    // std::vector

const int N             = 1000;  // elements per array: 8 KB, in the L1 cache
const int NREPS         = 20000; // calls of the function in one measurement
const int NMEASUREMENTS = 5;     // measurements; the smallest time is kept

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

// The frequency of the core, in GHz: a chain of COUNT multiplications of
// integers, 3 cycles each (see 01-dot-sums.cpp).
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

// Executes work() NREPS times in a row, NMEASUREMENTS times, keeps the smallest time,
// and returns it in cycles per element (see 02-filter.cpp).
template <class Work> double cycles_per_element(Work work, double ghz)
{
  double best = 1e30;
  for (int t = 0; t < NMEASUREMENTS; t++) {
    double t0 = now();
    for (int r = 0; r < NREPS; r++)
      work();
    double elapsed = now() - t0;
    best = std::min(best, elapsed);
  }
  return best * ghz * 1e9 / ((double) NREPS * N);
}

// ---- 1. a division by the same number at each iteration
__attribute__((noinline, noipa))
void scale_div(const double *a, double *b, int n, double d)
{
  for (int i = 0; i < n; i++)
    b[i] = a[i] / d;
}

__attribute__((noinline, noipa))
void scale_mul(const double *a, double *b, int n, double d)
{
  double inverse = 1.0 / d; // one division, before the loop
  for (int i = 0; i < n; i++)
    b[i] = a[i] * inverse;
}

// ---- 2. two results written through two pointers
__attribute__((noinline, noipa))
void sums_pointers(const double *a, int n, double *sum, double *squares)
{
  *sum = 0.0;
  *squares = 0.0;
  for (int i = 0; i < n; i++) {
    *sum = *sum + a[i];
    *squares = *squares + a[i] * a[i];
  }
}

__attribute__((noinline, noipa))
void sums_locals(const double *a, int n, double *sum, double *squares)
{
  double s = 0.0, q = 0.0; // two local variables
  for (int i = 0; i < n; i++) {
    s = s + a[i];
    q = q + a[i] * a[i];
  }
  *sum = s; // written once, at the end
  *squares = q;
}

// ---- 3. a power
__attribute__((noinline, noipa))
void cube_pow(const double *a, double *b, int n)
{
  for (int i = 0; i < n; i++)
    b[i] = std::pow(a[i], 3.0);
}

__attribute__((noinline, noipa))
void cube_mul(const double *a, double *b, int n)
{
  for (int i = 0; i < n; i++)
    b[i] = a[i] * a[i] * a[i];
}

// ---- 4. a call that the compiler cannot see into
// f_hidden stands for a function that is defined in another file: "noinline,
// noipa" hide its body from the loop that calls it.
__attribute__((noinline, noipa))
double f_hidden(double x) { return 2.0 * x + 1.0; }
inline double f_visible(double x) { return 2.0 * x + 1.0; }

__attribute__((noinline, noipa))
void map_hidden(const double *a, double *b, int n)
{
  for (int i = 0; i < n; i++)
    b[i] = f_hidden(a[i]);
}

__attribute__((noinline, noipa))
void map_visible(const double *a, double *b, int n)
{
  for (int i = 0; i < n; i++)
    b[i] = f_visible(a[i]);
}

int main()
{
  print_processor();
  double ghz = frequency();
  printf("frequency: %.2f GHz\n", ghz);

  // numbers between 1 and 2, without any pattern
  std::vector<double> va(N), vb(N);
  double *a = va.data(), *b = vb.data();
  unsigned int seed = 2026;
  for (int i = 0; i < N; i++) {
    seed = seed * 1664525u + 1013904223u;
    a[i] = 1.0 + (seed >> 8) / 16777216.0;
  }
  double d = 3.0, sum = 0.0, squares = 0.0;

  double c;
  printf("cycles per element:\n");
  c = cycles_per_element([&] { scale_div(a, b, N, d); }, ghz);
  printf("  b[i] = a[i] / d                    %6.2f   (b[0] = %.17g)\n", c, b[0]);
  c = cycles_per_element([&] { scale_mul(a, b, N, d); }, ghz);
  printf("  b[i] = a[i] * (1 / d)              %6.2f   (b[0] = %.17g)\n", c, b[0]);
  c = cycles_per_element([&] { sums_pointers(a, N, &sum, &squares); }, ghz);
  printf("  two sums through two pointers      %6.2f   (%.6f, %.6f)\n", c, sum, squares);
  c = cycles_per_element([&] { sums_locals(a, N, &sum, &squares); }, ghz);
  printf("  two sums in local variables        %6.2f   (%.6f, %.6f)\n", c, sum, squares);
  c = cycles_per_element([&] { cube_pow(a, b, N); }, ghz);
  printf("  b[i] = pow(a[i], 3.0)              %6.2f   (b[0] = %.17g)\n", c, b[0]);
  c = cycles_per_element([&] { cube_mul(a, b, N); }, ghz);
  printf("  b[i] = a[i] * a[i] * a[i]          %6.2f   (b[0] = %.17g)\n", c, b[0]);
  c = cycles_per_element([&] { map_hidden(a, b, N); }, ghz);
  printf("  b[i] = f(a[i]), f in another file  %6.2f\n", c);
  c = cycles_per_element([&] { map_visible(a, b, N); }, ghz);
  printf("  b[i] = f(a[i]), f visible          %6.2f\n", c);
  return 0;
}
