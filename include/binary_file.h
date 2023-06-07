#ifndef BINARY_FILE_H
#define BINARY_FILE_H

#include <fstream>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <random>
#include <cassert>

#include "include/conv_code.h"

namespace ConvCode75
{

class BinaryFile
{
public:
    BinaryFile(std::size_t file_length);
    BinaryFile(std::filesystem::path file_path);

    void read(std::filesystem::path file_path);
    void write(std::filesystem::path file_path);

    void binaryDisplay() const;
    void hexDisplay() const;
    void binaryCodewordDisplay() const;
    void hexCodewordDisplay() const;

protected:
    void randomInitialize(std::size_t file_length);
    void convertBitsToBytes(const std::vector<int> &bits, std::vector<int> &bytes);
    void convertBytesToBits(const std::vector<int> &bytes, std::vector<int> &bits);

public:
    std::vector<int> bits;
    std::vector<int> bytes;
    std::vector<int> codeword;
    std::vector<int> codeword_bytes;

}; // END class BinaryFile

} // END namespace ConvCode75

#endif // #ifndef BINARY_FILE_H
