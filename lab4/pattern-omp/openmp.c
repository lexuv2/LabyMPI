#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include "numgen.c"

#include <stdbool.h>

bool is_prime(long long n)
{
  if (n < 2)
    return false;
  if (n % 2 == 0)
    return n == 2;
  if (n % 3 == 0)
    return n == 3;
  for (long long i = 5; i * i <= n; i += 6)
  {
    if (n % i == 0)
      return false;
    if (n % (i + 2) == 0)
      return false;
  }
  return true;
}
// 0 1 2 3 4 5 6
// 1 1 2 2 3 3
long long prime_pairs_in_range(int start, int end)
{
  //    int local_count = 0;
  // for (long long i = start; i <= end; ++i) {
  //   if (is_prime(i) && is_prime(i + 2)) {
  //     ++local_count;
  //   }
  // }
  //   return local_count;
  int local_count = 0;
  bool last_primes[2] = {false, false};
  last_primes[0] = is_prime(start - 2);
  last_primes[1] = is_prime(start - 1);
  for (int i = start; i <= end; ++i)
  {
    bool current_is_prime = is_prime(i);
    if (current_is_prime && last_primes[0])
    {
      ++local_count;
    }
    last_primes[0] = last_primes[1];
    last_primes[1] = current_is_prime;
  }
  return local_count;
}

int main(int argc, char **argv)
{

  Args ins__args;
  parseArgs(&ins__args, &argc, argv);

  // set number of threads
  const int thr = 4;
  omp_set_num_threads(thr);


  struct timeval ins__tstart, ins__tstop;
  gettimeofday(&ins__tstart, NULL);

  int start = ins__args.start;
  int stop = ins__args.stop;

  // run your computations here (including OpenMP stuff)
  long long wyn = 0;
  #pragma omp parallel reduction(+ : wyn) // reduction allow to execute operation on chosen variable in all threads 
  {
    int thread_id = omp_get_thread_num();
    int num_threads = omp_get_num_threads();
    int local_start = start + thread_id * (stop - start + 1) / num_threads;
    int local_stop = start + (thread_id + 1) * (stop - start+ 1) / num_threads - 1;
    if (local_stop > stop)
      local_stop = stop;
    wyn += prime_pairs_in_range(local_start, local_stop);
    
  }
  
  

  // synchronize/finalize your computations
  printf("wynik: %lld\n", wyn);
  gettimeofday(&ins__tstop, NULL);
  ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
}
