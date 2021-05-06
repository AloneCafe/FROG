# FROG 编程语言




## 项目概述
第一次自创一种编译型的编程语言及其编译器，围绕着它所实现的词法分析器、语法分析器、类型检查 & 语义分析器、代码生成器 ... 这个编译系统的任何环节均是原创，并且采用 C++ 语言手工编写，没有借助任何分析器生成工具。只是作者本人想试着从零开始编写一个编译系统，满足自己从最开始学习编程技术开始直到现在总是想独立完成的一个小小的心愿，幸运的是如今终于能够窥见其冰山一角。虽然代码过于冗杂，已经无法控制复杂度，但是总算是蹩脚地正常运行起来了，即便在程序的各个角落都可能潜伏着一些臭虫。

汇编器与虚拟机这两个部件是对编程语言的补充，因为编译器只完成了生成中间语言 IL 的功能，为了使得程序能够真正地独立运行起来，就必须先通过汇编器将 IL 汇编成字节码，然后交由虚拟机执行。而且字节码无需再进行额外处理就能直接在虚拟机上运行，这也就是所谓的「一次编译，到处运行」的特性。

这个项目最终成为了我的本科毕业设计。要学的东西还有很多，须永远保持空杯心态，勿在浮沙筑高台。

<!--
[![Logo](https://i.loli.net/2021/04/22/TEozG6QHwLqZiP1.png)](https://github.com/AloneCafe/frog/)
-->

![Logo](https://www.gnu.org/graphics/lgplv3-147x51.png)

<!--
<span style="color:blue;">项目吉祥物：开心快乐的青蛙国王 FROG 桑，由 GIMP 手工绘制（渣绘轻喷）</span>
-->

![GitHub top language](https://img.shields.io/github/languages/top/AloneCafe/frog?color=%2300ffff)
![GitHub lines](https://img.shields.io/tokei/lines/github/AloneCafe/frog?color=%233388bb)
![GitHub license](https://img.shields.io/github/license/AloneCafe/frog?color=%23ddcc88)
![GitHub repo size](https://img.shields.io/github/repo-size/AloneCafe/frog)

![GitHub branch checks state](https://img.shields.io/github/checks-status/AloneCafe/frog/master?color=%23bb1c00)
![GitHub last commit](https://img.shields.io/github/last-commit/AloneCafe/frog?color=%2322dd66)
![GitHub commit activity](https://img.shields.io/github/commit-activity/y/AloneCafe/frog?color=%23cd00dd)

## 关于 FROG
FROG 是一种简单的、结构化的、面向过程的、静态类型 & 强类型的编程语言。

本项目亦包含三个主要的组成部分，分别是
* 编译器: FC (Compiler)
* 汇编器: FAS (Assembler)
* 虚拟机: FVM (Virtual Machine)

如果在无错的情况下，编写 & 运行 FROG 程序的生命周期大致是这样的：
1. 先将写好源程序交由编译器编译处理，编译器将生成一种中间语言 (Intermediate Language)，形式上类似一种汇编语言
2. 再将上一步生成的汇编代码交由 FROG 汇编器进行汇编处理，最终将生成二进制字节码，即 FROG 字节码文件
3. 最后将上一步生成的字节码文件交由 FROG 虚拟机执行，程序进入执行状态，最终程序退出结束执行

FROG 编程语言采用类 C (C-like) 的语法，与 C、C++、C#、Java 等语言的基本语法类似，只是稍有不同之处。
本语言源程序结构简单、语法清晰明了，易于上手使用。
* ~~FROG 语法简明手册~~

## 语言特性

FROG 是一种类 C 语法所定义的编程语言，所以大多数情况下，无需介绍基础的语法，以下介绍的是 FROG 语言与其他类 C 编程语言不相同的要素。

### 基本数据类型:

|数据类型|占用字节|
|:---:|:---:|
| boolean | 1 Byte |
| byte | 1 Byte |
| char | 1 Byte |
| short | 2 Byte |
| int | 4 Byte |
| long | 8 Byte |
| float | 4 Byte |
| double | 8 Byte |

### 隐式类型转换

在实际设计上，FROG 编程语言默认支持隐式类型转换，以简化可能在数据传送过程中，宽度自动适配的工作。

除了 boolean 类型无法隐式转换到其他数据类型。对于其他数据类型，如果需要的话，编译器将按照 byte < char < short < int < long < float < double 的大小关系对各个运算节点进行自下而上的隐式转换。



### 强制类型转换

当 FROG 编译器在需要对数据宽度进行适配，但是又无法对数据进行隐式类型转换时，将会报告一个错误。不过，FROG 语言层面提供了一个强制类型转换的语法，可以直接对数据进行强制折叠或者扩展。

语法: ```@ type ( expr )```

示例:

```c
float a = 1.5;
int b = @float(a * 2);
```



### 运算符与优先级

| 优先级 | 运算作用 | 顺序 & 结合性 | 运算符 (操作数 A, B, C) |
|:---:|:---|:---|:---:|
| 1 | 取矢量长度 | 从左到右，右结合 | len(A) |
| 1 | 逻辑非 | 从左到右，右结合 | ! A |
| 1 | 按位取反 | 从左到右，右结合 | ~ A |
| 1 | 负号 | 从左到右，左结合 | - A |
| 1 | 正号 | 从左到右，左结合 | + A |
| 2 | 乘 | 从左到右，左结合 | A * B |
| 2 | 除 | 从左到右，左结合 | A / B |
| 2 | 取余 | 从左到右，左结合 | A % B |
| 3 | 加 | 从左到右，左结合 | A + B |
| 3 | 减 | 从左到右，左结合 | A - B |
| 4 | 左移 | 从左到右，左结合 | A << B |
| 4 | 右移 | 从左到右，左结合 | A >> B |
| 4 | 无符号右移 | 从左到右，左结合 | A >>> B |
| 5 | 大于等于 | 从左到右，左结合 | A >= B |
| 5 | 大于 | 从左到右，左结合 | A > B |
| 5 | 小于等于 | 从左到右，左结合 | A <= B |
| 5 | 小于 | 从左到右，左结合 | A < B |
| 6 | 不等于 | 从左到右，左结合 | A != B |
| 6 | 等于 | 从左到右，左结合 | A == B |
| 7 | 按位与 | 从左到右，左结合 | A & B |
| 8 | 按位异或 | 从左到右，左结合 | A ^ B |
| 9 | 按位或 | 从左到右，左结合 | A \| B |
| 10 | 逻辑与 | 从左到右，左结合 | A && B |
| 11 | 逻辑或 | 从左到右，左结合 | A \|\| B |
| 12 | 条件 | 特殊 | A ? B : C |
| 13 | 赋值 | 从左到右，右结合 | A = B |
| 13 | 除赋值 | 从左到右，右结合 | A /= B |
| 13 | 乘赋值 | 从左到右，右结合 | A \*= B |
| 13 | 取余赋值 | 从左到右，右结合 | A %= B |
| 13 | 加赋值 | 从左到右，右结合 | A += B |
| 13 | 减赋值 | 从左到右，右结合 | A -= B |
| 13 | 左移赋值 | 从左到右，右结合 | A <<= B |
| 13 | 右移赋值 | 从左到右，右结合 | A \>\>= B |
| 13 | 无符号右移赋值 | 从左到右，右结合 | A \>\>\>= B |
| 13 | 按位与赋值 | 从左到右，右结合 | A &= B |
| 13 | 按位异或赋值 | 从左到右，右结合 | A ^= B |
| 13 | 按位或赋值 | 从左到右，右结合 | A \|= B |
| 14 | 逗号 | 从左到右，左结合 | A , B |

### 矢量:
可以像一些语言定义数组类似的形式来定义一维或者多维的矢量，在使用上也是类似的。
> 因为在 FROG 虚拟机层面，矢量是可变长的。这意味着一般情况下，如果内存足够充裕，下标是不会溢出的。

```c
char[][] ss = { "hello\n", "world\n" };
int[] fibo = { 1, 1, 2, 3, 5, 8, 13, 21 };

print(ss[0]);
print(ss[1]);
for (int i = 0; i < len(fibo); i += 1) {
    print(fibo[i]);
    print("\n");
}
```

> 定义式中不可指定下标，下标范围将由虚拟机实现决定。

### 语句不可悬空
任何语句都必须放置在花括号构成的语句块中，此处以 if 语句为例，其他语句类似。

❌ 错误用法:
```c
if (a < 0)
    print("a is less than 0\n");
```

✔ 正确用法:
```c
if (a < 0) {
    print("a is less than 0\n");
}
```

## 汇编器 / 虚拟机相关
* [FROG 汇编 & 虚拟机简明手册](https://github.com/AloneCafe/frog/wiki/FROG-%E6%B1%87%E7%BC%96-&-%E8%99%9A%E6%8B%9F%E6%9C%BA%E7%AE%80%E6%98%8E%E6%89%8B%E5%86%8C)

## 附加功能 TODO / DONE
- ✔ 原生字符串
- ✔ 原生矢量 (变长数组)
- ✔ Mark-Sweep 垃圾回收器
- ❌ 小型基础库
- ❌ 原生函数接口
- ......