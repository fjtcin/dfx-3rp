#pragma once

#include <string>

/**
 * @brief Reads a stream of floating-point numbers from a text file into a buffer.
 *
 * The function reads numbers separated by whitespace (spaces, tabs, newlines).
 * It will attempt to read up to the initial value of buffer_size elements.
 * Upon successful completion, buffer_size is updated to reflect the actual
 * number of elements read into the buffer.
 *
 * @param filename The path to the input text file.
 * @param buffer A pre-allocated float array where the numbers will be stored.
 * @param buffer_size As input, the maximum capacity of the buffer.
 * As output, the actual number of floats read from the file.
 * @return 0 on success, -1 if the file cannot be opened, -2 if a read error occurs
 * or if no numbers could be read.
 */
int readStream(const std::string& filename, float* buffer, int& buffer_size);

/**
 * @brief Writes a float array to a text file, with each number formatted to
 * at most two decimal places.
 *
 * Numbers will be space-separated in the output file.
 *
 * @param filename The path to the output text file.
 * @param buffer The float array containing the numbers to write.
 * @param buffer_size The number of elements in the buffer to write.
 * @return 0 on success, -1 if the file cannot be opened.
 */
int writeStream(const std::string& filename, const float* buffer, const int buffer_size);
