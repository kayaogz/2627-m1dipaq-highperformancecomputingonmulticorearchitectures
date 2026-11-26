// Session 1 - The anatomy of a CPU core architecture
// 01-dot.cpp: the dot product of two arrays, the program that the session
// follows through the processor.
//   g++ -O2 01-dot.cpp -o 01-dot
#include <cstdio> // printf
#include <vector> // std::vector

// s = a[0]*b[0] + a[1]*b[1] + ... + a[n-1]*b[n-1]
double dot(const double* a, const double* b, int n)
{
  double s = 0.0;
  for (int i = 0; i < n; i++)
    s = s + a[i] * b[i];
  return s;
}

int main()
{
  int n = 1000000;
  std::vector<double> a(n, 1.0); // a[i] = 1
  std::vector<double> b(n, 2.0); // b[i] = 2
  double s = dot(a.data(), b.data(), n);
  printf("dot = %.1f (expected %.1f)\n", s, 2.0 * n);
  return 0;
}
