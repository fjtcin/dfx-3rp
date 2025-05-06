#include "lib/stream_utils.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

#define SIZE_IN_BYTES       0x4000000	//64MB

#define MUX_OFFSET			0
#define MUX_OFFSET_MEM		0x0

#define A_OFFSET			0x10
#define C_OFFSET			0x4014      // 0x10 + 16388
#define A_OFFSET_MEM		0x40
#define C_OFFSET_MEM		0x10050
#define TID_0				0x0
#define TID_1				0x1


#define die(fmt, args ...) do { fprintf(stderr, \
	"ERROR:%s():%u " fmt ": %s\n", \
	__func__, __LINE__, ##args, errno ? strerror(errno) : ""); \
	exit(EXIT_FAILURE); \
} while (0)

int InitializeMapRMs(int slot);
int StartAccel(int slot);
int FinaliseUnmapRMs(int slot);
void mapBuffer(xrt::bo boa);
int DataToAccel(int slot, uint64_t data, uint64_t size, uint8_t tid);
int DataFromAccel(int slot, uint64_t data, uint64_t size);
int DataToAccelDone(int slot);
int DataFromAccelDone(int slot);

int getFormat(const char* str) {
	if (!strcmp(str, "--coo")) return 1;
	if (!strcmp(str, "--csr")) return 2;
	if (!strcmp(str, "--csc")) return 3;
	return 0;
}

uint32_t Mux = 0;

// A Input Buffer
float A[16384+2];
int input_size = 16384+2;

// C Output Buffer
float C[4096];
int output_size;


int main(int argc, char** argv) {
	if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_filename> <output_filename> [--coo, --csr, --csc]\n", argv[0]);
        fprintf(stderr, "Example: %s A_COO.txt A.txt\n", argv[0]);
        return 1; // Return an error code
    }
	int format = getFormat(argv[3]);
	if (!format) die("format %s", argv[3]);
	if (readStream(argv[1], A, input_size)) die("readStream");

	int slot = 1;

	if (InitializeMapRMs(slot) == -1) die("Slot number %d", slot);
	StartAccel(slot);

	//Allocate XRT buffer to be used for input and output of the application
	auto device = xrt::device(0);
	auto bufferObject = xrt::bo(device, SIZE_IN_BYTES, 0);
	uint32_t* vptr = (uint32_t*)bufferObject.map<int*>();
	mapBuffer(bufferObject);

	memcpy(vptr+MUX_OFFSET, &Mux, sizeof(Mux));
	memcpy(vptr+A_OFFSET, A, input_size*4);

	DataToAccel(slot, MUX_OFFSET_MEM, 1, TID_1);
	if (!DataToAccelDone(slot)) die("DataToAccelDone(%d)", slot);

	DataToAccel(slot, A_OFFSET_MEM, (input_size+3)/4, TID_0);
	if (!DataToAccelDone(slot)) die("DataToAccelDone(%d)", slot);

	DataFromAccel(slot, C_OFFSET_MEM, 1);
	if (!DataFromAccelDone(slot)) die("DataFromAccelDone(%d)", slot);
	const int N = *((float*)vptr+C_OFFSET), M = *((float*)vptr+C_OFFSET+1);
	const int nnz = *((float*)vptr+C_OFFSET+2);
	if (format == 1) output_size = 3*nnz + 3;
	else if (format == 2) output_size = 2*nnz + N + 1 + 3;
	else output_size = 2*nnz + M + 1 + 3;

	DataFromAccel(slot, C_OFFSET_MEM+16, (output_size-1)/4);
	if (!DataFromAccelDone(slot)) die("DataFromAccelDone(%d)", slot);

	printf("\t Success: Selected Operation Done !.\n");
	memcpy(C, vptr+C_OFFSET, output_size*4);
	if (writeStream(argv[2], C, output_size)) die("writeStream");

	FinaliseUnmapRMs(slot);
	return 0;
}
