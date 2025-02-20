#include <ap_int.h>
#include <hls_stream.h>

void mux_3_1(ap_uint<2>& opcode, hls::stream<ap_uint<128>>& in0, hls::stream<ap_uint<128>>& in1, hls::stream<ap_uint<128>>& in2, hls::stream<ap_uint<128>>& out) {
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE ap_none port=opcode
#pragma HLS INTERFACE axis port=in0
#pragma HLS INTERFACE axis port=in1
#pragma HLS INTERFACE axis port=in2
#pragma HLS INTERFACE axis port=out

	switch (opcode) {
		case 1:
			out.write(in1.read());
			break;
		case 2:
			out.write(in2.read());
			break;
		default:
			out.write(in0.read());
			break;
	}
}
