export OMPI_PRTERUN=/usr/lib64/openmpi/bin/prterun
/usr/lib64/openmpi/bin/mpicc mpi.c -o mpi 
/usr/lib64/openmpi/bin/mpirun -np 8 ./mpi 1 10000 RUN1