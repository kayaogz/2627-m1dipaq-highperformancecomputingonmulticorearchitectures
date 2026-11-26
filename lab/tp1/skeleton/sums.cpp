// =====================================================================
//  Lab 1 - Sequential code optimization
//  Exercise 3: Several sums  --  SKELETON
//
//  Fill in the parts marked  TODO  in the order of the questions.
//
//  Run it on the MyDocker server, in its terminal:
//    g++ -O2 -fno-tree-vectorize sums.cpp -o sums && ./sums
//  Read its instructions on Compiler Explorer (https://godbolt.org):
//  compiler x86-64 gcc 16.2, the same options. The server has g++ 16
//  too: the instructions are almost the same.
// =====================================================================

// =====================================================================
//  ANSWERS to the questions of the assignment (write them here, as
//  comments; this is the only file you submit for this exercise).
//
//  (c) Cycles per element and speedups with 1, 2, 4 and 8 sums. How do
//      you explain each number? Why are 8 sums not faster than 4?
//
//  (d) Are the four results identical? Why? Why does the compiler not
//      create the sums itself?
//
//  (e) Without -fno-tree-vectorize: which versions change speed? Which
//      instructions replace mulsd and addsd in dot8?
//
//  (f) The sum of an array: how many sums, how many cycles per element,
//      on this server and on Zen 2?
//
// =====================================================================

#include <algorithm> // std::min
#include <chrono>    // timing
#include <cpuid.h>   // name of the processor
#include <cstdio>    // printf
#include <vector>    // std::vector

const int NREPS         = 100000; // calls of the function per measurement
const int NMEASUREMENTS = 5;      // number of measurements

// ---------------------------------------------------------------------
//  Timing helper (provided): returns the current time in seconds.
// ---------------------------------------------------------------------
double now()
{
  using namespace std::chrono;
  return duration<double>(steady_clock::now().time_since_epoch()).count();
}

// ---------------------------------------------------------------------
//  Name of the processor (provided).
// ---------------------------------------------------------------------
void print_processor()
{
  unsigned int r[13] = {0};
  for (unsigned int k = 0; k < 3; k++)
    __get_cpuid(0x80000002 + k, &r[4 * k], &r[4 * k + 1], &r[4 * k + 2],
                &r[4 * k + 3]);
  printf("processor: %s\n", (const char *) r);
}

// ---------------------------------------------------------------------
//  Frequency of the core, in GHz (provided): a chain of COUNT
//  multiplications of integers, 3 cycles each (see exercise 2).
// ---------------------------------------------------------------------
double frequency()
{
  const long COUNT = 50000000;
  double best = 1e30;
  for (int t = 0; t < 3; t++) {
    long x = 1, three = 3, count = COUNT;
    double t0 = now();
    __asm__ volatile("1: imul %[three], %[x]\n dec %[count]\n jnz 1b"
                     : [x] "+r"(x), [count] "+r"(count)
                     : [three] "r"(three) : "cc");
    double elapsed = now() - t0;
    best = std::min(best, elapsed);
  }
  return 3.0 * COUNT / best / 1e9;
}

// ---------------------------------------------------------------------
//  Measurement (provided): what you wrote in exercise 2, for any piece
//  of code. Executes work() NREPS times in a row, NMEASUREMENTS times,
//  keeps the smallest duration, and returns it in cycles per element.
//  Usage:  cycles_per_element([&] { result = dot1(a, b, n); }, n, ghz)
// ---------------------------------------------------------------------
template <class Work>
double cycles_per_element(Work work, int elements, double ghz)
{
  double best = 1e30;
  for (int t = 0; t < NMEASUREMENTS; t++) {
    double t0 = now();
    for (int r = 0; r < NREPS; r++)
      work();
    double elapsed = now() - t0;
    best = std::min(best, elapsed);
  }
  return best * ghz * 1e9 / ((double) NREPS * elements);
}

