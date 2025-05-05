#include <assert.h>
#include <ap_int.h>
#include <hls_stream.h>
#include "gemm.h"

#define MAX_SIZE 128

void inputMatrix(hls::stream<pack>& in, float* matrix, int& N, int& M) {
	pack tmp = in.read();
	N = tmp.num0;
	M = tmp.num1;
	ap_uint<2> p = 2;

	for (int i = 0; i < N*M; ++i) {
		switch (p) {
			case 0:
				tmp = in.read();
				matrix[i] = tmp.num0;
				break;
			case 1:
				matrix[i] = tmp.num1;
				break;
			case 2:
				matrix[i] = tmp.num2;
				break;
			case 3:
				matrix[i] = tmp.num3;
				break;
		}
		++p;
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

void gemm(hls::stream<pack>& in, hls::stream<pack>& out) {
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out

	float a[MAX_SIZE], b[MAX_SIZE], c[MAX_SIZE];
	int N, M, K, K2;

	inputMatrix(in, a, N, K);
	inputMatrix(in, b, K2, M);
	assert(K == K2);

	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < M; ++j) {
			float sum = 0;
			for (int k = 0; k < K; ++k) {
				sum += a[i*K + k] * b[k*M + j];
			}
			c[i*M + j] = sum;
		}
	}

	outputMatrix(out, c, N, M);
}
