#include "ap_int.h"

void mul_dvd(ap_uint<128>* in1, ap_uint<128>* in2, ap_uint<128>* out, ap_uint<1> opcode) {
#pragma HLS INTERFACE ap_none port=opcode
#pragma HLS INTERFACE axis port=in1
#pragma HLS INTERFACE axis port=in2
#pragma HLS INTERFACE axis port=out
	ap_uint<128> result;
	if (opcode == 1) {
		result(127,96) = (*in1)(127,96) * (*in2)(127,96);
		result(95,64) = (*in1)(95,64) * (*in2)(95,64);
		result(63,32) = (*in1)(63,32) * (*in2)(63,32);
		result(31,0) = (*in1)(31,0) * (*in2)(31,0);
	}
	else {
		result(127,96) = (*in1)(127,96) / (*in2)(127,96);
		result(95,64) = (*in1)(95,64) / (*in2)(95,64);
		result(63,32) = (*in1)(63,32) / (*in2)(63,32);
		result(31,0) = (*in1)(31,0) / (*in2)(31,0);
	}
	*out = result;
}
