#include <assert.h>
#include <ap_int.h>
#include <hls_stream.h>
#include "coo_enc.h"

#define MAX_SIZE 128

void inputMatrix(hls::stream<pack>& in, float* matrix, int& N, int& M, int& L) {
	pack tmp = in.read();
	N = tmp.num0;
	M = tmp.num1;
	L = 0;
	int st = 0;
	const int ed = N * M;
	ap_uint<2> p = 2;

	while (st < ed) {
		float num;
		switch (p) {
			case 0:
				tmp = in.read();
				num = tmp.num0;
				break;
			case 1:
				num = tmp.num1;
				break;
			case 2:
				num = tmp.num2;
				break;
			case 3:
				num = tmp.num3;
				break;
		}
		matrix[st] = num;
		if (num) ++L;
		++p;
		++st;
	}
}

void outputMatrix(hls::stream<pack>& out, const float* matrix, const int N, const int M, const int L) {
	pack tmp;
	tmp.num0 = N;
	tmp.num1 = M;
	tmp.num2 = L;
	ap_uint<2> p = 3;

	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < M; ++j) {
			float num = matrix[i*M + j];
			if (num) {
				// row
				switch (p) {
					case 0:
						out.write(tmp);
						tmp.num0 = i;
						break;
					case 1:
						tmp.num1 = i;
						break;
					case 2:
						tmp.num2 = i;
						break;
					case 3:
						tmp.num3 = i;
						break;
				}
				++p;
				// col
				switch (p) {
					case 0:
						out.write(tmp);
						tmp.num0 = j;
						break;
					case 1:
						tmp.num1 = j;
						break;
					case 2:
						tmp.num2 = j;
						break;
					case 3:
						tmp.num3 = j;
						break;
				}
				++p;
				// row
				switch (p) {
					case 0:
						out.write(tmp);
						tmp.num0 = num;
						break;
					case 1:
						tmp.num1 = num;
						break;
					case 2:
						tmp.num2 = num;
						break;
					case 3:
						tmp.num3 = num;
						break;
				}
				++p;
			}
		}
	}
	out.write(tmp);
}

void coo_enc(hls::stream<pack>& in, hls::stream<pack>& out) {
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out

	float a[MAX_SIZE];
	int N, M, L;

	inputMatrix(in, a, N, M, L);
	outputMatrix(out, a, N, M, L);
}
