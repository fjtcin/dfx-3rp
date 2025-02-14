#include <hls_stream.h>

constexpr int N = 4;
constexpr int K = 4;
constexpr int M = 4;

struct pack {
	float num1;
	float num2;
	float num3;
	float num4;
};

void gemm(hls::stream<pack>& a_s, hls::stream<pack>& b_s, hls::stream<pack>& c_s) {
#pragma HLS INTERFACE axis port=a_s
#pragma HLS INTERFACE axis port=b_s
#pragma HLS INTERFACE axis port=c_s

	float a[N][K], b[K][M];

	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < K; j += 4) {
			pack tmp = a_s.read();
			a[i][j] = tmp.num1;
			a[i][j+1] = tmp.num2;
			a[i][j+2] = tmp.num3;
			a[i][j+3] = tmp.num4;
		}
	}

	for (int i = 0; i < K; ++i) {
		for (int j = 0; j < M; j += 4) {
			pack tmp = b_s.read();
			b[i][j] = tmp.num1;
			b[i][j+1] = tmp.num2;
			b[i][j+2] = tmp.num3;
			b[i][j+3] = tmp.num4;
		}
	}

	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < M; j += 4) {
			pack sum;
			sum.num1 = 0;
			sum.num2 = 0;
			sum.num3 = 0;
			sum.num4 = 0;
			for (int k = 0; k < K; ++k) {
				sum.num1 += a[i][k] * b[k][j];
				sum.num2 += a[i][k] * b[k][j+1];
				sum.num3 += a[i][k] * b[k][j+2];
				sum.num4 += a[i][k] * b[k][j+3];
			}
			c_s.write(sum);
		}
	}
}
