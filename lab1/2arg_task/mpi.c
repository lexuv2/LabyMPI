#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <stdbool.h>


bool is_prime(int n) {
  if (n < 2) return false;
  if (n % 2 == 0) return n == 2;
  if (n % 3 == 0) return n == 3;
  for (int i = 5; i * i <= n; i += 6) {
    if (n % i == 0) return false;
    if (n % (i + 2) == 0) return false;
  }
  return true;
}

int main(int argc,char **argv) {

  Args ins__args;
  parseArgs(&ins__args, &argc, argv);

  int INITIAL_NUMBER = ins__args.start; 
  int FINAL_NUMBER = ins__args.stop;

  printf("Start %d\n", INITIAL_NUMBER);
  printf("Stop %d\n", FINAL_NUMBER);

  struct timeval ins__tstart, ins__tstop;

  int myrank,nproc;
  
  MPI_Init(&argc,&argv);

  // obtain my rank
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  // and the number of processes
  MPI_Comm_size(MPI_COMM_WORLD,&nproc);

  if(!myrank) 
      gettimeofday(&ins__tstart, NULL);

  // run your computations here (including MPI communication)

  int part_size = (FINAL_NUMBER - INITIAL_NUMBER + nproc - 1) / nproc; 
  int start = INITIAL_NUMBER + myrank * part_size;
  int end = start + part_size;
  int local_count = 0;
  for (int i = start; i < end && i < FINAL_NUMBER; ++i) {
    if (is_prime(i) && is_prime(i + 2)) {
      ++local_count;
    }
  }
  printf("Proc %d znalazł %d\n" , myrank, local_count);
  int global_count = 0;

    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (!myrank) {
    printf("wynik: %d\n", global_count);
    gettimeofday(&ins__tstop, NULL);
    ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
  }
  
  MPI_Finalize();

}
