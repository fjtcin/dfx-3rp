#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

#define SIZE_IN_BYTES       0x4000000	//64MB


#define A_OFFSET			0
#define B_OFFSET			32
#define RESULT_OFFSET		128
#define A_OFFSET_MEM		0x0
#define B_OFFSET_MEM		0x80
#define RESULT_OFFSET_MEM	0x200
#define INPUT_SIZE			0x4
#define TID_0				0x0
#define TID_2				0x2

int InitializeMapRMs(int slot);
int StartAccel(int slot);
int FinaliseUnmapRMs(int slot);
void mapBuffer(xrt::bo boa);
int DataToAccel(int slot, uint64_t data, uint64_t size, uint8_t tid);
int DataFromAccel(int slot, uint64_t data, uint64_t size);
int DataToAccelDone(int slot);
int DataFromAccelDone(int slot);

// A Input Buffer
float A[] = {
	1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 0.1, 1.2, 2.3, 3.4
};

// B Input Buffer
float B[] = {
	1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1
};

// C Output Buffer
float C[16];


int main(int argc, char *argv[]) {
	//Default slot Set to 2 unless passed as an argument
	int slot = 2;
	if (argc > 1) {
		//Updating slot number provided as command line argument
		slot = atoi(argv[1]);
		if (slot != 2) {
			printf("- Invalid slot number provided %s. Valid values : 2\n", argv[1]);
			return 0;
		}
	}
	//Initialize and memory map RMs
	if (InitializeMapRMs(slot) == -1) {
		printf("- Check the slot number where the accelerator is loaded and run the test on the specific slot.\n");
		return 0;
	}
	//Allocate XRT buffer to be used for input and output of the application
	auto device = xrt::device(0);
	auto bufferObject = xrt::bo(device, SIZE_IN_BYTES, 0);
	uint32_t* vptr = (uint32_t*)bufferObject.map<int*>();
	mapBuffer(bufferObject);

	memcpy(vptr+A_OFFSET, A, sizeof(A));
	memcpy(vptr+B_OFFSET, B, sizeof(B));

	//Initialize RM
	StartAccel(slot);
	//Program A to Accelerator
	DataToAccel(slot, A_OFFSET_MEM, INPUT_SIZE, TID_0);
	int status = DataToAccelDone(slot);
	//Program B to Accelerator
	if (status) {
		DataToAccel(slot, B_OFFSET_MEM, INPUT_SIZE, TID_2);
		status = DataToAccelDone(slot);
	}
	if (status) {
		DataFromAccel(slot, RESULT_OFFSET_MEM, INPUT_SIZE);
		status = DataFromAccelDone(slot);
		printf("\t Success: Selected Operation Done !.\n");
		memcpy(C, vptr+RESULT_OFFSET, sizeof(C));
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				printf("%f ", C[i*4 + j]);
			}
			putchar('\n');
		}
	}
	if (status) {
		FinaliseUnmapRMs(slot);
	}
	return 0;
}
