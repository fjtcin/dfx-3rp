#include <algorithm>

#define TILE_SIZE 12

void mmult(const float* a, // Read-Only Full Matrix A
		   const float* b, // Read-Only Full Matrix B
		   float* c,       // Output Full Matrix C
		   int A_ROWS,     // Full Matrix A Row Size
		   int A_COLS,     // Full Matrix A Col Size (also B Row Size)
		   int B_COLS      // Full Matrix B Col Size
		   ) {
	// Local memory to store tiles of input and output matrices
	float localA[TILE_SIZE][TILE_SIZE];
#pragma HLS ARRAY_PARTITION variable = localA dim = 1 complete

	float localB[TILE_SIZE][TILE_SIZE];
#pragma HLS ARRAY_PARTITION variable = localB dim = 2 complete

	float localC[TILE_SIZE][TILE_SIZE];
#pragma HLS ARRAY_PARTITION variable = localC dim = 0 complete

	// Iterate over the rows of matrix A and C in tile steps
	for (int i_outer = 0; i_outer < A_ROWS; i_outer += TILE_SIZE) {
		int current_tile_A_rows = std::min(A_ROWS - i_outer, TILE_SIZE);
		// Iterate over the columns of matrix B and C in tile steps
		for (int j_outer = 0; j_outer < B_COLS; j_outer += TILE_SIZE) {
			int current_tile_B_cols = std::min(B_COLS - j_outer, TILE_SIZE);
			// Iterate over the shared dimension (A_COLS or B_ROWS) in tile steps
			for (int k_outer = 0; k_outer < A_COLS; k_outer += TILE_SIZE) {
				int current_tile_K_dim = std::min(A_COLS - k_outer, TILE_SIZE);

			// Load tile of A from global memory to localA, padding with 0 if necessary
			// Auto-pipeline is going to apply pipeline to these loops
			readA:
				for (int r_a = 0; r_a < TILE_SIZE; ++r_a) {
					for (int c_a = 0; c_a < TILE_SIZE; ++c_a) {
						if (r_a < current_tile_A_rows && c_a < current_tile_K_dim) {
							localA[r_a][c_a] = a[(i_outer + r_a) * A_COLS + (k_outer + c_a)];
						} else {
							localA[r_a][c_a] = 0; // Pad with zero
						}
					}
				}

			// Load tile of B from global memory to localB, padding with 0 if necessary
			readB:
				for (int r_b = 0; r_b < TILE_SIZE; ++r_b) { // r_b corresponds to K-dimension
					for (int c_b = 0; c_b < TILE_SIZE; ++c_b) {
						if (r_b < current_tile_K_dim && c_b < current_tile_B_cols) {
							localB[r_b][c_b] = b[(k_outer + r_b) * B_COLS + (j_outer + c_b)];
						} else {
							localB[r_b][c_b] = 0; // Pad with zero
						}
					}
				}

// Perform systolic matrix multiply
// local matrices localA and localB have been partitioned in dimensions
// 1 and 2 respectively. local matrix C has been partitioned completely

// This partitioning enables to access MAX_SIZE elements in parallel in
// the local matrices. Because of the mode of access of array elements,
// we are able to perform MAX_SIZE*MAX_SIZE operations in parallel.

// Note : i, j and k loops are interchanged.

// The top loop systolic1 runs only for a_col iterations instead of
// MAX_SIZE like the inner loops. The inner loops have fixed loop
// iteration counts to enable complete unroll

// The following diagram explains how the matrix multiply happens
//
//        B_0        B_1        B_2        B_3
//         |          |          |          |
//         v          v          v          v
//        ___        ___        ___        ___
//       |   |      |   |      |   |      |   |
//  A0_->|C00| ---- |C01| ---- |C02| ---- |C03|
//       |___|      |___|      |___|      |___|
//         |          |          |          |
//        ___        ___        ___        ___
//       |   |      |   |      |   |      |   |
//  A1_->|C10| ---- |C11| ---- |C12| ---- |C13|
//       |___|      |___|      |___|      |___|
//         |          |          |          |
//        ___        ___        ___        ___
//       |   |      |   |      |   |      |   |
//  A2_->|C20| ---- |C21| ---- |C21| ---- |C21|
//       |___|      |___|      |___|      |___|
//         |          |          |          |
//        ___        ___        ___        ___
//       |   |      |   |      |   |      |   |
//  A3_->|C30| ---- |C31| ---- |C32| ---- |C33|
//       |___|      |___|      |___|      |___|

			systolic1:
				for (int k = 0; k < current_tile_K_dim; ++k) {
				systolic2:
					for (int i = 0; i < TILE_SIZE; ++i) {
#pragma HLS UNROLL
					systolic3:
						for (int j = 0; j < TILE_SIZE; ++j) {
#pragma HLS UNROLL
							float last_sum = (k_outer || k) ? localC[i][j] : 0;
							localC[i][j] = last_sum + localA[i][k] * localB[k][j];
						}
					}
				}
			}

		// Burst write from output matrices to global memory
		// Burst write from matrix C
		writeC:
			for (int r_c = 0; r_c < current_tile_A_rows; ++r_c) {
				for (int c_c = 0; c_c < current_tile_B_cols; ++c_c) {
					c[(i_outer + r_c) * B_COLS + (j_outer + c_c)] = localC[r_c][c_c];
				}
			}
		}
	}
}
