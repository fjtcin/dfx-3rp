#include <hls_stream.h>
#include "csr_dec.h"

float A[24] = {
	4, 6, 8,
	10, 20, 30, 40, 50, 60, 70, 80,
	0, 1, 1, 3, 2, 3, 4, 5,
	0, 2, 4, 7, 8
};

int main(void) {
	hls::stream<pack> in;
	hls::stream<pack> out;

	for (int i = 0; i < 24; i += 4) {
		pack tmp{A[i], A[i+1], A[i+2], A[i+3]};
		in.write(tmp);
	}

	csr_dec(in, out);

	for (int i = 0; i < 28; i += 4) {
		pack tmp = out.read();
		printf("%f %f %f %f\n", tmp.num0, tmp.num1, tmp.num2, tmp.num3);
	}
	return 0;
}
