#include <assert.h>
#include <ap_int.h>
#include <hls_stream.h>
#include "coo_dec.h"

#define MAX_SIZE 16384

void inputMatrix(hls::stream<pack>& in, float* matrix, int& N, int& M) {
	pack tmp = in.read();
	N = tmp.num0;
	M = tmp.num1;
	int L = tmp.num2;
	ap_uint<2> p = 3, q = 0;
	int row, col;

	for (int i = 0; i < 3*L; ++i) {
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
		++p;
		switch (q) {
			case 0:
				q = 1;
				row = num;
				break;
			case 1:
				q = 2;
				col = num;
				break;
			default:
				q = 0;
				assert(row >= 0 && row < N && col >= 0 && col < M);
				matrix[row*M + col] = num;
				break;
		}
	}
}

void outputMatrix(hls::stream<pack>& out, const float* matrix, const int N, const int M) {
	pack tmp;
	tmp.num0 = N;
	tmp.num1 = M;
	ap_uint<2> p = 2;

	for (int i = 0; i < N*M; ++i) {
		switch (p) {
			case 0:
				out.write(tmp);
				tmp.num0 = matrix[i];
				break;
			case 1:
				tmp.num1 = matrix[i];
				break;
			case 2:
				tmp.num2 = matrix[i];
				break;
			case 3:
				tmp.num3 = matrix[i];
				break;
		}
		++p;
	}
	out.write(tmp);
}

void coo_dec(hls::stream<pack>& in, hls::stream<pack>& out) {
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out

	float a[MAX_SIZE] = {0};
	int N, M;

	inputMatrix(in, a, N, M);
	outputMatrix(out, a, N, M);
}
