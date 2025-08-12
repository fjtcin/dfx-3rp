# Tiled Systolic Array

The implementation is based on the systolic array in [Xilinx Vitis Accel Examples](https://github.com/Xilinx/Vitis_Accel_Examples/blob/2023.1/cpp_kernels/systolic_array/src/mmult.cpp) ([Documentation](https://xilinx.github.io/Vitis_Accel_Examples/2023.1/html/systolic_array.html)), with tiling added to support larger matrices.

## Details

The algorithm partitions the input matrices A and B into small blocks (tiles), which are then fed into the systolic array for computation. Each Processing Element (PE) within the systolic array performs a multiply-accumulate operation. The tiling strategy helps manage on-chip memory resources (like BRAMs), enabling the module to process matrices that are much larger than the on-chip storage capacity. In the HLS implementation, it is necessary to meticulously design the control flows for data loading, the computational logic of the PE array, and the writing back of results.

For the PEs of the systolic array, applying the `PIPELINE` directive to the internal multiply-accumulate operation loop is standard practice; this is handled automatically by Vitis without needing manual setup. To achieve parallel data flow between PEs, the relevant loops are typically fully unrolled using the `UNROLL` directive. The on-chip memories (BRAMs) used to cache the input matrix tiles are partitioned using the `ARRAY_PARTITION` directive to provide sufficient parallel read/write bandwidth.

Regarding interfaces, the input matrix data (or its tiles) is typically received through an AXIS slave interface, which facilitates efficient data reception from a preceding module (e.g. a decompression module) or from the PS. The computation results (e.g. the dense matrix) are output through an AXIS master interface, allowing them to stream to a subsequent module (e.g. a compression module) or to the PS. For scenarios involving direct loading of tiles from or writing back to main memory, an AXIMM master interface is also configured. The selection of input and output interfaces is likewise handled by data selectors.
