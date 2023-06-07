#include "include/binary_file.h"

namespace ConvCode75
{

BinaryFile::BinaryFile(std::size_t file_length)
{
    assert(file_length % 8 == 0);
    randomInitialize(file_length);
    ConvolutionalCode_7_5::encode(bits, codeword);
    convertBitsToBytes(codeword, codeword_bytes);
}

BinaryFile::BinaryFile(std::filesystem::path file_path)
{
    read(file_path);
    ConvolutionalCode_7_5::encode(bits, codeword);
    convertBitsToBytes(codeword, codeword_bytes);
}

/**
 * @brief 读取文件（一般操作系统单位为字节），转换为二进制以便编码
 * 
 * @param file_path 文件路径
 */
void BinaryFile::read(std::filesystem::path file_path)
{
    std::ifstream std_ifstream;
    std_ifstream.open(file_path, std::ios_base::in | std::ios_base::binary);

    if (std_ifstream)
    {
        // 读取字节
        std_ifstream.seekg(0, std::ios::end);
        std::size_t file_bytes = static_cast<std::size_t>(std_ifstream.tellg());
        bytes.resize(file_bytes + 1);
        std_ifstream.seekg(0, std::ios::beg);
        for (std::size_t byte_idx = 0; byte_idx < file_bytes; ++byte_idx)
            std_ifstream.read((char*)(&(bytes[byte_idx])), sizeof(std::uint8_t));
        bytes[file_bytes] = 0; // 补充一个0以便使用卷积码
        std_ifstream.close();
        // 转换为二进制以便编码
        convertBytesToBits(bytes, bits);
    }
    else
    {
        std::cout << "无法打开文件" << file_path.string() << std::endl;
    }
}

/**
 * @brief 写入文件（单位为字节）
 * 
 * @param file_path 文件路径
 */
void BinaryFile::write(std::filesystem::path file_path)
{
    std::ofstream std_ofstream;
    std_ofstream.open(file_path, std::ios_base::out | std::ios_base::binary);

    if (std_ofstream)
    {
        std::size_t file_bytes = bytes.size() - 1; // 去掉末尾补的0
        for (std::size_t byte_idx = 0; byte_idx < file_bytes; ++byte_idx)
            std_ofstream.write((char*)(&(bytes[byte_idx])), sizeof(std::uint8_t));
        std_ofstream.close();
    }
    else
    {
        std::cout << "无法打开文件" << file_path.string() << std::endl;
    }
}

/**
 * @brief 二进制显示文件
 */
void BinaryFile::binaryDisplay() const
{
    bool aligned = bits.size() % 8 == 0;
    std::size_t rows = aligned ? (bits.size() / 8) : (bits.size() / 8 + 1);
    std::cout << "---------------------------------------\n";
    std::cout << "  \\col|  00  01  02  03  04  05  06  07\n"
              << "row\\  |\n"
              << "------|--------------------------------\n";
    for (std::size_t row_idx = 0; row_idx < (aligned ? rows : (rows - 1)); ++row_idx)
    {
        std::size_t start_idx = row_idx * 8;
        std::cout << std::hex << std::setw(6) << std::setfill('0') << row_idx << "|   ";
        std::cout << bits[start_idx];
        for (int col_idx = 1; col_idx < 8; ++col_idx)
            std::cout << "   " << bits[start_idx + col_idx];
        std::cout << "\n";
    }
    if (!aligned)
    {
        std::size_t row_idx = rows - 1;
        std::size_t start_idx = row_idx * 8;
        std::cout << std::hex << std::setw(6) << std::setfill('0') << row_idx << "|   ";
        std::cout << bits[start_idx];
        for (int col_idx = 1; col_idx < bits.size() - start_idx; ++col_idx)
            std::cout << "   " << bits[start_idx + col_idx];
        std::cout << "\n";
    }
    std::cout << "---------------------------------------" << std::endl;
}

/**
 * @brief 十六进制显示文件
 */
void BinaryFile::hexDisplay() const
{
    bool aligned = bytes.size() % 16 == 0;
    std::size_t rows = aligned ? (bytes.size() / 16) : (bytes.size() / 16 + 1);
    std::cout << "-----------------------------------------------------------------------\n";
    std::cout << "  \\col|  00  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f\n"
              << "row\\  |\n"
              << "------|----------------------------------------------------------------\n";
    for (std::size_t row_idx = 0; row_idx < (aligned ? rows : (rows - 1)); ++row_idx)
    {
        std::size_t start_idx = row_idx * 16;
        std::cout << std::hex << std::setw(4) << std::setfill('0') << row_idx << "  |  ";
        std::cout << std::hex << std::setw(2) << std::setfill('0') << bytes[start_idx];
        for (int col_idx = 1; col_idx < 16; ++col_idx)
            std::cout << "  "  << std::hex << std::setw(2) << std::setfill('0') << bytes[start_idx + col_idx];
        std::cout << "\n";
    }
    if (!aligned)
    {
        std::size_t row_idx = rows - 1;
        std::size_t start_idx = row_idx * 16;
        std::cout << std::hex << std::setw(4) << std::setfill('0') << row_idx << "  |  ";
        std::cout << std::hex << std::setw(2) << std::setfill('0') << bytes[start_idx];
        for (int col_idx = 1; col_idx < bytes.size() - start_idx; ++col_idx)
            std::cout << "  "  << std::hex << std::setw(2) << std::setfill('0') << bytes[start_idx + col_idx];
        std::cout << "\n";
    }
    std::cout << "-----------------------------------------------------------------------" << std::endl;
}

/**
 * @brief 二进制显示文件卷积码编码结果
 */
void BinaryFile::binaryCodewordDisplay() const
{
    bool aligned = codeword.size() % 8 == 0;
    std::size_t rows = aligned ? (codeword.size() / 8) : (codeword.size() / 8 + 1);
    std::cout << "---------------------------------------\n";
    std::cout << "  \\col|  00  01  02  03  04  05  06  07\n"
              << "row\\  |\n"
              << "------|--------------------------------\n";
    for (std::size_t row_idx = 0; row_idx < (aligned ? rows : (rows - 1)); ++row_idx)
    {
        std::size_t start_idx = row_idx * 8;
        std::cout << std::hex << std::setw(6) << std::setfill('0') << row_idx << "|   ";
        std::cout << codeword[start_idx];
        for (int col_idx = 1; col_idx < 8; ++col_idx)
            std::cout << "   " << codeword[start_idx + col_idx];
        std::cout << "\n";
    }
    if (!aligned)
    {
        std::size_t row_idx = rows - 1;
        std::size_t start_idx = row_idx * 8;
        std::cout << std::hex << std::setw(6) << std::setfill('0') << row_idx << "|   ";
        std::cout << codeword[start_idx];
        for (int col_idx = 1; col_idx < codeword.size() - start_idx; ++col_idx)
            std::cout << "   " << codeword[start_idx + col_idx];
        std::cout << "\n";
    }
    std::cout << "---------------------------------------" << std::endl;
}

/**
 * @brief 十六进制显示文件卷积码编码结果
 */
void BinaryFile::hexCodewordDisplay() const
{
    bool aligned = codeword_bytes.size() % 16 == 0;
    std::size_t rows = aligned ? (codeword_bytes.size() / 16) : (codeword_bytes.size() / 16 + 1);
    std::cout << "-----------------------------------------------------------------------\n";
    std::cout << "  \\col|  00  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f\n"
              << "row\\  |\n"
              << "------|----------------------------------------------------------------\n";
    for (std::size_t row_idx = 0; row_idx < (aligned ? rows : (rows - 1)); ++row_idx)
    {
        std::size_t start_idx = row_idx * 16;
        std::cout << std::hex << std::setw(4) << std::setfill('0') << row_idx << "  |  ";
        std::cout << std::hex << std::setw(2) << std::setfill('0') << codeword_bytes[start_idx];
        for (int col_idx = 1; col_idx < 16; ++col_idx)
            std::cout << "  "  << std::hex << std::setw(2) << std::setfill('0') << codeword_bytes[start_idx + col_idx];
        std::cout << "\n";
    }
    if (!aligned)
    {
        std::size_t row_idx = rows - 1;
        std::size_t start_idx = row_idx * 16;
        std::cout << std::hex << std::setw(4) << std::setfill('0') << row_idx << "  |  ";
        std::cout << std::hex << std::setw(2) << std::setfill('0') << codeword_bytes[start_idx];
        for (int col_idx = 1; col_idx < codeword_bytes.size() - start_idx; ++col_idx)
            std::cout << "  "  << std::hex << std::setw(2) << std::setfill('0') << codeword_bytes[start_idx + col_idx];
        std::cout << "\n";
    }
    std::cout << "-----------------------------------------------------------------------" << std::endl;
}

/**
 * @brief 随机二进制初始化，模拟创建文件
 * 
 * @param file_length 文件长度（有效长度为该，因为卷积码要求最后补相当于寄存器数目的0）
 */
void BinaryFile::randomInitialize(std::size_t file_length)
{
    // 随机数发生器
    std::random_device rd;
    std::mt19937 reng(rd());
    std::uniform_int_distribution<> dis01(0, 1);

    // 二进制随机
    int num_registers = 2;
    bits.resize(file_length);
    int no_padding_length = file_length - num_registers;
    for (std::size_t b = 0; b < no_padding_length; ++b)
        bits[b] = dis01(reng);
    for (std::size_t b = no_padding_length; b < file_length; ++b)
        bits[b] = 0;
    
    // 转换为字节
    convertBitsToBytes(bits, bytes);
}

/**
 * @brief 把比特串转换为字节串（假设bits数能被8整除，多余的会被舍弃）
 * 
 * @param bits 输入比特串
 * @param bytes 输出字节串
 */
void BinaryFile::convertBitsToBytes(const std::vector<int> &bits, std::vector<int> &bytes)
{
    std::size_t num_bytes = bits.size() >> 3;
    bytes.resize(num_bytes);
    for (std::size_t byte_idx = 0; byte_idx < num_bytes; ++byte_idx)
    {
        std::size_t start_bit_idx = byte_idx << 3;
        int byte = 0;
        for (int i = 0; i < 8; ++i)
            byte |= (bits[start_bit_idx + i] << (7 - i));
        bytes[byte_idx] = byte;
    }
}

/**
 * @brief 把字节串转换为比特串
 * 
 * @param bytes 输入字节串
 * @param bits 输出比特串
 */
void BinaryFile::convertBytesToBits(const std::vector<int> &bytes, std::vector<int> &bits)
{
    std::size_t nbytes = bytes.size();
    bits.resize(nbytes << 3);
    for (std::size_t byte_idx = 0; byte_idx < nbytes; ++byte_idx)
    {
        std::size_t start_bit_idx = byte_idx << 3;
        std::uint8_t byte = bytes[byte_idx];
        for (int i = 7; i >= 0; --i)
        {
            bool bit = (byte & (1 << i)) != 0;
            bits[start_bit_idx + (7 - i)] = static_cast<int>(bit);
        }
    }
}

} // END namespace ConvCode75
