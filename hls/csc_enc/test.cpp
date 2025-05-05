#include <hls_stream.h>
#include "csc_enc.h"

float A[16] = {
	4, 3,
	1, 0, 3,
	0, 2, 0,
	1, 0, 0,
	0, 0, 3
};

int main(void) {
	hls::stream<pack> in;
	hls::stream<pack> out;

	for (int i = 0; i < 16; i += 4) {
		pack tmp{A[i], A[i+1], A[i+2], A[i+3]};
		in.write(tmp);
	}

	csc_enc(in, out);

	for (int i = 0; i < 20; i += 4) {
		pack tmp = out.read();
		printf("%f %f %f %f\n", tmp.num0, tmp.num1, tmp.num2, tmp.num3);
	}
	return 0;
}
