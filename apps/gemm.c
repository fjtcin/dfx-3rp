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

#define A_OFFSET			64
#define RESULT_OFFSET		128
#define A_OFFSET_MEM		0x100
#define RESULT_OFFSET_MEM	0x200
#define INPUT_SIZE			0x8
#define OUTPUT_SIZE			0x5
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
float A[] = {
	4, 3,
	1, 2, 3,
	3, 2, 1,
	1, 2, 3,
	3, 2, 1,
	0, 0,
	3, 4,
	3, 4, 5, 6,
	7, 6, 5, 4,
	2, 4, 6, 5
};

// C Output Buffer
float C[4*OUTPUT_SIZE];


int main(void) {
	int slot = 2;
	//Initialize and memory map RMs
	if (InitializeMapRMs(slot) == -1) {
		printf("- Check the slot number where the accelerator is loaded and run the test on the specific slot.\n");
		return 0;
	}
	StartAccel(slot);

	//Allocate XRT buffer to be used for input and output of the application
	auto device = xrt::device(0);
	auto bufferObject = xrt::bo(device, SIZE_IN_BYTES, 0);
	uint32_t* vptr = (uint32_t*)bufferObject.map<int*>();
	mapBuffer(bufferObject);

	memcpy(vptr+MUX_OFFSET, &Mux, sizeof(Mux));
	memcpy(vptr+A_OFFSET, A, sizeof(A));

	DataToAccel(slot, MUX_OFFSET_MEM, 1, TID_1);
	if (!DataToAccelDone(slot)) die("DataToAccelDone(%d)", slot);

	//Program A to Accelerator
	DataToAccel(slot, A_OFFSET_MEM, INPUT_SIZE, TID_0);
	if (!DataToAccelDone(slot)) die("DataToAccelDone(%d)", slot);

	DataFromAccel(slot, RESULT_OFFSET_MEM, OUTPUT_SIZE);
	if (!DataFromAccelDone(slot)) die("DataFromAccelDone(%d)", slot);

	printf("\t Success: Selected Operation Done !.\n");
	memcpy(C, vptr+RESULT_OFFSET, sizeof(C));
	for (int i = 0; i < 4*OUTPUT_SIZE; ++i) {
		printf("%f ", C[i]);
	}
	putchar('\n');

	FinaliseUnmapRMs(slot);
	return 0;
}
