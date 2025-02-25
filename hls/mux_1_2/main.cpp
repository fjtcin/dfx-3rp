#include <ap_int.h>
#include <hls_stream.h>

void mux_1_2(const ap_uint<2> opcode, hls::stream<ap_uint<128>>& in, hls::stream<ap_uint<128>>& out0, hls::stream<ap_uint<128>>& out1) {
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE ap_none port=opcode
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out0
#pragma HLS INTERFACE axis port=out1

	switch (opcode) {
		case 1:
		case 3:
			out1.write(in.read());
			break;
		default:
			out0.write(in.read());
			break;
	}
}