// ---------------------------------------------------------------------
//  The dot product with one sum (provided).
// ---------------------------------------------------------------------
__attribute__((noinline, noipa))
double dot1(const double *a, const double *b, int n)
{
  double s = 0.0;
  for (int i = 0; i < n; i++)
    s = s + a[i] * b[i];
  return s;
}

// ---------------------------------------------------------------------
//  Question (a): the dot product with two sums, for any n.
//  s0 receives the products of the elements 0, 2, 4, ... and s1 those
//  of the elements 1, 3, 5, ... The result must also be right when n
//  is odd: the program checks it on 1003 elements.
// ---------------------------------------------------------------------
__attribute__((noinline, noipa))
double dot2(const double *a, const double *b, int n)
{
  // TODO (a)
  return 0.0;
}

// ---------------------------------------------------------------------
//  Question (b): the dot product with four sums, then with eight sums,
//  for any n.
// ---------------------------------------------------------------------
__attribute__((noinline, noipa))
double dot4(const double *a, const double *b, int n)
{
  // TODO (b)
  return 0.0;
}

__attribute__((noinline, noipa))
double dot8(const double *a, const double *b, int n)
{
  // TODO (b)
  return 0.0;
}

// ---------------------------------------------------------------------
//  Helper (provided): compares the result of a version with the result
//  of dot1, and prints how much they differ.
// ---------------------------------------------------------------------
void check(const char *name, double val, double ref)
{
  double difference = (val - ref) / ref;
  if (difference < 0.0) difference = -difference;
  if (val == 0.0)
    printf("  [%s] result 0: function not written yet?\n", name);
  else if (difference < 1e-12)
    printf("  [%s] OK       (relative difference %.1e)\n", name,
           difference);
  else
    printf("  [%s] WRONG    (%.17g instead of %.17g)\n", name, val, ref);
}

int main()
{
  const int N = 1000; // elements per array: 8 KB, in the L1 cache
  const int M = 1003; // not a multiple of 2, 4 or 8

  print_processor();
  double ghz = frequency();
  printf("frequency: %.2f GHz\n", ghz);

  // (provided) numbers between 0 and 1, without any pattern
  std::vector<double> va(M), vb(M);
  double *a = va.data(), *b = vb.data();
  unsigned int seed = 2026;
  for (int i = 0; i < M; i++) {
    seed = seed * 1664525u + 1013904223u;
    a[i] = (seed >> 8) / 16777216.0;
    seed = seed * 1664525u + 1013904223u;
    b[i] = (seed >> 8) / 16777216.0;
  }

  // (provided) are the versions correct, on N and on M elements?
  printf("N = %d\n", N);
  check("dot2", dot2(a, b, N), dot1(a, b, N));
  check("dot4", dot4(a, b, N), dot1(a, b, N));
  check("dot8", dot8(a, b, N), dot1(a, b, N));
  printf("M = %d\n", M);
  check("dot2", dot2(a, b, M), dot1(a, b, M));
  check("dot4", dot4(a, b, M), dot1(a, b, M));
  check("dot8", dot8(a, b, M), dot1(a, b, M));

  // (provided) the measurements of questions (c), (d) and (e)
  double result = 0.0, c;
  printf("cycles per element, N = %d:\n", N);
  c = cycles_per_element([&] { result = dot1(a, b, N); }, N, ghz);
  printf("  1 sum : %5.2f   result %.17g\n", c, result);
  c = cycles_per_element([&] { result = dot2(a, b, N); }, N, ghz);
  printf("  2 sums: %5.2f   result %.17g\n", c, result);
  c = cycles_per_element([&] { result = dot4(a, b, N); }, N, ghz);
  printf("  4 sums: %5.2f   result %.17g\n", c, result);
  c = cycles_per_element([&] { result = dot8(a, b, N); }, N, ghz);
  printf("  8 sums: %5.2f   result %.17g\n", c, result);

  return 0;
}
