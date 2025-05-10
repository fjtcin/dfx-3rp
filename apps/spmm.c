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

#define MUX0_OFFSET			0
#define MUX1_OFFSET         4
#define MUX2_OFFSET         8
#define MUX0_OFFSET_MEM		0x0
#define MUX1_OFFSET_MEM     0x10
#define MUX2_OFFSET_MEM     0x20

#define A_OFFSET			0x10
#define B_OFFSET			0x1010
#define C_OFFSET			0x2010
#define A_OFFSET_MEM		0x40
#define B_OFFSET_MEM		0x4040
#define C_OFFSET_MEM		0x8040
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

uint32_t Mux0 = 2;
uint32_t Mux1 = 2;
uint32_t Mux2 = 3;

// A Input Buffer
float A[4096];
int A_input_size = 4096;

// B Input Buffer
float B[4096];
int B_input_size = 4096;

// C Output Buffer
float C[4096];
int output_size;


int main(int argc, char** argv) {
	if (argc != 5) {
        fprintf(stderr, "Usage: %s <input_filename> <input_filename> <output_filename> [--coo, --csr, --csc]\n", argv[0]);
        fprintf(stderr, "Example: %s A_CSR.txt B_CSR.txt C_CSC.txt --csc\n", argv[0]);
        return 1; // Return an error code
    }
	int format = getFormat(argv[4]);
	if (!format) die("format %s", argv[4]);
	if (readStream(argv[1], A, A_input_size)) die("readStream");
	if (readStream(argv[2], B, B_input_size)) die("readStream");

	int slot0 = 0;
	int slot1 = 1;
	int slot2 = 2;

	if (InitializeMapRMs(slot0) == -1) die("Slot number %d", slot0);
	StartAccel(slot0);
	if (InitializeMapRMs(slot1) == -1) die("Slot number %d", slot1);
	StartAccel(slot1);
	if (InitializeMapRMs(slot2) == -1) die("Slot number %d", slot2);
	StartAccel(slot2);

	//Allocate XRT buffer to be used for input and output of the application
	auto device = xrt::device(0);
	auto bufferObject = xrt::bo(device, SIZE_IN_BYTES, 0);
	uint32_t* vptr = (uint32_t*)bufferObject.map<int*>();
	mapBuffer(bufferObject);

	memcpy(vptr+MUX0_OFFSET, &Mux0, sizeof(Mux0));
	memcpy(vptr+MUX1_OFFSET, &Mux1, sizeof(Mux1));
	memcpy(vptr+MUX2_OFFSET, &Mux2, sizeof(Mux2));
	memcpy(vptr+A_OFFSET, A, A_input_size*4);
	memcpy(vptr+B_OFFSET, B, B_input_size*4);

	DataToAccel(slot0, MUX0_OFFSET_MEM, 1, TID_1);
	if (!DataToAccelDone(slot0)) die("DataToAccelDone(%d)", slot0);

	DataToAccel(slot1, MUX1_OFFSET_MEM, 1, TID_1);
	if (!DataToAccelDone(slot1)) die("DataToAccelDone(%d)", slot1);

	DataToAccel(slot2, MUX2_OFFSET_MEM, 1, TID_1);
	if (!DataToAccelDone(slot2)) die("DataToAccelDone(%d)", slot2);

	DataToAccel(slot0, A_OFFSET_MEM, (A_input_size+3)/4, TID_0);
	if (!DataToAccelDone(slot0)) die("DataToAccelDone(%d)", slot0);

	DataToAccel(slot0, B_OFFSET_MEM, (B_input_size+3)/4, TID_0);
	if (!DataToAccelDone(slot0)) die("DataToAccelDone(%d)", slot0);

	DataFromAccel(slot1, C_OFFSET_MEM, 1);
	if (!DataFromAccelDone(slot1)) die("DataFromAccelDone(%d)", slot1);
	const int N = *((float*)vptr+C_OFFSET), M = *((float*)vptr+C_OFFSET+1);
	const int nnz = *((float*)vptr+C_OFFSET+2);
	if (format == 1) output_size = 3*nnz + 3;
	else if (format == 2) output_size = 2*nnz + N + 1 + 3;
	else output_size = 2*nnz + M + 1 + 3;

	DataFromAccel(slot1, C_OFFSET_MEM+16, (output_size-1)/4);
	if (!DataFromAccelDone(slot1)) die("DataFromAccelDone(%d)", slot1);

	printf("\t Success: Selected Operation Done !.\n");
	memcpy(C, vptr+C_OFFSET, output_size*4);
	if (writeStream(argv[3], C, output_size)) die("writeStream");

	FinaliseUnmapRMs(slot0);
	FinaliseUnmapRMs(slot1);
	FinaliseUnmapRMs(slot2);
	return 0;
}
