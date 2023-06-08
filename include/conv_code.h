#ifndef CONV_CODE_H
#define CONV_CODE_H

#include <vector>
#include <cassert>

namespace ConvCode75
{

class ConvolutionalCode_7_5
{
public:
    static void encode(const std::vector<int> &src, std::vector<int> &codeword);
    static void decode(const std::vector<int> &codeword, std::vector<int> &decoded);

protected:
    static int binary_to_decimal(int a, int b);
    static int num_different_bit(int a, int b);
    static std::pair<int, int> path_node(int path_index);

protected:
    static inline int state_table[4][4][2] = {
        // (7,5)_8卷积码对应状态表，第一个下标=起始状态，第二个下标=到达状态，第0个值=输入，第1个值=输出
        // 状态格式：(S0,S1) = (下标1,下标2)，对应的十进制数
        // 输出格式：(c1,c2) 对应的十进制数
        // 例:state_table[0][2][0] = 1; state_table[0][2][1] = 3; 表示从状态00输入1到状态10，编码输出应为11
        // 输入和输出=-1 表示该通路不存在
        // 通路编号顺序：到达状态相同的两条为一组，到达状态较小的在前；同组内起始状态较小的在前
         0,  0, // state_table[0][0][0] =  0;  state_table[0][0][1] =  0;	// 00 通路编号1：00到00
        -1, -1,// state_table[0][1][0] = -1;  state_table[0][1][1] = -1;
         1,  3,// state_table[0][2][0] =  1;  state_table[0][2][1] =  3;	// 11 通路编号5：00到10
        -1, -1,// state_table[0][3][0] = -1;  state_table[0][3][1] = -1;

         0,  3,// state_table[1][0][0] =  0;  state_table[1][0][1] =  3;	// 11 通路编号2：01到00
        -1, -1,// state_table[1][1][0] = -1;  state_table[1][1][1] = -1;
         1,  0,// state_table[1][2][0] =  1;  state_table[1][2][1] =  0;	// 00 通路编号6：01到10
        -1, -1,// state_table[1][3][0] = -1;  state_table[1][3][1] = -1;

        -1, -1,// state_table[2][0][0] = -1;  state_table[2][0][1] = -1;
         0,  2,// state_table[2][1][0] =  0;  state_table[2][1][1] =  2;	// 10 通路编号3：10到01
        -1, -1,// state_table[2][2][0] = -1;  state_table[2][2][1] = -1;
         1,  1,// state_table[2][3][0] =  1;  state_table[2][3][1] =  1;	// 01 通路编号7：10到11

        -1, -1,// state_table[3][0][0] = -1;  state_table[3][0][1] = -1;
         0,  1,// state_table[3][1][0] =  0;  state_table[3][1][1] =  1;	// 01 通路编号4：11到01
        -1, -1,// state_table[3][2][0] = -1;  state_table[3][2][1] = -1;
         1,  2// state_table[3][3][0] =  1;  state_table[3][3][1] =  2;	// 10 通路编号8：11到11
    };

}; // END class ConvolutionalCode_7_5

} // END namespace ConvCode75

#endif // #ifndef CONV_CODE_H
