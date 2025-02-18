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
#define RESULT_OFFSET		128
#define A_OFFSET_MEM		0x0
#define RESULT_OFFSET_MEM	0x200
#define INPUT_SIZE			0x4
#define TID_0				0x0

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

// C Output Buffer
float C[16];


int main(void) {
	int slot0 = 0;
	int slot1 = 1;
	//Initialize and memory map RMs
	if (InitializeMapRMs(slot0) == -1) {
		printf("- Check the slot number where the accelerator is loaded and run the test on the specific slot.\n");
		return 0;
	}
	StartAccel(slot0);
	if (InitializeMapRMs(slot1) == -1) {
		printf("- Check the slot number where the accelerator is loaded and run the test on the specific slot.\n");
		return 0;
	}
	StartAccel(slot1);

	//Allocate XRT buffer to be used for input and output of the application
	auto device = xrt::device(0);
	auto bufferObject = xrt::bo(device, SIZE_IN_BYTES, 0);
	uint32_t* vptr = (uint32_t*)bufferObject.map<int*>();
	mapBuffer(bufferObject);

	memcpy(vptr+A_OFFSET, A, sizeof(A));

	DataToAccel(slot0, A_OFFSET_MEM, INPUT_SIZE, TID_0);
	int status = DataToAccelDone(slot0);
	if (status) {
		DataFromAccel(slot1, RESULT_OFFSET_MEM, INPUT_SIZE);
		status = DataFromAccelDone(slot1);
	}
	if (status) {
		printf("\t Success: Selected Operation Done !.\n");
		memcpy(C, vptr+RESULT_OFFSET, sizeof(C));
		for (int i = 0; i < 16; ++i) {
			printf("%f ", C[i]);
		}
		putchar('\n');
		FinaliseUnmapRMs(slot0);
		FinaliseUnmapRMs(slot1);
	}
	return 0;
}
