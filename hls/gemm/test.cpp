#include <hls_stream.h>
#include "gemm.h"

float A[32] = {
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

int main(void) {
	hls::stream<pack> in;
	hls::stream<pack> out;

	for (int i = 0; i < 32; i += 4) {
		pack tmp{A[i], A[i+1], A[i+2], A[i+3]};
		in.write(tmp);
	}

	gemm(in, out);

	for (int i = 0; i < 20; i += 4) {
		pack tmp = out.read();
		printf("%f %f %f %f\n", tmp.num0, tmp.num1, tmp.num2, tmp.num3);
	}
	return 0;
}
