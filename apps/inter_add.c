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
#define B_OFFSET			8
#define OPERATION_OFFSET	64
#define INTER_OFFSET        96
#define RESULT_OFFSET		128
#define A_OFFSET_MEM		0x0
#define B_OFFSET_MEM		0x20
#define OPERATION_OFFSET_MEM		0x100
#define INTER_OFFSET_MEM    0x180
#define RESULT_OFFSET_MEM	0x200
#define INPUT_SIZE			0x2
#define OPERATION_SIZE		0x1
#define TID_0				0x0
#define TID_1				0x1
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
uint32_t A[] = {
	0x55555555, 0x44444444, 0x33333333, 0x22222222,
	0x55555555, 0x44444444, 0x33333333, 0x22222222
};

// B Input Buffer
uint32_t B[] = {
	0x11111111, 0x11111111, 0x11111111, 0x11111111,
	0x11111111, 0x11111111, 0x11111111, 0x11111111
};

// C Output Buffer
uint32_t C[8];

// Operation
uint32_t Operation[] = {
	0x00000001, 0x00000000, 0x00000000, 0x00000000
};


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

	// Write A of Size 32 bytes (4bytes x 8 )
	memcpy(vptr+A_OFFSET, A, sizeof(A));
	// Write B of Size 32 bytes (4bytes x 8 )
	memcpy(vptr+B_OFFSET, B, sizeof(B));
	// Write Operation
	memcpy(vptr+OPERATION_OFFSET, Operation, sizeof(Operation));

	DataToAccel(slot0, A_OFFSET_MEM, INPUT_SIZE, TID_0);
	int status = DataToAccelDone(slot0);
	if (status) {
		DataToAccel(slot0, OPERATION_OFFSET_MEM, OPERATION_SIZE, TID_1);
		status = DataToAccelDone(slot0);
	}
	if (status) {
		DataToAccel(slot0, B_OFFSET_MEM, INPUT_SIZE, TID_2);
		status = DataToAccelDone(slot0);
	}
	if (status) {
		DataFromAccel(slot0, INTER_OFFSET_MEM, INPUT_SIZE);
		status = DataFromAccelDone(slot0);
	}

	if (status) {
		DataToAccel(slot1, INTER_OFFSET_MEM, INPUT_SIZE, TID_0);
		status = DataToAccelDone(slot1);
	}
	if (status) {
		DataToAccel(slot1, OPERATION_OFFSET_MEM, OPERATION_SIZE, TID_1);
		status = DataToAccelDone(slot1);
	}
	if (status) {
		DataToAccel(slot1, B_OFFSET_MEM, INPUT_SIZE, TID_2);
		status = DataToAccelDone(slot1);
	}
	if (status) {
		DataFromAccel(slot1, RESULT_OFFSET_MEM, INPUT_SIZE);
		status = DataFromAccelDone(slot1);
	}

	if (status) {
		memcpy(C, vptr+RESULT_OFFSET, sizeof(C));
		printf("C\t[%x, %x, %x, %x, %x, %x, %x, %x]\n", C[0], C[1], C[2], C[3], C[4], C[5], C[6], C[7]);
		FinaliseUnmapRMs(slot0);
		FinaliseUnmapRMs(slot1);
	}
	return 0;
}
