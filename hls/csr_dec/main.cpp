#include <assert.h>
#include <ap_int.h>
#include <hls_stream.h>
#include "csr_dec.h"

#define MAX_SIZE 16384
#define MSIZE 4096

void inputMatrix(hls::stream<pack>& in, float* data, int& N, int& M, int& L) {
	pack tmp = in.read();
	N = tmp.num0;
	M = tmp.num1;
	L = tmp.num2;
	int st = 0;
	const int ed = 2*L + N + 1;
	ap_uint<2> p = 3;

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
		++p;
		++st;
		*data++ = num;
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

void csr_dec(hls::stream<pack>& in, hls::stream<pack>& out) {
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out

	float a[MAX_SIZE] = {0};
	float data[MSIZE];
	int N, M, L;

	inputMatrix(in, data, N, M, L);

	float* indices = data + L;
	float* indptr = data + 2*L;
	for (int i = 0; i < N; ++i)
		for (int k = (int)indptr[i]; k < (int)indptr[i+1]; ++k)
			a[i*M + (int)indices[k]] = data[k];

	outputMatrix(out, a, N, M);
}
