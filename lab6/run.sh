#!/bin/bash
/usr/local/cuda/bin/nvcc -O2 -allow-unsupported-compiler -U_GNU_SOURCE -ccbin /usr/bin/clang++-17 cuda.cu -o cuda
./cuda 1 10000000 RUN1
