// =====================================================================
//  Lab 1 - Sequential code optimization
//  Exercise 2: Measuring a loop  --  SKELETON
//
//  Fill in the parts marked  TODO  in the order of the questions.
//
//  Run it on the MyDocker server, in its terminal:
//    g++ -O2 -fno-tree-vectorize dot.cpp -o dot && ./dot
//  Read its instructions on Compiler Explorer (https://godbolt.org):
//  compiler x86-64 gcc 16.2, the same options. The server has g++ 16
//  too: the instructions are almost the same.
// =====================================================================

// =====================================================================
//  ANSWERS to the questions of the assignment (write them here, as
//  comments; this is the only file you submit for this exercise).
//
//  (b) Processor, frequency, cycles per element. Which number of
//      chapter 1 is it?
//
//  (c) Cycles per element with -O0, -O1, -O2 and -O3. Where is s at -O0?
//      Why is -O0 slower? Why are the three others equal?
//
//  (d) Cycles per element without "noipa". Why?
//
// =====================================================================

#include <algorithm> // std::min
#include <chrono>    // timing
#include <cpuid.h>   // name of the processor
#include <cstdio>    // printf
#include <vector>    // std::vector

// ---------------------------------------------------------------------
//  Timing helper (provided): returns the current time in seconds.
//  Usage:  double t0 = now(); ...work... ; double elapsed = now() - t0;
// ---------------------------------------------------------------------
double now()
{
  using namespace std::chrono;
  return duration<double>(steady_clock::now().time_since_epoch()).count();
}

// ---------------------------------------------------------------------
//  Name of the processor (provided). The numbers of cycles depend on
//  it: note it next to your measurements.
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
//  Frequency of the core, in GHz (provided). The loop written in
//  assembly multiplies x by 3, COUNT times. Each multiplication needs
//  the result of the previous one, and a multiplication of integers
//  takes 3 cycles: the loop lasts 3 * COUNT cycles. Dividing this number
//  by the duration of the loop gives the number of cycles per second.
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
//  The dot product of chapter 1 (provided).
//  Note: "noinline, noipa" make the compiler call the function each
//  time, as it is written (question (d) shows what happens otherwise).
// ---------------------------------------------------------------------
__attribute__((noinline, noipa))
double dot(const double *a, const double *b, int n)
{
  double s = 0.0;
  for (int i = 0; i < n; i++)
    s = s + a[i] * b[i];
  return s;
}

int main()
{
  const int N             = 1000;   // elements per array: 8 KB, in L1
  const int NREPS         = 100000; // calls of dot in one measurement
  const int NMEASUREMENTS = 5;      // number of measurements

  print_processor();
  double ghz = frequency();
  printf("frequency: %.2f GHz\n", ghz);

  // (provided) a[i] = 1 and b[i] = 2: the dot product is 2 * N.
  std::vector<double> a(N, 1.0), b(N, 2.0);
  double result = 0.0;

  // -------------------------------------------------------------------
  //  Question (a): one measurement. Call dot NREPS times in a row on the
  //  arrays a and b, and measure the duration of these NREPS calls.
  //  Then convert it: nanoseconds per element, cycles per element.
  // -------------------------------------------------------------------
  double seconds = 0.0; // TODO (a): the duration of NREPS calls of dot
  double elements = (double) NREPS * N; // elements processed in all
  double ns_elem = 0.0;     // TODO (a): nanoseconds per element
  double cycles_elem = 0.0; // TODO (a): cycles per element

  printf("result = %.1f (expected %.1f)\n", result, 2.0 * N);
  printf("(a) %.4f s, %.3f ns per element, %.2f cycles per element\n",
         seconds, ns_elem, cycles_elem);

  // -------------------------------------------------------------------
  //  Question (b): NMEASUREMENTS measurements like the one of question (a);
  //  keep the smallest duration, and convert it in the same way.
  // -------------------------------------------------------------------
  double best = 0.0; // TODO (b): the smallest of NMEASUREMENTS durations
  ns_elem = 0.0;     // TODO (b)
  cycles_elem = 0.0; // TODO (b)

  printf("(b) %.4f s, %.3f ns per element, %.2f cycles per element\n",
         best, ns_elem, cycles_elem);

  return 0;
}
