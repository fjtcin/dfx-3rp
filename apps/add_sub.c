/*
 * Copyright (C) 2022, Advanced Micro Devices, Inc. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

#define SIZE_IN_BYTES       0x4000000	//64MB


#define A_OFFSET			0 		//Decryption Key Offset at 0
#define B_OFFSET			8 		//Encryption Key Offset at 32
#define OPERATION_OFFSET	64 		        //Encrypted Buffer Offset at 256
#define RESULT_OFFSET		128		        //Decryped Buffer Offset at 512
#define A_OFFSET_MEM		0x0 	                //Decryption Key Mem Offset in Hex
#define B_OFFSET_MEM		0x20	                //Encryption Key Mem Offset in Hex
#define OPERATION_OFFSET_MEM		0x100 	        //Encrypted Buffer Mem Offset in Hex
#define RESULT_OFFSET_MEM	0x200	                //Result Buffer Mem Offset in Hex
#define INPUT_SIZE			0x2		//Size of Input Data
#define OPERATION_SIZE		0x1		        //Size of Operation
#define TID_0				0x0		//TID 0
#define TID_1				0x1		//TID 1
#define TID_2				0x2		//TID 2

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
uint32_t OperationAdd[] = {
	0x00000001, 0x00000000, 0x00000000, 0x00000000
};

uint32_t OperationSub[] = {
	0x00000000, 0x00000000, 0x00000000, 0x00000000
};


int main(int argc, char *argv[])
{
	//Default slot Set to 0 unless passed as an argument
	int slot =0;
	if(argc>1)
	{
		//Updating slot number provided as command line argument
		slot = atoi (argv[1]);
		if (slot != 2 && slot != 1 && slot != 0)
		{
			printf("- Invalid slot number provided %s. Valid values : 0, 1, 2\n", argv[1]);
			return 0;
		}
	}
	//Initialize and memory map RMs
	if(InitializeMapRMs(slot) == -1)
	{
		printf("- Check the slot number where the accelerator is loaded and run the test on the specific slot.\n");
		return 0;
	}
	//Allocate XRT buffer to be used for input and output of the application
	auto device = xrt::device(0);
	auto bufferObject = xrt::bo(device, SIZE_IN_BYTES, 0);
	uint32_t *vptr = (uint32_t *)bufferObject.map<int*>();
	mapBuffer(bufferObject);

	// Write A of Size 32 bytes (4bytes x 8 )
	std::memcpy(vptr+A_OFFSET, &A, sizeof(A));
	// Write B of Size 32 bytes (4bytes x 8 )
	std::memcpy(vptr+B_OFFSET, &B, sizeof(B));
	// Write Add Operation
	std::memcpy(vptr+OPERATION_OFFSET, &OperationAdd, sizeof(OperationAdd));
	// Write Sub Operation
	//std::memcpy(vptr+OPERATION_OFFSET, &OperationSub, sizeof(OperationSub));

	//Initialize RM
	StartAccel(slot);
	//Program A to Accelerator - Size 32 bytes (16bytes x 2 ) to Offset 0 (0x0)
	DataToAccel(slot,A_OFFSET_MEM,INPUT_SIZE,TID_0);
	int status = DataToAccelDone(slot);
	//Program B to Accelerator - Size 32 bytes (16bytes x 2 ) to Offset 8 (0x20)
	if(status)
	{
		DataToAccel(slot,OPERATION_OFFSET_MEM,OPERATION_SIZE,TID_1);
		status = DataToAccelDone(slot);
	}
	//Program Operation to Accelerator - Size 16 bytes (16bytes x 1 ) to Offset 64 (0x100)
	if(status)
	{
		DataToAccel(slot,B_OFFSET_MEM,INPUT_SIZE,TID_2);
		status = DataToAccelDone(slot);
	}
	if(status)
	{
		//DataFromAccel - Size 16 bytes (16bytes x 1 ) to Offset 128 (0x200)
		DataFromAccel(slot, RESULT_OFFSET_MEM, INPUT_SIZE);
		status = DataFromAccelDone(slot);
		printf("\t Success: Selected Operation Done !.\n");
		printf("A\t[%u, %u, %u, %u, %u, %u, %u, %u]\n", A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7]);
		printf("B\t[%u, %u, %u, %u, %u, %u, %u, %u]\n", B[0], B[1], B[2], B[3], B[4], B[5], B[6], B[7]);
		std::memcpy(C, vptr+RESULT_OFFSET, sizeof(C));
		printf("C\t[%u, %u, %u, %u, %u, %u, %u, %u]\n", C[0], C[1], C[2], C[3], C[4], C[5], C[6], C[7]);
	}
	if(status)
	{
		FinaliseUnmapRMs(slot);
	}
	return 0;
}
