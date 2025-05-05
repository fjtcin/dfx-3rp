#include <assert.h>
#include <ap_int.h>
#include <hls_stream.h>
#include "csc_dec.h"

#define MAX_SIZE 16384
#define MSIZE 4096

void inputMatrix(hls::stream<pack>& in, float* data, int& N, int& M, int& L) {
	pack tmp = in.read();
	N = tmp.num0;
	M = tmp.num1;
	L = tmp.num2;
	ap_uint<2> p = 3;

	for (int i = 0; i < 2*L + M + 1; ++i) {
		switch (p) {
			case 0:
				tmp = in.read();
				data[i] = tmp.num0;
				break;
			case 1:
				data[i] = tmp.num1;
				break;
			case 2:
				data[i] = tmp.num2;
				break;
			case 3:
				data[i] = tmp.num3;
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

void csc_dec(hls::stream<pack>& in, hls::stream<pack>& out) {
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out

	float a[MAX_SIZE] = {0};
	float data[MSIZE];
	int N, M, L;

	inputMatrix(in, data, N, M, L);

	float* indices = data + L;
	float* indptr = data + 2*L;
	for (int i = 0; i < M; ++i) {
		for (int k = (int)indptr[i]; k < (int)indptr[i+1]; ++k) {
			int j = indices[k];
			assert(j >= 0 && j < N);
			a[j*M + i] = data[k];
		}
	}

	outputMatrix(out, a, N, M);
}
