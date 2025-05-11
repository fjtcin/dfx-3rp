CC=g++
CFLAGS=-O1 -Wall -Wextra
INC=-I/usr/include/xrt -I/usr/include/dfx-mgr
LIBS=-lxrt_coreutil -ldfx-mgr
UTILS=lib/siha.c lib/stream_utils.cpp

BINARIES_TEST=twice inter_twice all_twice
BINARIES_SPMM=gemm decoder encoder dec_enc spmm
BINARIES_CPU=cpu_gemm

all: test matrix cpu

test: $(BINARIES_TEST)

matrix: $(BINARIES_SPMM)

cpu: $(BINARIES_CPU)

%: %.c $(UTILS)
	$(CC) $(CFLAGS) $(INC) $^ $(LIBS) -o $@

clean:
	rm -f $(BINARIES_TEST) $(BINARIES_SPMM) $(BINARIES_CPU)

.PHONY: all test matrix cpu clean
