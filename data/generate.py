import numpy as np
from scipy.sparse import random as sparse_random

# --- Configuration ---
ROWS = 128
COLS = 128
DENSITY = 0.01
VALUE_MULTIPLIER = 100.0 # To get values in [0, 100]

np.random.seed(0)

# --- Helper function to generate random data for sparse matrix ---
def generate_sparse_matrix_data(size):
    """
    Generates 'size' random float values, scaled to be between 0.00 and 100.00.
    Args:
        size (int): The number of data values to generate.
    Returns:
        np.ndarray: Array of float values rounded to two decimal places.
    """
    raw_values = np.random.rand(size) * VALUE_MULTIPLIER
    rounded_values = np.round(raw_values, 2)
    return rounded_values

# --- Helper functions for saving matrices ---

def save_dense_format(matrix_to_save, filename):
    """
    Saves a sparse matrix in dense format to a file.
    First line: rows cols
    Subsequent lines: matrix elements, space-separated, formatted to xx.xx.
    Args:
        matrix_to_save (scipy.sparse matrix): The sparse matrix to save.
        filename (str): The name of the file to save to.
    """
    print(f"Saving {filename} (Dense format)...")
    dense_array = matrix_to_save.toarray()
    rows, cols = dense_array.shape
    with open(filename, 'w') as f:
        f.write(f"{rows:.2f} {cols:.2f}\n")
        for i in range(rows):
            # Format each number to two decimal places
            f.write(" ".join([f"{val:.2f}" for val in dense_array[i]]) + "\n")
    print(f"{filename} saved.")

def save_coo_format(matrix_to_save, filename):
    """
    Saves a sparse matrix in COO (Coordinate) format to a file.
    First line: rows cols nnz
    Subsequent lines: row_idx col_idx value (space-separated, value formatted to xx.xx)
    Args:
        matrix_to_save (scipy.sparse matrix): The sparse matrix to save.
        filename (str): The name of the file to save to.
    """
    print(f"Saving {filename} (COO format)...")
    coo_m = matrix_to_save.tocoo()
    rows, cols = coo_m.shape
    nnz = coo_m.nnz
    with open(filename, 'w') as f:
        f.write(f"{rows:.2f} {cols:.2f} {nnz:.2f}\n")
        for r, c, val in zip(coo_m.row, coo_m.col, coo_m.data):
            f.write(f"{r:.2f} {c:.2f} {val:.2f}\n")
    print(f"{filename} saved.")

def save_csr_format(matrix_to_save, filename):
    """
    Saves a sparse matrix in CSR (Compressed Sparse Row) format to a file.
    First line: rows cols nnz
    Second line: values (space-separated, formatted to xx.xx)
    Third line: indices (column indices, space-separated)
    Fourth line: indptr (row pointers, space-separated)
    Args:
        matrix_to_save (scipy.sparse matrix): The sparse matrix to save.
        filename (str): The name of the file to save to.
    """
    print(f"Saving {filename} (CSR format)...")
    csr_m = matrix_to_save.tocsr()
    rows, cols = csr_m.shape
    nnz = csr_m.nnz
    with open(filename, 'w') as f:
        f.write(f"{rows:.2f} {cols:.2f} {nnz:.2f}\n")
        f.write(" ".join([f"{val:.2f}" for val in csr_m.data]) + "\n")
        f.write(" ".join([f"{ind:.2f}" for ind in csr_m.indices]) + "\n")
        f.write(" ".join([f"{ptr:.2f}" for ptr in csr_m.indptr]) + "\n")
    print(f"{filename} saved.")

