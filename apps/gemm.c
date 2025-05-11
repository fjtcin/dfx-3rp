#include "lib/stream_utils.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

#define SIZE_IN_BYTES       0x4000000	//64MB

#define MUX_OFFSET			0
#define MUX_OFFSET_MEM		0x0

#define A_OFFSET			0x10
#define B_OFFSET			0x4014      // 0x10 + 16388
#define C_OFFSET			0x8018
#define A_OFFSET_MEM		0x40
#define B_OFFSET_MEM		0x10050
#define C_OFFSET_MEM		0x20060
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

uint32_t Mux = 0;

// A Input Buffer
float A[16384+2];
int A_input_size = 16384+2;

// B Input Buffer
float B[16384+2];
int B_input_size = 16384+2;

// C Output Buffer
float C[16384+2];
int output_size;


int main(int argc, char** argv) {
	if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_filename> <input_filename> <output_filename>\n", argv[0]);
        fprintf(stderr, "Example: %s A.txt B.txt C.txt\n", argv[0]);
        return 1; // Return an error code
    }
	if (readStream(argv[1], A, A_input_size)) die("readStream");
	if (readStream(argv[2], B, B_input_size)) die("readStream");

	int slot = 2;

	if (InitializeMapRMs(slot) == -1) die("Slot number %d", slot);
	StartAccel(slot);

	//Allocate XRT buffer to be used for input and output of the application
	auto device = xrt::device(0);
	auto bufferObject = xrt::bo(device, SIZE_IN_BYTES, 0);
	uint32_t* vptr = (uint32_t*)bufferObject.map<int*>();
	mapBuffer(bufferObject);

	memcpy(vptr+MUX_OFFSET, &Mux, sizeof(Mux));
	memcpy(vptr+A_OFFSET, A, A_input_size*4);
	memcpy(vptr+B_OFFSET, B, B_input_size*4);

auto start_time = std::chrono::high_resolution_clock::now();
for (int i = 0; i < 100; ++i) {
	DataToAccel(slot, MUX_OFFSET_MEM, 1, TID_1);
	if (!DataToAccelDone(slot)) die("DataToAccelDone(%d)", slot);

	DataToAccel(slot, A_OFFSET_MEM, (A_input_size+3)/4, TID_0);
	if (!DataToAccelDone(slot)) die("DataToAccelDone(%d)", slot);

	DataToAccel(slot, B_OFFSET_MEM, (B_input_size+3)/4, TID_0);
	if (!DataToAccelDone(slot)) die("DataToAccelDone(%d)", slot);

	DataFromAccel(slot, C_OFFSET_MEM, 1);
	if (!DataFromAccelDone(slot)) die("DataFromAccelDone(%d)", slot);
	const int N = *((float*)vptr+C_OFFSET), M = *((float*)vptr+C_OFFSET+1);
	output_size = N*M + 2;

	DataFromAccel(slot, C_OFFSET_MEM+16, (output_size-1)/4);
	if (!DataFromAccelDone(slot)) die("DataFromAccelDone(%d)", slot);
}
auto end_time = std::chrono::high_resolution_clock::now();
auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
double average_ms = static_cast<double>(duration_ns.count()) / 100 / 1000000;
printf("Average time taken: %.6f ms\n", average_ms);

	printf("\t Success: Selected Operation Done !.\n");
	memcpy(C, vptr+C_OFFSET, output_size*4);
	if (writeStream(argv[3], C, output_size)) die("writeStream");

	FinaliseUnmapRMs(slot);
	return 0;
}
