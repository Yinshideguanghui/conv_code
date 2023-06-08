#include <iostream>
#include <cmath>

#include "include/binary_file.h"
#include "include/conv_code.h"

int main(int argc, char** argv)
{
    // 读取参数
    int num_files = 100;
    std::size_t file_length = 800UL; // bits
    int codeword_length = file_length * 2; // bits
    int default_max_num_block_bytes = 64;

    if (argc == 1)
        ;
    else if (argc == 2)
    {
        num_files = std::stoi(argv[1]);
    }
    else if (argc == 3)
    {
        num_files = std::stoi(argv[1]);
        file_length = std::stoul(argv[2]);
        codeword_length = file_length * 2;
    }
    else if (argc == 4)
    {
        num_files = std::stoi(argv[1]);
        file_length = std::stoul(argv[2]);
        codeword_length = file_length * 2;
        default_max_num_block_bytes = std::stoi(argv[3]);
    }
    else
        return -1;

    // 随机数发生器
    std::random_device rd;
    std::mt19937 reng(rd());

    // 生成原始文件，并且卷积码编码产生codeword
    std::vector<ConvCode75::BinaryFile> files;
    files.reserve(num_files);
    for (std::size_t fidx = 0; fidx < num_files; ++fidx)
        files.emplace_back(ConvCode75::BinaryFile(file_length));

    // 实验：集中损坏 burst error
    //      把codeword按x字节一块划分，随机损坏其中1块，尝试恢复文件
    //      逐步增大x，探索(7,5)_8卷积码能稳定恢复的分块大小极限
    int max_num_block_bytes = std::min(codeword_length / 8, default_max_num_block_bytes);
    for (int num_block_bytes = 1; num_block_bytes <= max_num_block_bytes; ++num_block_bytes)
    {
        int block_length = num_block_bytes * 8;
        int num_blocks = codeword_length / block_length;
        std::uniform_int_distribution<> dis(0, num_blocks - 1);     // 错误位置的随机数分布

        std::vector<std::vector<int>> broken_codewords(num_files);  // 损坏的编码
        std::vector<std::vector<int>> recoverd_files(num_files);    // 从损坏的编码中恢复出的文件

        // 遍历文件，统计错误数目
        int num_bit_error = 0;
        int num_file_error = 0;
        for (std::size_t fidx = 0; fidx < num_files; ++fidx)
        {
            // 产生损坏位置
            int block_broken = dis(reng);
            int first_bit_broken = block_broken * block_length;
            int last_bit_broken = first_bit_broken + block_length - 1;
            // 发生损坏
            broken_codewords[fidx].resize(codeword_length);
            for (std::size_t b = 0; b < codeword_length; ++b)
                broken_codewords[fidx][b] = (b >= first_bit_broken && b <= last_bit_broken) ? (!files[fidx].codeword[b]) : files[fidx].codeword[b];
            // 译码损坏codeword，即尝试恢复文件
            ConvCode75::ConvolutionalCode_7_5::decode(broken_codewords[fidx], recoverd_files[fidx]);
            // 统计误码数
            int cur_file_num_bit_error = 0;
            for (std::size_t b = 0; b < file_length; ++b)
                if (recoverd_files[fidx][b] != files[fidx].bits[b])
                    ++cur_file_num_bit_error;
            num_bit_error += cur_file_num_bit_error;
            // 统计无法恢复的文件数
            if (cur_file_num_bit_error)
                ++num_file_error;
        }

        // 显示结果
        std::cout << "当前集中损坏块大小 " << block_length << "bits/" << codeword_length
                << "bits, 恢复后总误码率 " << static_cast<double>(num_bit_error) / (file_length * num_files)
                << ", 无法恢复的文件数 " << num_file_error << "/" << num_files
                << std::endl;
    }

    return 0;
}
