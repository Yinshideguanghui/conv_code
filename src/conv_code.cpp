#include "include/conv_code.h"

namespace ConvCode75
{

void ConvolutionalCode_7_5::encode(const std::vector<int> &src, std::vector<int> &codeword)
{
    codeword.resize(src.size() * 2);
    // 初始时寄存器全零
    codeword[0] = src[0];
    codeword[1] = src[0];
    // 第二个信息比特，寄存器S1固定为零
    codeword[2] = (src[1] + src[0]) % 2;
    codeword[3] = src[1];
    // 之后，对于(7,5)_8码，某时刻c1=该时刻和前两个时刻message模2加，c2=该时刻message和上上个时刻模2加
    for (int i = 2; i < src.size(); i++)
    {
        codeword[2 * i] = (src[i] + src[i - 1] + src[i - 2]) % 2;
        codeword[2 * i + 1] = (src[i] + src[i - 2]) % 2;
    }
}

void ConvolutionalCode_7_5::decode(const std::vector<int> &codeword, std::vector<int> &decoded)
{
    // 硬判决维特比译码
    // 比较时若出现两条相等，则选编号较小的一条
    //*先把接收序列两个一组变为十进制，便于使用工具比较
    assert((codeword.size() % 2) == 0);
    int file_length = codeword.size() / 2;
    decoded.resize(file_length);
    int *re_codeword_10 = new int[file_length];
    for (int i = 0; i < file_length; i++)
    {
        re_codeword_10[i] = binary_to_decimal(codeword[i * 2], codeword[i * 2 + 1]);
    }
    //*初始化记录表 state_num = 4
    int **min_path_to_state = new int *[4]; // 到达某状态的最短路径长度记录
    for (int i = 0; i < 4; i++)
    {
        min_path_to_state[i] = new int[2]; // 第一列保存上一层结果，第二列保存当前层计算结果
        for (int j = 0; j < 2; j++)
            min_path_to_state[i][j] = 0; // 初始化
    }
    int **path_reserved = new int *[4]; // 到达某状态的保留路径表
    for (int i = 0; i < 4; i++)
    {
        path_reserved[i] = new int[file_length];
        for (int j = 0; j < file_length; j++)
            path_reserved[i][j] = 0; // 初始化
    }
    //*临时辅助变量
    int first_path = 0, second_path = 0; // 到达当前考虑的状态的两条路径的长度
                                            //*前两层中有部分节点不可到达，不需比较，故单独处理
                                            //**第1层（下标=层数-1，下同）
    min_path_to_state[0][0] = num_different_bit(re_codeword_10[0], state_table[0][0][1]);
    min_path_to_state[2][0] = num_different_bit(re_codeword_10[0], state_table[0][2][1]);
    path_reserved[0][0] = 1;
    path_reserved[2][0] = 5;
    //**第2层
    min_path_to_state[0][1] = min_path_to_state[0][0] + num_different_bit(re_codeword_10[1], state_table[0][0][1]);
    min_path_to_state[1][1] = min_path_to_state[2][0] + num_different_bit(re_codeword_10[1], state_table[2][1][1]);
    min_path_to_state[2][1] = min_path_to_state[0][0] + num_different_bit(re_codeword_10[1], state_table[0][2][1]);
    min_path_to_state[3][1] = min_path_to_state[2][0] + num_different_bit(re_codeword_10[1], state_table[2][3][1]);
    path_reserved[0][1] = 1;
    path_reserved[1][1] = 3;
    path_reserved[2][1] = 5;
    path_reserved[3][1] = 7;
    //***更新最短路径长度存储
    min_path_to_state[0][0] = min_path_to_state[0][1];
    min_path_to_state[1][0] = min_path_to_state[1][1];
    min_path_to_state[2][0] = min_path_to_state[2][1];
    min_path_to_state[3][0] = min_path_to_state[3][1];
    //*直到第(k-2)层，都是全路径比较
    //*也可将四个节点综合为一个代码，用表达式表示节点，即不写死，但运行速度会相应下降一些
    for (int i = 2; i < file_length - 2; i++)
    {
        //***到00(0)：00(0)和01(1)(1号和2号路径)
        first_path = min_path_to_state[0][0] + num_different_bit(re_codeword_10[i], state_table[0][0][1]);
        second_path = min_path_to_state[1][0] + num_different_bit(re_codeword_10[i], state_table[1][0][1]);
        if (first_path <= second_path)
        {
            min_path_to_state[0][1] = first_path;
            path_reserved[0][i] = 1;
        }
        else
        {
            min_path_to_state[0][1] = second_path;
            path_reserved[0][i] = 2;
        }
        //***到01(1)：10(2)和11(3)(3号和4号路径)
        first_path = min_path_to_state[2][0] + num_different_bit(re_codeword_10[i], state_table[2][1][1]);
        second_path = min_path_to_state[3][0] + num_different_bit(re_codeword_10[i], state_table[3][1][1]);
        if (first_path <= second_path)
        {
            min_path_to_state[1][1] = first_path;
            path_reserved[1][i] = 3;
        }
        else
        {
            min_path_to_state[1][1] = second_path;
            path_reserved[1][i] = 4;
        }
        //***到10(2)：00(0)和01(1)(5号和6号路径)
        first_path = min_path_to_state[0][0] + num_different_bit(re_codeword_10[i], state_table[0][2][1]);
        second_path = min_path_to_state[1][0] + num_different_bit(re_codeword_10[i], state_table[1][2][1]);
        if (first_path <= second_path)
        {
            min_path_to_state[2][1] = first_path;
            path_reserved[2][i] = 5;
        }
        else
        {
            min_path_to_state[2][1] = second_path;
            path_reserved[2][i] = 6;
        }
        //***到11(3)：10(2)和11(3)(7号和8号路径)
        first_path = min_path_to_state[2][0] + num_different_bit(re_codeword_10[i], state_table[2][3][1]);
        second_path = min_path_to_state[3][0] + num_different_bit(re_codeword_10[i], state_table[3][3][1]);
        if (first_path <= second_path)
        {
            min_path_to_state[3][1] = first_path;
            path_reserved[3][i] = 7;
        }
        else
        {
            min_path_to_state[3][1] = second_path;
            path_reserved[3][i] = 8;
        }
        //***更新最短路径长度存储
        min_path_to_state[0][0] = min_path_to_state[0][1];
        min_path_to_state[1][0] = min_path_to_state[1][1];
        min_path_to_state[2][0] = min_path_to_state[2][1];
        min_path_to_state[3][0] = min_path_to_state[3][1];
    }
    //*最后两层编码时固定输入00，有部分节点不可到达，故单独处理
    //**第(k-1)层：输入固定为0，只能到达00(0)和01(1)
    //***到00(0)：00(0)和01(1)(1号和2号路径)
    first_path = min_path_to_state[0][0] + num_different_bit(re_codeword_10[file_length - 2], state_table[0][0][1]);
    second_path = min_path_to_state[1][0] + num_different_bit(re_codeword_10[file_length - 2], state_table[1][0][1]);
    if (first_path <= second_path)
    {
        min_path_to_state[0][1] = first_path;
        path_reserved[0][file_length - 2] = 1;
    }
    else
    {
        min_path_to_state[0][1] = second_path;
        path_reserved[0][file_length - 2] = 2;
    }
    //***到01(1)：10(2)和11(3)(3号和4号路径)
    first_path = min_path_to_state[2][0] + num_different_bit(re_codeword_10[file_length - 2], state_table[2][1][1]);
    second_path = min_path_to_state[3][0] + num_different_bit(re_codeword_10[file_length - 2], state_table[3][1][1]);
    if (first_path <= second_path)
    {
        min_path_to_state[1][1] = first_path;
        path_reserved[1][file_length - 2] = 3;
    }
    else
    {
        min_path_to_state[1][1] = second_path;
        path_reserved[1][file_length - 2] = 4;
    }
    //***更新最短路径长度存储
    min_path_to_state[0][0] = min_path_to_state[0][1];
    min_path_to_state[1][0] = min_path_to_state[1][1];
    //**第k层：输入固定为0，只能到00(0)
    //***到00(0)：00(0)和01(1)(1号和2号路径)
    first_path = min_path_to_state[0][0] + num_different_bit(re_codeword_10[file_length - 1], state_table[0][0][1]);
    second_path = min_path_to_state[1][0] + num_different_bit(re_codeword_10[file_length - 1], state_table[1][0][1]);
    if (first_path <= second_path)
    {
        min_path_to_state[0][1] = first_path;
        path_reserved[0][file_length - 1] = 1;
    }
    else
    {
        min_path_to_state[0][1] = second_path;
        path_reserved[0][file_length - 1] = 2;
    }
    //*回溯路径，获得译码结果
    int *temp_state = NULL;
    int current_path_index = path_reserved[0][file_length - 1];
    for (int i = file_length - 1; i > 0; i--)
    {
        temp_state = path_node(current_path_index);                // 找到路径对应始末状态
        decoded[i] = state_table[temp_state[0]][temp_state[1]][0]; // 找到对应输入比特
        current_path_index = path_reserved[temp_state[0]][i - 1];
    }
    //**最后一层没有上一层，故单独处理
    temp_state = path_node(current_path_index);                // 找到路径对应始末状态
    decoded[0] = state_table[temp_state[0]][temp_state[1]][0]; // 找到对应输入比特
    //*释放内存
    delete[] re_codeword_10, min_path_to_state, path_reserved, temp_state;
}

int ConvolutionalCode_7_5::binary_to_decimal(int a, int b)
{
    // 工具：两位二进制数变为一个十进制数，a是高位
    switch (a)
    {
    case 0:
    {
        switch (b)
        {
        case 0: // 00
        {
            return 0;
        }
        break;
        case 1: // 01
        {
            return 1;
        }
        default:
            break;
        }
    }
    break;
    case 1:
    {
        switch (b)
        {
        case 0: // 10
        {
            return 2;
        }
        break;
        case 1: // 11
        {
            return 3;
        }
        default:
            break;
        }
    }
    break;
    default:
        return 0;
    }
    return 0;
}

int ConvolutionalCode_7_5::num_different_bit(int a, int b)
{
    // 工具：比较两个十进制表示的二进制数的存在差异的位数
    // 该实现为直接查表，仅适用于2位二进制数（十进制0到3）的比较
    switch (a ^ b) //按位异或运算
    {
    case 0: // 00
    {
        return 0;
    }
    break;
    case 1: // 01
    case 2: // 10
    {
        return 1;
    }
    break;
    case 3: // 11
    {
        return 2;
    }
    break;
    default:
        return 0;
    }
    return 0;
}

int *ConvolutionalCode_7_5::path_node(int path_index)
{
    // 输入通路编号，返回起始和到达状态（适用于(7,5)_8的状态图）
    int *result = new int[2];
    result[1] = (path_index - 1) / 2; // 到达状态
    int a = path_index % 2;           // 规律：此处定义下，可分奇偶处理
    switch (result[1])
    {
    case 0:
    case 2:
    {
        if (a == 0)
            result[0] = 1;
        else
            result[0] = 0;
    }
    break;
    case 1:
    case 3:
    {
        if (a == 0)
            result[0] = 3;
        else
            result[0] = 2;
    }
    break;
    default:
        break;
    }
    return result;
}

} // END namespace ConvCode75