#include <assert.h>
#include <ap_int.h>
#include <hls_stream.h>
#include "gemm.h"
#include "mmult.h"

#define MSIZE 16384

void inputMatrix(hls::stream<pack>& in, float* matrix, int& N, int& M) {
	pack tmp = in.read();
	N = tmp.num0;
	M = tmp.num1;
	int st = 0;
	const int ed = N * M;
	ap_uint<2> p = 2;

	while (st < ed) {
		switch (p) {
			case 0:
				tmp = in.read();
				matrix[st] = tmp.num0;
				break;
			case 1:
				matrix[st] = tmp.num1;
				break;
			case 2:
				matrix[st] = tmp.num2;
				break;
			case 3:
				matrix[st] = tmp.num3;
				break;
		}
		++p;
		++st;
	}
}

void outputMatrix(hls::stream<pack>& out, const float* matrix, const int N, const int M) {
	pack tmp;
	tmp.num0 = N;
	tmp.num1 = M;
	int st = 0;
	const int ed = N * M;
	ap_uint<2> p = 2;

	while (st < ed) {
		switch (p) {
			case 0:
				out.write(tmp);
				tmp.num0 = matrix[st];
				break;
			case 1:
				tmp.num1 = matrix[st];
				break;
			case 2:
				tmp.num2 = matrix[st];
				break;
			case 3:
				tmp.num3 = matrix[st];
				break;
		}
		++p;
		++st;
	}
	out.write(tmp);
}

void gemm(hls::stream<pack>& in, hls::stream<pack>& out) {
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out

	float a[MSIZE], b[MSIZE], c[MSIZE];
	int N, M, K, K2;

	inputMatrix(in, a, N, K);
	inputMatrix(in, b, K2, M);
	assert(K == K2);

	mmult(a, b, c, N, K, M);

	outputMatrix(out, c, N, M);
}
