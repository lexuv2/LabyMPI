#include <stdio.h>
#include <mpi.h>
#include <stdbool.h>
#include <math.h>
#include "utility.h"
#define PRECISION 0.000001
#define RANGESIZE 10000
#define DATA 0
#define RESULT 1
#define FINISH 2
// #define int long long
// #define DEBUG

bool is_prime(long long n) {
  if (n < 2) return false;
  if (n % 2 == 0) return n == 2;
  if (n % 3 == 0) return n == 3;
  for (long long i = 5; i * i <= n; i += 6) {
    if (n % i == 0) return false;
    if (n % (i + 2) == 0) return false;
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
  for (int i = start; i <= end; ++i) {
    bool current_is_prime = is_prime(i);
    if (current_is_prime && last_primes[0]) {
      ++local_count;
    }
    last_primes[0] = last_primes[1];
    last_primes[1] = current_is_prime;
  }
  return local_count;
}

int main(int argc, char **argv)
{
    int myrank, proccount;
    if (argc < 3)
    {
        return -1;
    }
      Args ins__args;
  parseArgs(&ins__args, &argc, argv);
      int a = ins__args.start; 
  int b = ins__args.stop;
    int range[2];
    int result = 0, resulttemp;
    int sentcount = 0;
    int i;
    MPI_Status status;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    // find out my rank
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    // find out the number of processes in MPI_COMM_WORLD
    MPI_Comm_size(MPI_COMM_WORLD, &proccount);

    if (proccount < 2)
    {
        printf("Run with at least 2 processes");
        MPI_Finalize();
        return -1;
    }

    if (((b - a) / RANGESIZE) < 2 * (proccount - 1))
    {
        printf("More subranges needed");
        MPI_Finalize();
        return -1;
    }

    // now the master will distribute the data and slave processes will perform computations
    if (myrank == 0)
    {
        range[0] = a;
        int active_slaves = 0;
        int alive[proccount];
        for (i = 0; i < proccount; ++i) alive[i] = 0;

        // first distribute some ranges to all slaves
        for (i = 1; i < proccount ; i++)
        {
            for (int j = 0; j < 2 ; j++) {
                range[1] = range[0] + RANGESIZE;
                if (range[1] > b) range[1] = b;
                MPI_Send(range, 2, MPI_INT, i, DATA, MPI_COMM_WORLD);
                alive[i]++;
                range[0] = range[1] + 1;
            }
            if (alive[i] > 0) active_slaves++;
        }

        while (active_slaves > 0)
        {
            MPI_Recv(&resulttemp, 1, MPI_INT, MPI_ANY_SOURCE, RESULT,
                     MPI_COMM_WORLD, &status);
            result += resulttemp;
            alive[status.MPI_SOURCE]--;

            if (range[0] <= b) {
                range[1] = range[0] + RANGESIZE;
                if (range[1] > b) range[1] = b;
                MPI_Send(range, 2, MPI_INT, status.MPI_SOURCE, DATA,
                         MPI_COMM_WORLD);
                alive[status.MPI_SOURCE]++;
                range[0] = range[1] + 1;
            } else if (alive[status.MPI_SOURCE] == 0) {
                active_slaves--;
            }
        }

        // shut down the slaves
        for (i = 1; i < proccount; i++)
        {
            MPI_Send(NULL, 0, MPI_INT, i, FINISH, MPI_COMM_WORLD);
        }
        // now display the result
        printf("\nHi, I am process 0, the result is %d\n", (int)result);
    }
    else
    { // slave
        int range[2], next[2];
        int result_buf;
        MPI_Request recv_req = MPI_REQUEST_NULL, send_req = MPI_REQUEST_NULL;

        MPI_Recv(range, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        while (status.MPI_TAG != FINISH)
        {
            
            MPI_Irecv(next, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_req);

            resulttemp = prime_pairs_in_range(range[0], range[1]);
            if (send_req != MPI_REQUEST_NULL) {
                MPI_Wait(&send_req, MPI_STATUS_IGNORE);
            }
            result_buf = resulttemp;
            
            MPI_Isend(&result_buf, 1, MPI_INT, 0, RESULT, MPI_COMM_WORLD, &send_req);
            MPI_Wait(&recv_req, &status);
            range[0] = next[0];
            range[1] = next[1];
        }

        if (send_req != MPI_REQUEST_NULL) {
            MPI_Wait(&send_req, MPI_STATUS_IGNORE);
        }
    }

    // Shut down MPI
    MPI_Finalize();

    return 0;
}