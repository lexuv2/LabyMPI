NVCC=/usr/local/cuda/bin/nvcc
HOSTCXX=/usr/bin/clang++-17
CUDAFLAGS=-O2 -allow-unsupported-compiler -U_GNU_SOURCE -ccbin ${HOSTCXX}

ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif


main: cuda.cu
	${NVCC} ${CUDAFLAGS} $< -o cuda

run:
	./cuda $(RUN_ARGS)

clean:
	rm -f cuda
