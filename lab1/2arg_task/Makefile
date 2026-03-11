MPICC=/usr/lib64/openmpi/bin/mpicc
CFLAGS=-O2
LIBS=-lm

ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif


main: mpi.c
	${MPICC} ${CFLAGS} $< -o mpi ${LIBS}

run:
	/usr/lib64/openmpi/bin/mpirun -np 4 ./mpi $(RUN_ARGS)

clean:
	rm mpi