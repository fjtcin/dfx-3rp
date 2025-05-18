#include "lib/stream_utils.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>


#define die(fmt, args ...) do { fprintf(stderr, \
	"ERROR:%s():%u " fmt ": %s\n", \
	__func__, __LINE__, ##args, errno ? strerror(errno) : ""); \
	exit(EXIT_FAILURE); \
} while (0)

// A Input Buffer
float A[16384+2];
int A_input_size = 16384+2;

// B Input Buffer
float B[16384+2];
int B_input_size = 16384+2;

// C Output Buffer
float C[16384+2];
int output_size;


int main(int argc, char** argv) {
	if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_filename> <input_filename> <output_filename>\n", argv[0]);
        fprintf(stderr, "Example: %s A.txt B.txt C.txt\n", argv[0]);
        return 1; // Return an error code
    }
	if (readStream(argv[1], A, A_input_size)) die("readStream");
	if (readStream(argv[2], B, B_input_size)) die("readStream");

	float a[16384], b[16384], c[16384];
	const int N = A[0], K = A[1], M = B[1];
	memcpy(a, A+2, (A_input_size-2)*4);
	memcpy(b, B+2, (B_input_size-2)*4);

auto start_time = std::chrono::high_resolution_clock::now();
for (int ii = 0; ii < 100; ++ii) {
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < M; ++j) {
			float sum = 0;
			for (int k = 0; k < K; ++k) {
				sum += a[i*K + k] * b[k*M + j];
			}
			c[i*M + j] = sum;
		}
	}
	output_size = N*M + 2;
}
auto end_time = std::chrono::high_resolution_clock::now();
auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
double average_ms = static_cast<double>(duration_ns.count()) / 100 / 1000000;
printf("Average time taken: %.6f ms\n", average_ms);

	printf("\t Success: Selected Operation Done !.\n");
	C[0] = N; C[1] = M;
	memcpy(C+2, c, (output_size-2)*4);
	if (writeStream(argv[3], C, output_size)) die("writeStream");

	return 0;
}
