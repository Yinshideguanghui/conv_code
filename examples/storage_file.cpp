#include "include/conv_code.h"
#include "include/binary_file.h"

int main(int argc, char** argv)
{
    ConvCode75::BinaryFile file1(800);
    file1.binaryDisplay();
    file1.hexDisplay();
    file1.binaryCodewordDisplay();
    file1.hexCodewordDisplay();

    return 0;
}