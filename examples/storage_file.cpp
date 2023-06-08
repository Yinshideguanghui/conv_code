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
    bool display = false;
    if (argc == 1)
    {
        file_path = std::filesystem::path(argv[0]).parent_path() / "../data/example1.png";
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
    else if (argc == 5)
    {
        file_path = argv[1];
        if (file_path.is_relative())
            file_path = std::filesystem::path(argv[0]).parent_path() / file_path;

        broken_type = std::stoi(argv[2]) ? broken_t::BURST : broken_t::SPREAD;
        num_broken_bits = std::stoi(argv[3]);

        display = std::stoi(argv[4]);
    }
    else
        return -1;

    result_path = file_path.parent_path() / ("recovered_" + file_path.filename().string());

    // 随机数发生器
    std::random_device rd;
    std::mt19937 reng(rd());

    // 读取文件并显示
    ConvCode75::BinaryFile file1(file_path);
    if (display)
    {
        file1.hexDisplay();
        file1.hexCodewordDisplay();
    }


    // 产生损坏并显示
    ConvCode75::BinaryFile file2;
    int codeword_length = file1.codeword.size();
    num_broken_bits = std::min(codeword_length, num_broken_bits);
    if (broken_type == broken_t::SPREAD)
    {
        std::uniform_int_distribution<> dis(0, codeword_length - 1); // 错误位置的随机数分布
        // 产生损坏位置
        std::vector<int> broken_flag(codeword_length, 0);
        for (int ibroken = 0; ibroken < num_broken_bits; ++ibroken)
        {
            int pos_broken = dis(reng);
            while (broken_flag[pos_broken])
                pos_broken = dis(reng);
            broken_flag[pos_broken] = 1;
        }
        // 发生损坏
        file2.codeword.resize(codeword_length);
        for (std::size_t b = 0; b < codeword_length; ++b)
            file2.codeword[b] = broken_flag[b] ? (!file1.codeword[b]) : file1.codeword[b];
    }
    else if (broken_type == broken_t::BURST)
    {
        int block_length = num_broken_bits;
        assert(block_length % 8 == 0);
        int num_blocks = codeword_length / block_length;
        std::uniform_int_distribution<> dis(0, num_blocks - 2); // 错误位置的随机数分布，最后一块可能不够block_length，故排除
        // 产生损坏位置
        int block_broken = dis(reng);
        int first_bit_broken = block_broken * block_length;
        int last_bit_broken = first_bit_broken + block_length - 1;
        // 发生损坏
        file2.codeword.resize(codeword_length);
        for (std::size_t b = 0; b < codeword_length; ++b)
            file2.codeword[b] = (b >= first_bit_broken && b <= last_bit_broken) ? (!file1.codeword[b]) : file1.codeword[b];
    }
    else
        return -1;
    file2.convertBitsToBytes(file2.codeword, file2.codeword_bytes);
    if (display)
    {
        file2.hexCodewordDisplay();
    }

    // 尝试译码恢复
    ConvCode75::ConvolutionalCode_7_5::decode(file2.codeword, file2.bits);

    // 显示译码恢复结果
    file2.convertBitsToBytes(file2.bits, file2.bytes);
    if (display)
    {
        file2.hexDisplay();
    }

    // 写入恢复后文件
    file2.write(result_path);

    return 0;
}