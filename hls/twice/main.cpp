#include <hls_stream.h>

struct pack {
	float num1;
	float num2;
	float num3;
	float num4;
};

void twice(hls::stream<pack>& in, hls::stream<pack>& out) {
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out

	pack tmp = in.read();
	tmp.num1 *= 2;
	tmp.num2 *= 2;
	tmp.num3 *= 2;
	tmp.num4 *= 2;
	out.write(tmp);
}
