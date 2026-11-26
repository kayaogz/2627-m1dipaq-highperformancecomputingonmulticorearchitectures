// =====================================================================
//  Lab 1 - Sequential code optimization
//  Exercise 4: Branches  --  SKELETON
//
//  Fill in the parts marked  TODO  in the order of the questions.
//
//  Run it on the MyDocker server, in its terminal:
//    g++ -O2 -fno-tree-vectorize split.cpp -o split && ./split
//  Read its instructions on Compiler Explorer (https://godbolt.org):
//  compiler x86-64 gcc 16.2, the same options. The server has g++ 16
//  too: the instructions are almost the same.
// =====================================================================

// =====================================================================
//  ANSWERS to the questions of the assignment (write them here, as
//  comments; this is the only file you submit for this exercise).
//
//  (a) Cycles per element of split_branch on sorted and on random data.
//      Why the difference? What does one wrong prediction cost?
//
//  (c) The four measurements. Which version for sorted data, for random
//      data, for unknown data? What replaces the conditional jump?
//
//  (d) Cycles per element on the 1000 random elements. Why?
//
// =====================================================================

#include <algorithm> // std::sort, std::min
#include <chrono>    // timing
#include <cpuid.h>   // name of the processor
#include <cstdio>    // printf
#include <vector>    // std::vector

const int NMEASUREMENTS = 5; // number of measurements

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
//  Measurement (provided): executes work() nreps times in a row,
//  NMEASUREMENTS times, keeps the smallest duration, and returns it in
//  cycles per element.
// ---------------------------------------------------------------------
template <class Work>
double cycles_per_element(Work work, int nreps, int elements, double ghz)
{
  double best = 1e30;
  for (int t = 0; t < NMEASUREMENTS; t++) {
    double t0 = now();
    for (int r = 0; r < nreps; r++)
      work();
    double elapsed = now() - t0;
    best = std::min(best, elapsed);
  }
  return best * ghz * 1e9 / ((double) nreps * elements);
}

// ---------------------------------------------------------------------
//  Splits the n elements of a (provided): those smaller than t are
//  copied to lo, the others to hi, in the order in which they come.
//  Returns the number of elements copied to lo.
// ---------------------------------------------------------------------
__attribute__((noinline, noipa))
int split_branch(const int *a, int n, int t, int *lo, int *hi)
{
  int nlo = 0, nhi = 0;
  for (int i = 0; i < n; i++) {
    if (a[i] < t)
      lo[nlo++] = a[i];
    else
      hi[nhi++] = a[i];
  }
  return nlo;
}

// ---------------------------------------------------------------------
//  Question (b): the same result (the same elements in lo and in hi,
//  in the same order, and the same number returned), without any "if"
//  in the loop. lo and hi each have room for n elements: the function
//  may write more elements than it keeps.
// ---------------------------------------------------------------------
__attribute__((noinline, noipa))
int split_nobranch(const int *a, int n, int t, int *lo, int *hi)
{
  // TODO (b)
  return 0;
}

// ---------------------------------------------------------------------
//  Helper (provided): compares the arrays produced by split_nobranch
//  with those of split_branch.
// ---------------------------------------------------------------------
void check(const int *a, int n, int t)
{
  std::vector<int> lo1(n), hi1(n), lo2(n), hi2(n);
  int n1 = split_branch(a, n, t, lo1.data(), hi1.data());
  int n2 = split_nobranch(a, n, t, lo2.data(), hi2.data());
  bool same = (n1 == n2);
  for (int i = 0; same && i < n1; i++) same = (lo1[i] == lo2[i]);
  for (int i = 0; same && i < n - n1; i++) same = (hi1[i] == hi2[i]);
  if (n2 == 0)
    printf("[split_nobranch] nothing in lo: function not written yet?\n");
  else
    printf("[split_nobranch] %s\n", same ? "OK" : "WRONG");
}

int main()
{
  const int N     = 100000, NREPS = 100; // the long arrays
  const int SHORT = 1000;                // question (d)
  const int T     = 128;                 // the threshold

  print_processor();
  double ghz = frequency();
  printf("frequency: %.2f GHz\n", ghz);

  // (provided) numbers between 0 and 255 in random order, and the same
  // numbers sorted
  std::vector<int> vrandom(N), vsorted(N), vlo(N), vhi(N);
  int *random = vrandom.data(), *sorted = vsorted.data();
  int *lo = vlo.data(), *hi = vhi.data();
  unsigned int seed = 2026;
  for (int i = 0; i < N; i++) {
    seed = seed * 1664525u + 1013904223u;
    random[i] = (seed >> 16) & 255;
  }
  vsorted = vrandom;
  std::sort(vsorted.begin(), vsorted.end());

  check(random, N, T);

  // (provided) the measurements of questions (a) and (c)
  int n = 0;
  double c;
  printf("cycles per element, %d elements:\n", N);
  c = cycles_per_element([&] { n = split_branch(sorted, N, T, lo, hi); },
                         NREPS, N, ghz);
  printf("  with a branch,    sorted data: %5.2f\n", c);
  c = cycles_per_element([&] { n = split_branch(random, N, T, lo, hi); },
                         NREPS, N, ghz);
  printf("  with a branch,    random data: %5.2f\n", c);
  c = cycles_per_element([&] { n = split_nobranch(sorted, N, T, lo, hi); },
                         NREPS, N, ghz);
  printf("  without a branch, sorted data: %5.2f\n", c);
  c = cycles_per_element([&] { n = split_nobranch(random, N, T, lo, hi); },
                         NREPS, N, ghz);
  printf("  without a branch, random data: %5.2f\n", c);

  // (provided) question (d): the first SHORT random numbers only,
  // traversed again and again (the same number of elements in all)
  c = cycles_per_element(
      [&] { n = split_branch(random, SHORT, T, lo, hi); },
      NREPS * (N / SHORT), SHORT, ghz);
  printf("with a branch, %d random elements: %5.2f\n", SHORT, c);

  return 0;
}
