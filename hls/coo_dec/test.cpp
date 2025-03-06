#include <hls_stream.h>
#include "coo_dec.h"

float A[20] = {
	4, 3, 5,
	0, 0, 1,
	0, 1, 2,
	1, 2, 3,
	2, 0, 4,
	2, 2, 5
};

int main(void) {
	hls::stream<pack> in;
	hls::stream<pack> out;

	for (int i = 0; i < 20; i += 4) {
		pack tmp{A[i], A[i+1], A[i+2], A[i+3]};
		in.write(tmp);
	}

	coo_dec(in, out);

	for (int i = 0; i < 16; i += 4) {
		pack tmp = out.read();
		printf("%f %f %f %f\n", tmp.num0, tmp.num1, tmp.num2, tmp.num3);
	}
	return 0;
}
