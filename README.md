# FROG 编程语言

[![Logo](https://i.loli.net/2021/04/22/TEozG6QHwLqZiP1.png)](https://github.com/AloneCafe/frog/)

<span style="color:blue;">项目吉祥物：开心快乐的青蛙国王 FROG 桑，由 GIMP 手工绘制（渣绘轻喷）</span>

![GitHub top language](https://img.shields.io/github/languages/top/AloneCafe/frog?color=%2300ffff)
![GitHub lines](https://img.shields.io/tokei/lines/github/AloneCafe/frog?color=%233388bb)
![GitHub license](https://img.shields.io/github/license/AloneCafe/frog?color=%23ddcc88)
![GitHub repo size](https://img.shields.io/github/repo-size/AloneCafe/frog)

![GitHub branch checks state](https://img.shields.io/github/checks-status/AloneCafe/frog/master?color=%23bb1c00)
![GitHub last commit](https://img.shields.io/github/last-commit/AloneCafe/frog?color=%2322dd66)
![GitHub commit activity](https://img.shields.io/github/commit-activity/y/AloneCafe/frog?color=%23cd00dd)

## 关于作者
第一次自创一种编译型的编程语言，围绕着它所实现的词法分析器、语法分析器、类型检查 & 语义分析器、代码生成器 ... 这个编译系统的任何环节均是原创，并且采用 C++ 语言手工编写，没有借助任何分析器生成工具。只是作者本人想试着从零开始编写一个编译系统，满足自己从最开始学习编程技术开始直到现在总是想独立完成的一个小小的心愿，幸运的是如今终于能够窥见其冰山一角。虽然代码过于冗杂，已经无法控制复杂度，但是总算是蹩脚地正常运行起来了，即便在程序的各个角落都可能潜伏着一些臭虫。



## 关于 FROG
FROG 是一种简单的、结构化的、面向过程的、静态类型 & 强类型的编程语言。

本项目亦包含三个主要的组成部分，分别是
* 编译器 (Compiler)
* 汇编器 (Assembler)
* 虚拟机 (Virtual Machine)

如果在无错的情况下，编写 & 运行 FROG 程序的生命周期大致是这样的：
1. 先将写好源程序交由编译器编译处理，编译器将生成一种中间语言 (Intermediate Language)，形式上类似一种汇编语言
2. 再将上一步生成的汇编代码交由 FROG 汇编器进行汇编处理，最终将生成二进制字节码，即 FROG 字节码文件
3. 最后将上一步生成的字节码文件交由 FROG 虚拟机执行，程序进入执行状态，最终程序退出结束执行

FROG 编程语言采用类 C (C-like) 的语法，与 C、C++、C#、Java 等语言的基本语法类似，只是稍有不同之处。
本语言源程序结构简单、语法清晰明了，易于上手使用。
* ~~FROG 语法简明手册~~
* ~~FROG 汇编指令手册~~
