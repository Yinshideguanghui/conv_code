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
    // Ӳ�о�ά�ر�����
    // �Ƚ�ʱ������������ȣ���ѡ��Ž�С��һ��
    //*�Ȱѽ�����������һ���Ϊʮ���ƣ�����ʹ�ù��߱Ƚ�
    assert((codeword.size() % 2) == 0);
    int file_length = codeword.size() / 2;
    decoded.resize(file_length);
    int *re_codeword_10 = new int[file_length];
    for (int i = 0; i < file_length; i++)
    {
        re_codeword_10[i] = binary_to_decimal(codeword[i * 2], codeword[i * 2 + 1]);
    }
    //*��ʼ����¼�� state_num = 4
    int **min_path_to_state = new int *[4]; // ����ĳ״̬�����·�����ȼ�¼
    for (int i = 0; i < 4; i++)
    {
        min_path_to_state[i] = new int[2]; // ��һ�б�����һ�������ڶ��б��浱ǰ�������
        for (int j = 0; j < 2; j++)
            min_path_to_state[i][j] = 0; // ��ʼ��
    }
    int **path_reserved = new int *[4]; // ����ĳ״̬�ı���·����
    for (int i = 0; i < 4; i++)
    {
        path_reserved[i] = new int[file_length];
        for (int j = 0; j < file_length; j++)
            path_reserved[i][j] = 0; // ��ʼ��
    }
    //*��ʱ��������
    int first_path = 0, second_path = 0; // ���ﵱǰ���ǵ�״̬������·���ĳ���
    //*ǰ�������в��ֽڵ㲻�ɵ������Ƚϣ��ʵ�������
    //**��1�㣨�±�=����-1����ͬ��
    min_path_to_state[0][0] = num_different_bit(re_codeword_10[0], state_table[0][0][1]);
    min_path_to_state[2][0] = num_different_bit(re_codeword_10[0], state_table[0][2][1]);
    path_reserved[0][0] = 1;
    path_reserved[2][0] = 5;
    //**��2��
    min_path_to_state[0][1] = min_path_to_state[0][0] + num_different_bit(re_codeword_10[1], state_table[0][0][1]);
    min_path_to_state[1][1] = min_path_to_state[2][0] + num_different_bit(re_codeword_10[1], state_table[2][1][1]);
    min_path_to_state[2][1] = min_path_to_state[0][0] + num_different_bit(re_codeword_10[1], state_table[0][2][1]);
    min_path_to_state[3][1] = min_path_to_state[2][0] + num_different_bit(re_codeword_10[1], state_table[2][3][1]);
    path_reserved[0][1] = 1;
    path_reserved[1][1] = 3;
    path_reserved[2][1] = 5;
    path_reserved[3][1] = 7;
    //***�������·�����ȴ洢
    min_path_to_state[0][0] = min_path_to_state[0][1];
    min_path_to_state[1][0] = min_path_to_state[1][1];
    min_path_to_state[2][0] = min_path_to_state[2][1];
    min_path_to_state[3][0] = min_path_to_state[3][1];
    //*ֱ����(k-2)�㣬����ȫ·���Ƚ�
    //*Ҳ�ɽ��ĸ��ڵ��ۺ�Ϊһ�����룬�ñ���ʽ��ʾ�ڵ㣬����д�����������ٶȻ���Ӧ�½�һЩ
    for (int i = 2; i < file_length - 2; i++)
    {
        //***��00(0)��00(0)��01(1)(1�ź�2��·��)
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
        //***��01(1)��10(2)��11(3)(3�ź�4��·��)
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
        //***��10(2)��00(0)��01(1)(5�ź�6��·��)
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
        //***��11(3)��10(2)��11(3)(7�ź�8��·��)
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
        //***�������·�����ȴ洢
        min_path_to_state[0][0] = min_path_to_state[0][1];
        min_path_to_state[1][0] = min_path_to_state[1][1];
        min_path_to_state[2][0] = min_path_to_state[2][1];
        min_path_to_state[3][0] = min_path_to_state[3][1];
    }
    //*����������ʱ�̶�����00���в��ֽڵ㲻�ɵ���ʵ�������
    //**��(k-1)�㣺����̶�Ϊ0��ֻ�ܵ���00(0)��01(1)
    //***��00(0)��00(0)��01(1)(1�ź�2��·��)
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
    //***��01(1)��10(2)��11(3)(3�ź�4��·��)
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
    //***�������·�����ȴ洢
    min_path_to_state[0][0] = min_path_to_state[0][1];
    min_path_to_state[1][0] = min_path_to_state[1][1];
    //**��k�㣺����̶�Ϊ0��ֻ�ܵ�00(0)
    //***��00(0)��00(0)��01(1)(1�ź�2��·��)
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
    //*����·�������������
    int *temp_state = NULL;
    int current_path_index = path_reserved[0][file_length - 1];
    for (int i = file_length - 1; i > 0; i--)
    {
        temp_state = path_node(current_path_index);                // �ҵ�·����Ӧʼĩ״̬
        decoded[i] = state_table[temp_state[0]][temp_state[1]][0]; // �ҵ���Ӧ�������
        current_path_index = path_reserved[temp_state[0]][i - 1];
    }
    //**���һ��û����һ�㣬�ʵ�������
    temp_state = path_node(current_path_index);                // �ҵ�·����Ӧʼĩ״̬
    decoded[0] = state_table[temp_state[0]][temp_state[1]][0]; // �ҵ���Ӧ�������
    //*�ͷ��ڴ�
    delete[] re_codeword_10, min_path_to_state, path_reserved, temp_state;
}

int ConvolutionalCode_7_5::binary_to_decimal(int a, int b)
{
    // ���ߣ���λ����������Ϊһ��ʮ��������a�Ǹ�λ
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
    // ���ߣ��Ƚ�����ʮ���Ʊ�ʾ�Ķ��������Ĵ��ڲ����λ��
    // ��ʵ��Ϊֱ�Ӳ������������2λ����������ʮ����0��3���ıȽ�
    switch (a ^ b) // ��λ�������
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
    // ����ͨ·��ţ�������ʼ�͵���״̬��������(7,5)_8��״̬ͼ��
    int *result = new int[2];
    result[1] = (path_index - 1) / 2; // ����״̬
    int a = path_index % 2;           // ���ɣ��˴������£��ɷ���ż����
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