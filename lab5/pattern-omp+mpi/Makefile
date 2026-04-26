MPICC=/usr/lib64/openmpi/bin/mpicc
MPIRUN=/usr/lib64/openmpi/bin/mpirun
CFLAGS=-O2
LIBS=-lm
OMPFLAGS=-fopenmp

ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif


main: mpi+openmp.c
	${MPICC} ${CFLAGS} ${OMPFLAGS} $< -o openmp+mpi ${LIBS}

run:
	OMPI_PRTERUN=/usr/lib64/openmpi/bin/prterun ${MPIRUN} -bind-to none -np 2 ./openmp+mpi $(RUN_ARGS)

clean:
	rm -f openmp+mpi
