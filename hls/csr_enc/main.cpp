#include <math.h>
#include <assert.h>
#include <ap_int.h>
#include <hls_stream.h>
#include "csr_enc.h"

#define MAX_SIZE 16384
#define MSIZE 4096

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

void outputMatrix(hls::stream<pack>& out, const float* data, const int N, const int M, const int L) {
	pack tmp;
	tmp.num0 = N;
	tmp.num1 = M;
	tmp.num2 = L;
	ap_uint<2> p = 3;

	for (int i = 0; i < 2*L + N + 1; ++i) {
		float num = data[i];
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
	out.write(tmp);
}

void csr_enc(hls::stream<pack>& in, hls::stream<pack>& out) {
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out

	float a[MAX_SIZE];
	float data[MSIZE];
	int N, M, L;

	inputMatrix(in, a, N, M, L);

	float* indices = data + L;
	float* indptr = data + 2*L;
	int nnz = 0;
	for (int i = 0; i < N; ++i) {
		indptr[i] = nnz;
		for (int j = 0; j < M; ++j) {
			float num = a[i*M + j];
			if (fabsf(num) > 1e-6) {
				data[nnz] = num;
				indices[nnz] = j;
				++nnz;
			}
		}
	}
	assert(L == nnz);
	indptr[N] = L;

	outputMatrix(out, data, N, M, L);
}
