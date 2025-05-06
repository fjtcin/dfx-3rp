CC=g++
CFLAGS=-O1 -Wall -Wextra
INC=-I/usr/include/xrt -I/usr/include/dfx-mgr
LIBS=-lxrt_coreutil -ldfx-mgr
UTILS=lib/siha.c lib/stream_utils.cpp

BINARIES_TEST=twice inter_twice all_twice
BINARIES_SPMM=gemm decoder encoder dec_enc spmm

all: test matrix

test: $(BINARIES_TEST)

matrix: $(BINARIES_SPMM)

%: %.c $(UTILS)
	$(CC) $(CFLAGS) $(INC) $^ $(LIBS) -o $@

clean:
	rm -f $(BINARIES_TEST) $(BINARIES_SPMM)

.PHONY: all test matrix clean
