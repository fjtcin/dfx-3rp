#include "stream_utils.h"
#include <fstream>      // For std::ifstream, std::ofstream
#include <sstream>      // For std::istringstream (potentially useful, but not strictly needed here)
#include <iomanip>      // For std::fixed, std::setprecision
#include <iostream>     // For error reporting (e.g., std::cerr)
#include <limits>       // For std::numeric_limits

// Implementation of readStream
int readStream(const std::string& filename, float* buffer, int& buffer_size) {
    // Check if the buffer pointer is valid and buffer_size is non-negative
    if (buffer == nullptr) {
        std::cerr << "Error: Input buffer is null." << std::endl;
        return -2; // Indicate a critical error with input parameters
    }
    if (buffer_size <= 0) {
        std::cerr << "Error: Initial buffer_size must be positive." << std::endl;
        // If buffer_size is 0, no elements can be read, so it's not necessarily an error
        // but rather a no-op. Let's treat it as such and return 0 elements read.
        // However, if the intent is to fill a buffer, a non-positive size is problematic.
        // For this implementation, we'll assume a positive initial buffer_size is expected
        // if any reading is to occur.
        buffer_size = 0; // No elements read
        return 0; // Or return an error if non-positive initial size is strictly invalid.
                  // For now, let's say 0 elements read.
    }

    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file for reading: " << filename << std::endl;
        buffer_size = 0; // No elements read
        return -1; // Error code for file open failure
    }

    int elements_read = 0;
    float temp_val;

    // Read numbers from the file until EOF, error, or buffer is full
    while (elements_read < buffer_size && (inputFile >> temp_val)) {
        buffer[elements_read] = temp_val;
        elements_read++;
    }

    // Check for stream errors (e.g., bad format if not EOF)
    // If we stopped reading before filling the buffer and it's not EOF,
    // it might be due to non-numeric data or other stream errors.
    if (inputFile.bad()) {
        std::cerr << "Error: A fatal I/O error occurred while reading file: " << filename << std::endl;
        buffer_size = elements_read; // Update with elements successfully read before error
        inputFile.close();
        return -2; // Error code for read error
    }
    // If inputFile.fail() is true but not inputFile.eof(), it means a formatting error occurred.
    // inputFile >> temp_val would have failed.
    // We don't need to explicitly check fail() here if we only care about what was successfully read.

    inputFile.close();

    if (elements_read == 0 && buffer_size > 0) {
        // This case means the file might be empty or contain no parsable numbers,
        // and the initial buffer_size was > 0.
        // Depending on requirements, this could be an error or just a valid state.
        // For now, we'll report 0 elements read.
        std::cerr << "Warning: No numbers read from file or file is empty: " << filename << std::endl;
    }

    buffer_size = elements_read; // Update buffer_size to actual count
    return 0; // Success
}

// Implementation of writeStream
int writeStream(const std::string& filename, const float* buffer, const int buffer_size) {
    if (buffer == nullptr && buffer_size > 0) {
        std::cerr << "Error: Output buffer is null but buffer_size is positive." << std::endl;
        return -2; // Indicate a critical error with input parameters
    }

    std::ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return -1; // Error code for file open failure
    }

    // Set precision for floating point output
    // std::fixed ensures that setprecision controls digits *after* the decimal point.
    // std::defaultfloat (or not using std::fixed) makes setprecision control total significant digits.
    outputFile << std::fixed << std::setprecision(2);

    for (int i = 0; i < buffer_size; ++i) {
        outputFile << buffer[i];
        if (i < buffer_size - 1) {
            outputFile << " "; // Add a space separator between numbers
        }
    }
    outputFile << std::endl; // Add a newline at the end of the file (optional, but good practice)

    if (outputFile.fail()) { // Check for errors after writing
        std::cerr << "Error: Failed to write all data to file: " << filename << std::endl;
        outputFile.close();
        return -2; // Error code for write error
    }

    outputFile.close();
    return 0; // Success
}

/*
// Example Usage (optional, can be in a main.cpp)
// To compile this example along with the utils:
// g++ stream_utils.cpp main.cpp -o main_executable -std=c++11
//
// main.cpp:
#include "stream_utils.h"
#include <iostream>
#include <vector>

int main() {
    // --- Test writeStream ---
    const int write_count = 5;
    float data_to_write[write_count] = {1.234f, 5.0f, 6.789f, 100.f, 0.12f};
    std::string out_filename = "test_output.txt";

    std::cout << "Writing data to " << out_filename << std::endl;
    if (writeStream(out_filename, data_to_write, write_count) == 0) {
        std::cout << "Successfully wrote data." << std::endl;
    } else {
        std::cout << "Failed to write data." << std::endl;
        return 1;
    }

    // --- Test readStream ---
    const int max_read_capacity = 10; // Max elements our buffer can hold
    float read_buffer[max_read_capacity];
    int elements_in_buffer = max_read_capacity; // Pass capacity
    std::string in_filename = "test_output.txt"; // Read the file we just wrote

    std::cout << "\nReading data from " << in_filename << std::endl;
    int read_status = readStream(in_filename, read_buffer, elements_in_buffer);

    if (read_status == 0) {
        std::cout << "Successfully read " << elements_in_buffer << " elements:" << std::endl;
        for (int i = 0; i < elements_in_buffer; ++i) {
            std::cout << read_buffer[i] << " ";
        }
        std::cout << std::endl;
    } else if (read_status == -1) {
        std::cout << "Failed to open file for reading." << std::endl;
        return 1;
    } else {
        std::cout << "Error during reading or no data read. Elements read: " << elements_in_buffer << std::endl;
        // Potentially print what was read if elements_in_buffer > 0
        if (elements_in_buffer > 0) {
             std::cout << "Partially read data: ";
             for (int i = 0; i < elements_in_buffer; ++i) std::cout << read_buffer[i] << " ";
             std::cout << std::endl;
        }
        return 1;
    }

    // Test reading from a file that doesn't exist
    std::cout << "\nAttempting to read from non_existent_file.txt" << std::endl;
    int non_existent_buffer_size = 5;
    float non_existent_buffer[non_existent_buffer_size];
    readStream("non_existent_file.txt", non_existent_buffer, non_existent_buffer_size);


    // Test reading from an empty file
    std::ofstream("empty_file.txt").close(); // Create an empty file
    std::cout << "\nAttempting to read from empty_file.txt" << std::endl;
    int empty_buffer_size = 5;
    float empty_buffer[empty_buffer_size];
    readStream("empty_file.txt", empty_buffer, empty_buffer_size);
    std::cout << "Elements read from empty file: " << empty_buffer_size << std::endl;


    return 0;
}
*/
