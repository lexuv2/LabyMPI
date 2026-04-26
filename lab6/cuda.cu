#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <sys/time.h>
#include <limits.h>

__host__
void errorexit(const char *s) {
  fprintf(stderr, "\n%s\n", s);
  exit(EXIT_FAILURE);
}

__device__
bool is_prime_device(long long n) {
  if (n < 2LL) return false;
  if (n % 2LL == 0LL) return n == 2LL;
  if (n % 3LL == 0LL) return n == 3LL;
  for (long long i = 5LL; i <= n / i; i += 6LL) {
    if (n % i == 0LL) return false;
    if (n % (i + 2LL) == 0LL) return false;
  }
  return true;
}

__global__
void calculate(int *result, long long start, long long stop, long long count) {
  long long my_index = (long long)blockIdx.x * blockDim.x + threadIdx.x;
  if (my_index >= count) return;
  long long n = start + my_index;
  if (n <= stop - 2LL && is_prime_device(n) && is_prime_device(n + 2LL)) {
    result[my_index] = 1;
  } else {
    result[my_index] = 0;
  }
}

int main(int argc,char **argv) {
  Args ins__args;
  parseArgs(&ins__args, &argc, argv);

  if (ins__args.stop < ins__args.start) {
    errorexit("Stop must be greater than or equal to start.");
  }

  struct timeval ins__tstart, ins__tstop;
  gettimeofday(&ins__tstart, NULL);

  long long count_ll = ins__args.stop - ins__args.start + 1LL;
  if (count_ll <= 0LL) {
    errorexit("Invalid range size.");
  }
  size_t count = (size_t)count_ll;
  if ((long long)count != count_ll) {
    errorexit("Range is too large for memory allocation.");
  }
  size_t bytes = count * sizeof(int);
  if (count != 0 && bytes / sizeof(int) != count) {
    errorexit("Range is too large for memory allocation.");
  }

  int *hresults = (int*)malloc(bytes);
  if (!hresults) errorexit("Error allocating memory on the host");

  int *dresults = NULL;
  if (cudaSuccess != cudaMalloc((void **)&dresults, bytes))
    errorexit("Error allocating memory on the GPU");

  const int threadsinblock = 256;
  long long blocksingrid_ll = (count_ll + threadsinblock - 1LL) / threadsinblock;
  if (blocksingrid_ll > INT_MAX) {
    errorexit("Range is too large for this kernel launch configuration.");
  }
  int blocksingrid = (int)blocksingrid_ll;

  calculate<<<blocksingrid, threadsinblock>>>(dresults, ins__args.start, ins__args.stop, count_ll);
  if (cudaSuccess != cudaGetLastError())
    errorexit("Error during kernel launch");
  if (cudaSuccess != cudaDeviceSynchronize())
    errorexit("Error during kernel execution");

  if (cudaSuccess != cudaMemcpy(hresults, dresults, bytes, cudaMemcpyDeviceToHost))
    errorexit("Error copying results");

  long long result = 0;
  for (size_t i = 0; i < count; i++) {
    result += hresults[i];
  }

  printf("wynik: %lld\n", result);

  free(hresults);
  if (cudaSuccess != cudaFree(dresults))
    errorexit("Error when deallocating space on the GPU");

  gettimeofday(&ins__tstop, NULL);
  ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
  return 0;
}
