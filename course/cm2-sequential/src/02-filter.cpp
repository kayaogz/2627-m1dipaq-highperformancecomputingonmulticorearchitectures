// Sessions 2 and 3 - Sequential code optimization
// 02-filter.cpp: copying the elements of an array that are at least t, with a
// branch and without, on sorted and on random data.
//   Compile and run it on the lab server (MyDocker), or on a PC with an Intel
//   or AMD processor:
//     g++ -O2 -fno-tree-vectorize 02-filter.cpp -o 02-filter && ./02-filter
//   To read its instructions: Compiler Explorer (https://godbolt.org), compiler
//   x86-64 gcc 16.2, with the same options.
#include <algorithm> // std::sort, std::min
#include <chrono>    // timing
#include <cpuid.h>   // name of the processor
#include <cstdio>    // printf
#include <vector>    // std::vector

const int NMEASUREMENTS = 5; // measurements; the smallest time is kept

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

// The measurement of 01-dot-sums.cpp, written once for any piece of code:
// executes work() nreps times in a row, NMEASUREMENTS times, keeps the smallest time,
// and returns it in cycles per element.
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

// Copies to out the elements of a that are at least t; returns their number.
__attribute__((noinline, noipa))
int filter_branch(const int *a, int n, int t, int *out)
{
  int k = 0;
  for (int i = 0; i < n; i++)
    if (a[i] >= t)
      out[k++] = a[i];
  return k;
}

// The same without a branch: every element is written to out, and the position
// in out advances only when the element is kept. out needs room for n elements.
__attribute__((noinline, noipa))
int filter_nobranch(const int *a, int n, int t, int *out)
{
  int k = 0;
  for (int i = 0; i < n; i++) {
    out[k] = a[i];
    k = k + (a[i] >= t); // the comparison is worth 1 or 0
  }
  return k;
}

int main()
{
  print_processor();
  double ghz = frequency();
  printf("frequency: %.2f GHz\n", ghz);

  // numbers between 0 and 255 in random order, and the same numbers sorted
  const int N = 100000, NREPS = 100;
  std::vector<int> random(N), sorted(N), out(N);
  unsigned int seed = 2026;
  for (int i = 0; i < N; i++) {
    seed = seed * 1664525u + 1013904223u;
    random[i] = (seed >> 16) & 255;
  }
  sorted = random;
  std::sort(sorted.begin(), sorted.end());

  int *rnd = random.data(), *srt = sorted.data(), *o = out.data();
  int kept = 0;
  double c;
  printf("cycles per element, %d elements:\n", N);
  c = cycles_per_element([&] { kept = filter_branch(srt, N, 128, o); }, NREPS, N, ghz);
  printf("  with a branch,    sorted data: %5.2f   (%d elements kept)\n", c, kept);
  c = cycles_per_element([&] { kept = filter_branch(rnd, N, 128, o); }, NREPS, N, ghz);
  printf("  with a branch,    random data: %5.2f   (%d elements kept)\n", c, kept);
  c = cycles_per_element([&] { kept = filter_nobranch(srt, N, 128, o); }, NREPS, N, ghz);
  printf("  without a branch, sorted data: %5.2f   (%d elements kept)\n", c, kept);
  c = cycles_per_element([&] { kept = filter_nobranch(rnd, N, 128, o); }, NREPS, N, ghz);
  printf("  without a branch, random data: %5.2f   (%d elements kept)\n", c, kept);

  // a trap: the first 1000 random numbers only, traversed again and again
  const int SHORT = 1000, MORE = NREPS * (N / SHORT); // the same work in all
  c = cycles_per_element([&] { kept = filter_branch(rnd, SHORT, 128, o); }, MORE, SHORT, ghz);
  printf("with a branch, %d random numbers again and again: %5.2f\n", SHORT, c);
  return 0;
}
