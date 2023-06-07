# Convolutional Code

《大数据存储技术》期末项目。

## 简介
该项目实现一个快速的(7,5)_8卷积码算法，并且具有两项主要功能：
1. 可以将一个二进制序列编码，并使用Hard-Viterbi算法译码。
2. 可以二进制读写文件，并且模拟其出现分散损坏或集中损坏时，卷积码的译码纠错效果。

## 已测试环境
```
Ubuntu 20.04 LTS
GNU Make 4.2.1
cmake version 3.25.0
```
无依赖第三方软件包。

## 仓库结构







## 运行
### 1.下载与编译
```
#! /bin/bash
cd /your/workspace/
git clone
mkdir build && cd build
cmake ..
make -j4
```

### 2.执行
1. 分散损坏实验，可附加参数：1随机生成的文件数、2每文件比特数、3指定最大损坏比特数
```
cd conv_code/exec/
./spread_error
```
2. 集中损坏实验，可附加参数：1随机生成的文件数、2每文件比特数、3指定最大文件分块块字节数
```
cd conv_code/exec/
./burst_error
```
3. 单文件分散损坏与读写实验，可附加参数：1文件路径（相对或绝对）、2指定损坏比特数
```
cd conv_code/exec/
./storage_file
```

## I/O示例