def save_csc_format(matrix_to_save, filename):
    """
    Saves a sparse matrix in CSC (Compressed Sparse Column) format to a file.
    First line: rows cols nnz
    Second line: values (space-separated, formatted to xx.xx)
    Third line: indices (row indices, space-separated)
    Fourth line: indptr (column pointers, space-separated)
    Args:
        matrix_to_save (scipy.sparse matrix): The sparse matrix to save.
        filename (str): The name of the file to save to.
    """
    print(f"Saving {filename} (CSC format)...")
    csc_m = matrix_to_save.tocsc()
    rows, cols = csc_m.shape
    nnz = csc_m.nnz
    with open(filename, 'w') as f:
        f.write(f"{rows:.2f} {cols:.2f} {nnz:.2f}\n")
        f.write(" ".join([f"{val:.2f}" for val in csc_m.data]) + "\n")
        f.write(" ".join([f"{ind:.2f}" for ind in csc_m.indices]) + "\n")
        f.write(" ".join([f"{ptr:.2f}" for ptr in csc_m.indptr]) + "\n")
    print(f"{filename} saved.")

# --- Main script execution ---
if __name__ == "__main__":
    print("Starting matrix generation and saving process...\n")

    # Generate sparse matrix A
    print("Generating matrix A...")
    # random_state ensures reproducibility for A.
    # data_rvs specifies our custom function for generating data values.
    A = sparse_random(
        ROWS, COLS,
        density=DENSITY,
        format='coo',  # Start with COO, can be converted later
        dtype=np.float64,
        random_state=42, # Seed for A
        data_rvs=generate_sparse_matrix_data
    )
    A.sum_duplicates()
    A.eliminate_zeros()
    print(f"Matrix A generated: {A.shape}, NNZ={A.nnz}, Density={A.nnz/(ROWS*COLS)*100:.4f}%")

    # Generate sparse matrix B
    print("\nGenerating matrix B...")
    # Using a different random_state for B to make it different from A.
    B = sparse_random(
        COLS, ROWS,
        density=DENSITY,
        format='coo',
        dtype=np.float64,
        random_state=123, # Seed for B
        data_rvs=generate_sparse_matrix_data
    )
    B.sum_duplicates()
    B.eliminate_zeros()
    print(f"Matrix B generated: {B.shape}, NNZ={B.nnz}, Density={B.nnz/(ROWS*COLS)*100:.4f}%")

    # Calculate C = A * B
    print("\nCalculating C = A * B...")
    C = A.dot(B) # Matrix multiplication for sparse matrices
    # The result C might be in CSR or CSC format by default, depending on SciPy.
    # Its density can be different from A or B.
    C.sum_duplicates()
    C.eliminate_zeros()
    print(f"Matrix C calculated: {C.shape}, NNZ={C.nnz}, Density={C.nnz/(ROWS*ROWS)*100:.4f}%")
    if C.nnz == 0:
        print("Warning: Matrix C has no non-zero elements. This can happen with sparse matrices.")
        # If C is all zeros, ensure it has the correct shape for saving.
        # .tocoo() etc on an all-zero matrix (if not explicitly shaped) might be tricky.
        # However, `A.dot(B)` should preserve shape.
        # If C has 0 nnz, its data array will be empty. The save functions should handle this.
        # For dense format, it will correctly print all zeros.
        # For sparse formats, nnz will be 0, data/indices arrays will be empty.
        # indptr will still be valid (e.g., [0,0,...,0] for CSR/CSC).

    # --- Save Matrix A ---
    print("\n--- Saving Matrix A ---")
    save_dense_format(A, "A.txt")
    save_coo_format(A, "A_COO.txt")
    save_csr_format(A, "A_CSR.txt")
    save_csc_format(A, "A_CSC.txt")

    # --- Save Matrix B ---
    print("\n--- Saving Matrix B ---")
    save_dense_format(B, "B.txt")
    save_coo_format(B, "B_COO.txt")
    save_csr_format(B, "B_CSR.txt")
    save_csc_format(B, "B_CSC.txt")

    # --- Save Matrix C ---
    print("\n--- Saving Matrix C ---")
    save_dense_format(C, "C.txt")
    save_coo_format(C, "C_COO.txt")
    save_csr_format(C, "C_CSR.txt")
    save_csc_format(C, "C_CSC.txt")

    print("\nAll matrices saved. Process complete.")
