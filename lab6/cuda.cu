#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <sys/time.h>
#include <limits.h>

__host__
void errorexit(const char *s) {
printf("\n%s",s);
exit(EXIT_FAILURE);
}

__device__
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
//generate elements of sequence – parallel part
__global__
void calculate(int *result, long long start, long long stop, long long count) {
  int my_index =blockIdx.x*blockDim.x+threadIdx.x;
  if (my_index >= count) return;
  long long n = start + my_index;
  if (n <= stop - 2 && is_prime(n) && is_prime(n + 2)) {
    result[my_index] = 1;
  } else {
    result[my_index] = 0;
  }
}

int main(int argc,char **argv) {
long long result;
Args ins__args;
struct timeval ins__tstart, ins__tstop;
parseArgs(&ins__args, &argc, argv);
int threadsinblock=1024;
int blocksingrid=10000;
int size = threadsinblock*blocksingrid;
//memory allocation on host
int *hresults=(int*)malloc(size*sizeof(int));
if (!hresults) errorexit("Error allocating memory on the host");
int *dresults=NULL;
 gettimeofday(&ins__tstart, NULL);
//memory allocation on device (GPU)
if (cudaSuccess!=cudaMalloc((void **)&dresults,size*sizeof(int)))
errorexit("Error allocating memory on the GPU");
//call kernel on GPU – calculation are executed on GPU
calculate<<<blocksingrid,threadsinblock>>>(dresults,ins__args.start,ins__args.stop,size);
if (cudaSuccess!=cudaGetLastError())
errorexit("Error during kernel launch");
//copy all elements from device to host
if (cudaSuccess!=cudaMemcpy(hresults,dresults,size*sizeof(int),cudaMemcpyDeviceToHost))
errorexit("Error copying results");
//calculate sum of all elements on CPU side
result=0;
for(int i=0;i<size;i++) {
result = result + hresults[i];
}
printf("\nThe final result is %lld\n",result);
gettimeofday(&ins__tstop, NULL);
ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
//free resources
free(hresults);
if (cudaSuccess!=cudaFree(dresults))
errorexit("Error when deallocating space on the GPU");
}