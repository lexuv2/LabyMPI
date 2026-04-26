#!/bin/bash
export OMPI_PRTERUN=/usr/lib64/openmpi/bin/prterun
/usr/lib64/openmpi/bin/mpicc -O2 -fopenmp mpi+openmp.c -o openmp+mpi -lm
/usr/lib64/openmpi/bin/mpirun -bind-to none -np 4 ./openmp+mpi 1 10000000 RUN1 4
