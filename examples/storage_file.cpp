#include "include/conv_code.h"
#include "include/binary_file.h"

enum class broken_t
{
    SPREAD = 0,
    BURST = 1
};

int main(int argc, char** argv)
{
    // 读取参数
    std::filesystem::path file_path;
    std::filesystem::path result_path;
    broken_t broken_type = broken_t::SPREAD;
    int num_broken_bits = 8;
    if (argc == 1)
    {
        file_path = std::filesystem::path(argv[0]).parent_path() / "../data/example1.jpg";
    }
    else if (argc == 2)
    {
        file_path = argv[1];
        if (file_path.is_relative())
            file_path = std::filesystem::path(argv[0]).parent_path() / file_path;
    }
    else if (argc == 3)
    {
        file_path = argv[1];
        if (file_path.is_relative())
            file_path = std::filesystem::path(argv[0]).parent_path() / file_path;

        broken_type = std::stoi(argv[3]) ? broken_t::BURST : broken_t::SPREAD;
    }
    else if (argc == 4)
    {
        file_path = argv[1];
        if (file_path.is_relative())
            file_path = std::filesystem::path(argv[0]).parent_path() / file_path;

        broken_type = std::stoi(argv[2]) ? broken_t::BURST : broken_t::SPREAD;
        num_broken_bits = std::stoi(argv[3]);
    }
    else
        return -1;

    result_path = file_path.parent_path() / "recovered_" / file_path.filename();

    // 读取文件并显示
    ConvCode75::BinaryFile file1(file_path);
    file1.hexDisplay();
    file1.hexCodewordDisplay();

    // 产生损坏并显示
    ConvCode75::BinaryFile file2;
    if (broken_type == broken_t::SPREAD)
    {

    }
    else if (broken_type == broken_t::BURST)
    {

    }
    else
        return -1;
    file2.convertBitsToBytes(file2.codeword, file2.codeword_bytes);
    file2.hexCodewordDisplay();

    // 尝试译码恢复
    ConvCode75::ConvolutionalCode_7_5::decode(file2.codeword, file2.bits);

    // 显示译码恢复结果
    file2.convertBitsToBytes(file2.bits, file2.bytes);
    file2.hexDisplay();

    // 写入恢复后文件
    file2.write(result_path);

    return 0;
}