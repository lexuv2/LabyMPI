#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <omp.h>
#include <stdbool.h>

static bool is_prime(long long n)
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
int main(int argc, char **argv)
{
  Args ins__args;

  struct timeval ins__tstart, ins__tstop;

  int threadsupport;
  int myrank, nproc;

  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &threadsupport);

  if (threadsupport != MPI_THREAD_MULTIPLE)
  {
    printf("\nThe implementation does not support MPI_THREAD_MULTIPLE, it supports level %d\n", threadsupport);
    MPI_Finalize();
    return -1;
  }

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  parseArgs(&ins__args, &argc, argv);
  if (ins__args.n_thr <= 0)
  {
    if (!myrank)
      fprintf(stderr, "Number of threads must be greater than 0.\n");
    MPI_Finalize();
    return -1;
  }
  if (ins__args.stop < ins__args.start)
  {
    if (!myrank)
      fprintf(stderr, "Stop must be greater than or equal to start.\n");
    MPI_Finalize();
    return -1;
  }

  omp_set_num_threads(ins__args.n_thr);

  if (!myrank)
    gettimeofday(&ins__tstart, NULL);

  const long long total_workers = (long long)nproc * ins__args.n_thr;
  const long long proc_offset = (long long)myrank * ins__args.n_thr;
  long long local_count = 0;

#pragma omp parallel reduction(+ : local_count)
  {
    const int tid = omp_get_thread_num();
    const long long worker_id = proc_offset + tid;
    for (long long n = ins__args.start + worker_id; n <= ins__args.stop - 2; n += total_workers)
    {
      if (is_prime(n) && is_prime(n + 2))
      {
        ++local_count;
      }
    }
  }

  long long global_count = 0;
  MPI_Reduce(&local_count, &global_count, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

  if (!myrank)
  {
    printf("wynik: %lld\n", global_count);
    gettimeofday(&ins__tstop, NULL);
    ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
  }

  MPI_Finalize();
  return 0;
}
