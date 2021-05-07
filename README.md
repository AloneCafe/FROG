# 🐸 FROG 编程语言

*The little green frog,
Jumps on a log,
Takes off his cloak,
And begins to croak.*


## 🏛 概述
第一次自创一种编译型的编程语言及其编译器，围绕着它所实现的词法分析器、语法分析器、类型检查 & 语义分析器、代码生成器 ... 这个编译系统的任何环节均是原创，并且采用 C++ 语言手工编写，没有借助任何分析器生成工具。只是作者本人想试着从零开始编写一个编译系统，满足自己从最开始学习编程技术开始直到现在总是想独立完成的一个小小的心愿，幸运的是如今终于能够窥见其冰山一角。虽然代码过于冗杂，加上底层代码逻辑繁琐且未复用其他库，以及本人水平有限的缘故，编写了一些相对高耦合的代码，在现有的精力下已经无法控制复杂度。最近，总算是蹩脚地正常运行起来了，即便在程序的各个角落都可能潜伏着一些臭虫。

汇编器与虚拟机这两个部件是对编程语言的补充，因为编译器只完成了生成中间语言 IL 的功能，为了使得程序能够真正地独立运行起来，就必须先通过汇编器将 IL 汇编成字节码，然后交由虚拟机执行。而且字节码无需再进行额外处理就能直接在虚拟机上运行，这也就是所谓的「一次编译，到处运行」的特性。

这个小项目最终成为了我的本科毕业设计。要学的东西还有很多，须永远保持空杯心态，勿在浮沙筑高台。

<!--
[![Logo](https://i.loli.net/2021/04/22/TEozG6QHwLqZiP1.png)](https://github.com/AloneCafe/frog/)
-->



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

🌸💮🌹🌺🌻🌼🌷🌸💮🌹🌺🌻🌼🌷🌸💮🌹🌺🌻🌼🌷

![LGPL-Logo](https://www.gnu.org/graphics/lgplv3-147x51.png)&nbsp;&nbsp;
![BY-NC-SA-Logo](https://mirrors.creativecommons.org/presskit/buttons/80x15/png/by-nc-sa.png)

本项目所包含的程序源代码均是在 [GNU LGPL 3.0 许可协议](https://www.gnu.org/licenses/lgpl-3.0.txt) 下授权的自由软件。本文的原有内容 (不包含链接指向的内容和外部图片的部分) 是在 [知识共享署名-非商业性使用-相同方式共享 4.0 国际许可协议](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode.zh-Hans) (英文简写 BY-NC-SA 4.0) 下授权的文档创作。

## 🏷 目录
* [FROG 编程语言](#-frog-编程语言)
  * [概述](#-概述)
  * [关于 FROG](#-关于-frog)
  * [附加计划 TODO / DONE](#-附加计划-todo--done)
  * [构建项目](#-构建项目)
  * [使用说明](#-使用说明)
  * [FROG 语言特性](#-frog-语言特性)
    * [基本数据类型](#基本数据类型)
    * [隐式类型转换](#隐式类型转换)
    * [强制类型转换](#强制类型转换)
    * [全局变量](#全局变量)
    * [函数定义](#函数定义)
    * [函数重载](#函数重载)
    * [运算符与优先级](#运算符与优先级)
    * [矢量](#矢量)
    * [语句不可悬空](#语句不可悬空)
    * [内联 IL](#内联-il)
    * [自动指派返回值](#自动指派返回值)
    * [字符串字面量](#字符串字面量)
  * [汇编器 / 虚拟机相关](#-汇编器--虚拟机相关)
    * [FROG 虚拟机](#frog-虚拟机)
    * [META 指令](#meta-指令)
    * [标签与跳转](#标签与跳转)
    * [关于粒度](#关于粒度)
    * [变量定义 &amp; 传送](#变量定义--传送)
    * [直接型操作数栈指令](#直接型操作数栈指令)
    * [矢量的创建 &amp; 度量](#矢量的创建--度量)
    * [矢量的变址、解引用 &amp; 传送](#矢量的变址解引用--传送)
    * [基本算术运算](#基本算术运算)
    * [算术比较运算](#算术比较运算)
    * [逻辑运算](#逻辑运算)
    * [位运算 (无浮点粒度)](#位运算-无浮点粒度)
    * [粒度转换:  操作数的裁剪与增补](#粒度转换--操作数的裁剪与增补)
    * [VOID 置换空间](#void-置换空间)
    * [函数的调用与返回](#函数的调用与返回)
    * [虚拟机扩展功能 (VMEF)](#虚拟机扩展功能-vmef)
    * [其他指令](#其他指令)
    * [指令的字节码 (Little\-Endian)](#指令的字节码-little-endian)


## 📜 关于 FROG
FROG 是一种简单的、结构化的、面向过程的、静态类型 & 强类型的编程语言。

🐳 吃我大秘笈啦：⬆⬆⬇⬇⬅➡⬅➡🅱🅰🅱🅰

本项目亦包含三个主要的组成部分，分别是
* 编译器：FC  (Frog Compiler)
* 汇编器：FAS (Frog Assembler)
* 虚拟机：FVM (Frog Virtual Machine)

如果在无错的情况下，编写 & 运行 FROG 程序的生命周期大致是这样的：
1. 先将写好源程序交由编译器编译处理，编译器将生成一种中间语言 (Intermediate Language)，形式上类似一种汇编语言
2. 再将上一步生成的汇编代码交由 FROG 汇编器进行汇编处理，最终将生成二进制字节码，即 FROG 字节码文件
3. 最后将上一步生成的字节码文件交由 FROG 虚拟机执行，程序进入执行状态，最终程序退出结束执行

FROG 编程语言采用类 C (C-like) 的语法，与 C、C++、C#、Java 等语言的基本语法类似，只是稍有不同之处。
本语言源程序结构简单、语法清晰明了，易于上手使用。

## 📈 附加计划 TODO / DONE
- ✔ 函数重载
- ✔ 原生字符串
- ✔ 原生矢量 (变长数组)
- ✔ Mark-Sweep 垃圾回收器
- ❌ 小型基础库
- ❌ 原生函数接口
- ......

---

## 👨🏻‍💻 构建项目
本项目全部程序的构建只依赖 C++ 11 及其配套的 C++ 标准库，不含任何第三方依赖。为了构建 FROG 编译系统，需要使用 CMake 构建工具来进行本操作。同时，本项目是跨平台的，构建的前提条件是需要系统中正确安装了所需版本的 C++ 编译器和基础构建工具，比如 GNU/Linux 上面的 ```make```、```g++``` 和 Windows 上面的 ```Visual Studio```。

- 克隆源代码仓库、进入源代码目录

  \> ``` git clone https://github.com/AloneCafe/frog.git ```

  \> ``` cd frog ```

- 建立并进入构建输出目录

  \> ``` mkdir build ```

  \> ``` cd build ```

- 使用 CMake 完成前期环境检查，并且生成 Makefile 脚本

  \> ``` cmake .. ```

- 构建项目

  依据编译环境的不同，选择并使用 make 或者 msbuild 完整地编译、构建出项目程序。若需要一些高级配置，请自行查阅 CMake 与编译环境的一些相关资料。

  - GNU/Linux 或 MinGW: \> ``` make ```
  
  - Microsoft Visual Studio 控制台: \> ``` msbuild frog.sln```
  
如果生成没有错误，将在生成目录下或者某个子目录中生成三个可执行文件 ```fc```、```fas```、```fvm```，分别是编译器程序、汇编器程序与虚拟机程序三件套。

## 📒 使用说明
本项目的所有程序都对文件名和扩展名不敏感，只专注于内容处理，所以输入与输出文件的扩展名均可以任意指定，但为了解释方便，一般是做特殊处理以便区分。

* 编译器

  编译器可以有一个或多个输入文件作为源代码，编译时将做合并处理，如果需要引入库函数或者库变量，那么需要附加 ```-e name``` 参数指定引入 ```name``` 库，多个库需要用多个 ```-e``` 参数来引入。如果附加 ```-I``` 参数，编译器将从标准输入获取源代码。指定 ```-o path``` 将编译器生成的中间汇编代码输出至 ```path``` 路径指定的文件，特别地，如果指定了 ```-O``` 参数，编译器将会把生成的中间汇编代码输出至标准输出。

* 汇编器

  汇编器至多可接受一个输入文件作为中间汇编代码，如果附加 ```-I``` 参数，汇编器将从标准输入获取中间汇编代码。指定 ```-o path``` 将汇编器生成的字节码输出至 ```path``` 路径指定的文件，特别地，如果指定了 ```-O``` 参数，编译器将会把生成的字节码以十六进制 (文本串) 的形式输出至标准输出。

* 虚拟机

  虚拟机至多可接受一个输入文件作为字节码，虚拟机将按顺序执行该字节码，直到退出。虚拟机接受 ```-v``` 参数和 ```-s``` 参数分别是使得虚拟机运行在输出调试信息 (verbose) 模式和单步 (step) 模式下。

* 简单示例

  以编译源程序 ```test.frog``` 为例，首先需要将 ```test.frog``` 编译成 ```test.fas``` 中间汇编代码文件: 

  \> ``` fc test.frog -o test.fas ```

  再将中间汇编代码文件作为汇编器的输入，生成 ```test.fvm``` 字节码文件:

  \> ``` fas test.fas -o test.fvm ```

  使用虚拟机加载字节码文件并且执行:

  \> ``` fvm test.fvm ```
  
* 联合编译
  
  通过命令行的管道特性皮一下的话，编译的过程是可以一步到位的:
  
  \> ``` fc test.frog -O | fas -I -o test.fvm ```
  
  然后运行
  
  \> ``` fvm test.fvm ```

> 若需另外的参数用法信息，可以指定 ```-h``` 参数来获得提示。

---

## 💼 FROG 语言特性

FROG 是一种类 C 语法所定义的编程语言，所以大多数情况下，无需介绍基础的语法，以下介绍的是 FROG 语言与其他类 C 编程语言不相同的要素。

### 基本数据类型

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

### 全局变量

FROG 编程语言支持在源文件的顶层语句块中定义全局变量，以变量类型开头并且后接变量名、初始化表达式 (如果有的话)。编译器编译全局变量与引用该全局变量的语句时，其对全局变量定义的先后顺序不敏感的，即全局变量可以在某处引用该全局变量的下文中才被定义出来。

### 函数定义

与全局变量类似，FROG 编程语言支持在源文件的顶层语句块中定义自定义函数，以 ```func``` 关键字开头并且后接返回值类型、函数名、参数列表与函数体。编译器编译函数与函数调用语句时，其对定义函数的先后顺序不敏感的，即函数可以在某处调用的下文中才被定义出来。

### 函数重载

FROG 支持基于参数列表区别的函数重载特性。在参数的个数与类型的不同前提下，函数可以同名。函数调用时编译器将通过判断，优先选择一个合适的函数 (因为包含参数的隐式类型转换) 作为实际的函数调用。


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

### 矢量
可以像一些语言定义数组类似的形式来定义一维或者多维的矢量，定义矢量后即初始化，并且矢量将立即可用 (与 Java 初始化时需要使用 new 操作符不同)。矢量在使用上也是与一些编程语言的数组是类似的。
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

### 内联 IL
FROG 从语言层面允许在源程序的函数体中内联 IL，最终编译器将该部分保留，并与其他部分一起生成中间汇编代码文件。

使用 ```asm``` 关键字所定义的语句块来表示内联汇编的区块，其中加入一条或者多条由双引号括起的表示 IL 语句的字符串，比如下面这样：

```c
func void print(int a) {
    asm {
        "PUSH DW @a;"
        "EFCALL \"stdout_ni\";"
    }
}
```

在内联 IL 中，可以通过 ```@``` 标记引用非内联部分的变量名字，以便在 IL 中直接使用变量。除了 ```@``` 标记的变量引用之外，字符串将原封不动地输出到中间汇编代码文件。

> 在多个 IL 字符串之间，编译器将自动加入换行符以示分割，但编译器不保证 IL 的语法正确性，同样也不保证内联 IL 之后，程序的堆栈平衡与执行的正确性。

### 自动指派返回值

无论函数是否具备返回值，编译器将在调用该函数的指令后生成处理返回值的代码。所以编译器有必要考虑函数中的某个没有显式指定 return 的分支的情况。所以 FROG 在默认情况下允许不编写 return 语句，无论如何，编译器都将自动在函数控制流的末尾自动生成一个使得函数正常返回的指令，以免贯穿执行到非法指令区域，而无论返回值类型为何，编译器将始终将其的内存字节全部填充为零，接着作为返回值返回给调用者代码。

### 字符串字面量

与大多数主流的编程语言一样，在 FROG 源程序中，以 ```"``` 开始并结束的一串字符即字符串字面量。FROG 编译器最终会使得该字符串字面量规范化成 char 类型的一维矢量，并且以 0 字节作为结尾元素。因此在各个层面上，char 类型的一维矢量是与字符串字面量在数据类型上等价的。

---

## ⚙ 汇编器 / 虚拟机相关

* [汇编器 / 虚拟机相关](#-汇编器--虚拟机相关)
    * [FROG 虚拟机](#frog-虚拟机)
    * [META 指令](#meta-指令)
    * [标签与跳转](#标签与跳转)
    * [关于粒度](#关于粒度)
    * [变量定义 &amp; 传送](#变量定义--传送)
    * [直接型操作数栈指令](#直接型操作数栈指令)
    * [矢量的创建 &amp; 度量](#矢量的创建--度量)
    * [矢量的变址、解引用 &amp; 传送](#矢量的变址解引用--传送)
    * [基本算术运算](#基本算术运算)
    * [算术比较运算](#算术比较运算)
    * [逻辑运算](#逻辑运算)
    * [位运算 (无浮点粒度)](#位运算-无浮点粒度)
    * [粒度转换:  操作数的裁剪与增补](#粒度转换--操作数的裁剪与增补)
    * [函数的调用与返回](#函数的调用与返回)
    * [虚拟机扩展功能 (VMEF)](#虚拟机扩展功能-vmef)
    * [其他指令](#其他指令)
    * [指令的字节码 (Little-Endian)](#指令的字节码-little-endian)



### FROG 虚拟机

FROG 虚拟机是一个假想的、抽象的栈式计算机模型。

从虚拟机的运行时刻来看，它目前由 5 个部分组成:

- 字节码指令 (OPROM)
- 操作数栈 (OPSTACK)
- 函数调用栈 (FNSTACK)

* 标量 RAM (SRAM, Scalar-RAM)
* 矢量 RAM (VRAM, Vector-RAM)

> 为避免混淆，此处的 SRAM、VRAM ... 之类的称谓并非是计算机科学领域内通常指代事物的缩写。

其中，OPROM 是字节码文件被加载到虚拟机后，字节码指令所存放的地方，虚拟机将从这里取出指令并按照程序编译好的流程执行。

OPSTACK 是虚拟机执行大多数依赖外部输入、或者将产生输出的运算，所必须用到的空间，更准确地说，OPSTACK 在字节码执行的时候充当了寄存器的角色，其中的数据都是具有暂存性质的，OPSTACK 的空间一般会经常变化。

SRAM 是程序中用于存放标量的空间，所谓标量一般就是指 FROG 源程序层面的、以基本数据类型定义的变量。

相对应地，VRAM 是程序中用于存放矢量的空间，而矢量一般指 FROG 源程序层面定义的数组 (未来可能会扩充)。除了句柄 (或者说指向数组的指针) 以标量 (通常是 4 Byte 整数) 的形式放在 SRAM 中，数组本体是存放于 VRAM 中的，知道

> 如果不能理解的话，可以通过搜索引擎搜索与「栈式计算机」或者「栈式虚拟机」相关的词条获得更多提示信息。




### META 指令

因为程序文件的物理结构必须符合某种规范才能被虚拟机正确解析，因此为了锚定程序的结构，FROG 汇编语言必须具备一定程度的 meta 指令 (或者也可以说是伪指令) 功能，它们只是在程序中起到标记或者锚定程序结构的作用。实际上，作者在设计这个汇编语言的时候，并不想做的如同微软的 MASM 一样那么多的伪指令，我想让本语言拥有更少的伪指令，以期降低复杂度，使得汇编器的工作更纯粹 😉。在 FROG 程序结构中只需要锚定至多两种程序段就足够了，一种是「静态段」，另一种是「函数段」。静态段是当字节码文件被虚拟机装载之后就无条件地被执行的代码段 (这一点我是吸收了 Java 类加载器的原理)；函数段是每一个被定义的函数在编译后自动生成的独立代码段，当一个函数被调用时，就会从该函数段的开头开始执行指令。

* ```.STATIC```

  静态段指令: 标记一个静态段的开头

* ```.FUNC name```

  函数段指令: 标记一个名字为 name 的函数段的开头

* ```.END```

  段闭合指令: 标记一个最近的静态段或者函数段的结束



### 标签与跳转

在汇编器对汇编代码进行汇编处理之前，包括编译器对源代码进行编译之前，每一条指令所在的地址是未被确定的，对于指令长度不固定的指令集尤其是这样。其实从 FROG 源程序层面上就需要实现非线性的控制流，比如各种意义的跳转，分支、循环，甚至的函数调用 (在 FROG 里面函数调用另外实现) 这种特殊的跳转，那么就需要给需要的指令 —— 它们各个跳入、跳出位置做出标记，以便汇编器至少在完成第一遍汇编过后，对每一处跳入位置确定地址。在这个基础上再将每一处跳出位置进行重定位 (relocation)。

> 为了避免歧义，这里笔者所说的跳入位置指的是控制流从别处跳转到此处，那么此处就是跳入位置。同理，跳出位置指的是控制流从此处跳转到别处，那么此处就是跳出位置。

我们在汇编层面可以使用标签 (label) 定义标记，可以标记一处跳入位置。

* ``` # name :```

  标签定义式 (跳入位置)，标签以「#」开头，并且以「:」结尾，中间是标签名 ```name```

同样，编写跳转指令需要指定一个标签，可以标记一个跳出位置。目前 FROG 虚拟机支持三种跳转指令

* ```J #name```

  无条件跳转: 直接跳出到标签名为 ```name``` 的位置

* ```JT #name```

  真值跳转: 从操作数栈中弹出 1 Byte，若是真值则跳出到标签名为 ```name``` 的位置

* ```JF #name```

  假值跳转: 从操作数栈中弹出 1 Byte，若是假值则跳出到标签名为 ```name```的位置



### 关于粒度

* 粒度 (granularity)

  粒度有七种，其中六种可以在 FROG 源程序层面分别对应一种甚至多种数据类型，它们分别是 ```B, W, DW, QW, FLT, DBL``` ，可以用表格来表示它们之间的联系。

  | 粒度 | 所描述的字节数           | 对应的源程序中的数据类型 |
    | ---- | ------------------------ | ------------------------ |
  | B    | 1 Byte                   | boolean, char, byte      |
  | W    | 2 Byte                   | short                    |
  | DW   | 4 Byte                   | int                      |
  | QW   | 8 Byte                   | long                     |
  | FLT  | 4 Byte (IEEE 754 单精度) | float                    |
  | DBL  | 8 Byte (IEEE 754 双精度) | double                   |

  > 最后一种是 ```VOID``` ，它比较特殊，我们在数据裁剪与增补指令「RSZ」中会讲到有关 ```VOID``` 粒度的用法。



### 变量定义 & 传送

「变量」这个概念究其根本就只是一块存储区域罢了，所以变量 (数据) 的定义与传送几乎是任何机器 (真实机或者虚拟机) 所必备的功能，在 FROG 汇编语言中，变量定义与传送总是需要附带「粒度」参数，就如同大多数汇编语言一样，来指定指令所能 (必须) 处理的数据宽度。

* ```DEF gran name```

  DEF 指令:  变量的定义 (definition)，FROG 汇编语言支持通过指定数据名称与粒度来定义指定大小的变量，而变量最终被汇编时，将会被汇编器用一个确定的索引地址替代。

  其中 ```gran``` 字段用上文中提到的粒度缩写替代，而 ```name``` 则是需要指定的变量名称。譬如 ```DEF DW i``` 表示定义一个名为 i 且大小为的 4 Byte 整数变量。

* ```PUSH gran name```

  PUSH 指令:  从变量 ```name``` 中取出粒度为 ```gran``` 的数据，并压入操作数栈。

* ```POP gran name```

  POP 指令:  从操作数栈顶弹出粒度为 ```gran``` 的数据，并存入变量 ```name``` 中。

* ```TOP gran name```

  TOP 指令:  从操作数栈顶取得粒度为 ```gran``` 的数据 (但不弹出)，并存入变量 ```name``` 中。



### 直接型操作数栈指令

* ```IPUSH gran imme```

  IPUSH 指令:  从 ```imme``` 立即数中取得粒度为 ```gran``` 的数据，将其压入操作数栈。(```imme``` 立即数将被汇编器随着指令本体一同被编码)

* ```DUP gran```

  DUP 指令:  将操作数栈顶的 ```gran``` 粒度的数据，复制一份相同的数据，再压入操作数栈顶。



### 矢量的创建 & 度量

矢量是虚拟机维护的一块至少在逻辑上是连续的、以某个粒度对齐的可变长序列。在 FROG 源程序层面通常用于实现一维或者多维数组、字符串等功能。

* ```MKVEC degree gran```

  MKVEC 指令:  虚拟机将创建一个维度为 ```degree``` ，且其中元素粒度为 ```gran``` 的矢量，并且将其对应的「矢量句柄」压入操作数栈。

* ```LEN```

  LEN 指令:  计算矢量的长度，从操作数栈中弹出一个粒度为 ```DW``` 的数据作为「矢量句柄」，计算并得到该矢量的长度 (粒度为 ```DW```)，将其压入操作数栈。



### 矢量的变址、解引用 & 传送

* ```OFFSET```

  OFFSET 指令:  从操作数栈顶依次弹出两个粒度为 ```DW``` 的数据作为 OFFSET 指令的参数。按照弹栈顺序，分别是「下标」和「矢量句柄」。虚拟机将依据这两个参数计算得到一个指向对应下标元素实际内存的原生地址 (大小由 ABI 决定)，虚拟机得到这个原生内存地址之后，将其压入操作数栈。

* ```HPUSH gran```

  HPUSH 指令:  从操作数栈顶弹出一个粒度为 ```DW``` 或 ```QW``` 的数据 (具体取决于 ABI) 作为「原生内存地址」。虚拟机将该原生地址进行解引用，取出粒度为 ```gran``` 的数据，然后将其压入操作数栈。

* ```HPOP gran```

  HPOP 指令:  从操作数栈顶弹出一个粒度为 ```gran``` 的数据作为「数据」，然后再次从操作数栈顶弹出一个粒度为 ```DW``` 或 ```QW``` 的数据 (具体取决于 ABI) 作为「原生内存地址」。虚拟机将该数据以 ```gran``` 为粒度，写入原生内存地址所指示的内存位置。



### 基本算术运算

* ```ADD gran```

  ADD 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 ADD 指令的操作数。按照弹栈顺序，虚拟机将对这两个数据进行算术加法运算，再将运算结果以 ```gran``` 为粒度压入操作数栈。

* ```SUB gran```

  SUB 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 SUB 指令的操作数。按照弹栈顺序，虚拟机将对这两个数据进行算术减法运算，再将运算结果以 ```gran``` 为粒度压入操作数栈。

* ```MUL gran```

  MUL 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 MUL 指令的操作数。按照弹栈顺序，虚拟机将对这两个数据进行算术乘法运算，再将运算结果以 ```gran``` 为粒度压入操作数栈。

* ```DIV gran```

  DIV 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 DIV 指令的操作数。按照弹栈顺序，虚拟机将对这两个数据进行算术除法运算，再将运算结果以 ```gran``` 为粒度压入操作数栈。

* ```MOD gran```

  MOD 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 MOD 指令的操作数。按照弹栈顺序，虚拟机将对这两个数据进行算术取模运算，再将运算结果以 ```gran``` 为粒度压入操作数栈。

* ```NEG gran```

  NEG 指令:  从操作数栈顶弹出一个粒度为 ```gran``` 的数据作为 NEG 指令的操作数。虚拟机将对这个数据取得对应的相反数，再将运算结果以 ```gran``` 为粒度压入操作数栈。



### 算术比较运算

* ```LT gran```

  LT 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 LT 指令的操作数。按照弹栈顺序，虚拟机将比较前者是否小于后者，再将结果真值 (粒度为 ```B```) 压入操作数栈。

* ```LE gran```

  LE 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 LE 指令的操作数。按照弹栈顺序，虚拟机将比较前者是否小于或等于后者，再将结果真值 (粒度为 ```B```) 压入操作数栈。

* ```EQ gran```

  EQ 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 EQ 指令的操作数。按照弹栈顺序，虚拟机将比较前者是否等于后者，再将结果真值 (粒度为 ```B```) 压入操作数栈。

* ``` NE gran```

  NE 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 NE 指令的操作数。按照弹栈顺序，虚拟机将比较前者是否不等于后者，再将结果真值 (粒度为 ```B```) 压入操作数栈。

* ```GE gran```

  GE 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 GE 指令的操作数。按照弹栈顺序，虚拟机将比较前者是否大于或等于后者，再将结果真值 (粒度为 ```B```) 压入操作数栈。

* ```GT gran```

  GT 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 GT 指令的操作数。按照弹栈顺序，虚拟机将比较前者是否大于后者，再将结果真值 (粒度为 ```B```) 压入操作数栈。


### 逻辑运算

* ```LAND```

  LAND 指令:  从操作数栈顶依次弹出两个粒度为 ```B``` 的数据作为 LAND 指令的操作数。按照弹栈顺序，虚拟机将对这两个真值进行逻辑与运算，再将结果真值 (粒度为 ```B```) 压入操作数栈。

* ```LOR```

  LOR 指令:  从操作数栈顶依次弹出两个粒度为 ```B``` 的数据作为 LOR 指令的操作数。按照弹栈顺序，虚拟机将对这两个真值进行逻辑或运算，再将结果真值 (粒度为 ```B```) 压入操作数栈。

* ```LNOT```

  LNOT 指令:  从操作数栈顶弹出一个粒度为 ```B``` 的数据作为 LNOT 指令的操作数。按照弹栈顺序，虚拟机将对这个真值取反，再将结果真值 (粒度为 ```B```) 压入操作数栈。



### 位运算 (无浮点粒度)

在 FROG 的各个层面，位运算均不支持浮点数 (粒度为 ```FLT``` 和 ```DBL```) 作为操作数。

* ```BAND gran```

  BAND 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 BAND 指令的操作数。按照弹栈顺序，虚拟机将对这两个数据进行按位与运算，再将运算结果以 ```gran``` 为粒度压入操作数栈。

* ```BOR gran```

  BOR 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 BOR 指令的操作数。按照弹栈顺序，虚拟机将对这两个数据进行按位或运算，再将运算结果以 ```gran``` 为粒度压入操作数栈。

* ```BXOR gran```

  BXOR 指令:  从操作数栈顶依次弹出两个粒度为 ```gran``` 的数据作为 BXOR 指令的操作数。按照弹栈顺序，虚拟机将对这两个数据进行按位异或运算，再将运算结果以 ```gran``` 为粒度压入操作数栈。

* ``` BNOT gran```

  BNOT 指令:  从操作数栈顶弹出一个粒度为 ```gran``` 的数据作为 BNOT 指令的操作数。虚拟机将对这个操作数进行按位取反运算，再将运算结果以 ```gran``` 为粒度压入操作数栈。

* ```SHL gran```

  SHL 指令:  先从操作数栈顶弹出一个粒度为 ```gran``` 的数据作为操作数，接着再从操作数栈顶弹出一个粒度为 ```B``` 的数据作为左移的移位数。该左移将产生运算结果，虚拟机将比较前者是否大于或等于后者，再将运算结果以 ```gran``` 为粒度压入操作数栈。

* ```SHR gran```

  SHR 指令 (左侧空当补充符号位):  先从操作数栈顶弹出一个粒度为 ```gran``` 的数据作为操作数，接着再从操作数栈顶弹出一个粒度为 ```B``` 的数据作为逻辑右移的移位数。该右移将产生运算结果，再将运算结果以 ```gran``` 为粒度压入操作数栈。

* ```SHRZ gran```

  SHRZ 指令 (其中 Z 表示 Zero，左侧空当补充 0):  先从操作数栈顶弹出一个粒度为 ```gran``` 的数据作为操作数，接着再从操作数栈顶弹出一个粒度为 ```B``` 的数据作为算术右移的移位数。该右移将产生运算结果，虚拟机将比较前者是否大于后者，再将运算结果以 ```gran``` 为粒度压入操作数栈。

> 如果不理解「算术右移」和「逻辑右移」的区别，可以另寻资料



### 粒度转换:  操作数的裁剪与增补

有时候需要对操作数栈顶的数据进行某种形式的「裁剪」或者「增补」，以使得操作数的粒度适用于新的运算指令并且维持栈平衡。从 FROG 源程序的角度来讲，无论是隐式类型转换还是强制类型转换，都最终会表现成对操作数栈顶的某个粒度数据的裁剪与增补。

* ```RSZ gran1 gran2```

  RSZ 指令:  先从操作数栈顶弹出一个粒度为 ```gran1``` 的数据作为操作数，然后将其转换为粒度为 ```gran2``` 的数据，再重新压入操作数栈顶。
    
### VOID 置换空间

* VOIDHOLE 机制:  RSZ 指令的两个参数均可以是特殊的粒度 —— ```VOID```，当试图将操作数从某个粒度转换为 ```VOID```，虚拟机将为其保存一份原有粒度的拷贝，并将操作数完全弹栈，接着如果在之后的某个指令中又试图将操作数从 ```VOID``` 粒度转换为其他粒度，则虚拟机将取出之前的一份原有粒度的拷贝，并转换为现在指定的粒度，接着压入操作数栈。

> 采用名为 VOIDHOLE 的置换空间这个机制是为了编译器代码生成时，在某些无需向上层代码递交结果的代码 (比如纯表达式语句)，继续地保持堆栈平衡。




### 函数的调用与返回

从 FROG 源程序的角度来讲，在编译不发生错误的情况下，编译器将自动生成函数调用与返回的代码，这些代码将保证函数调用、参数传递、函数返回全过程中的操作数栈、函数调用栈平衡。但是，从 FROG 汇编层面来讲，函数调用并不强制要求函数调用过程中参数传递、函数返回的正确性，函数的参数传递必须手工编写操作数栈指令，且函数的返回也需要根据返回值的具体粒度编写返回指令，以确保函数调用栈的平衡。

源程序意义上的函数在 FROG 编译器编译后，将为函数本体生成一个以函数名与参数列表类型为唯一标识的签名 (sign name by name mangling)，这样便于实现函数重载。

* ```CALL signname ```

  CALL 指令:  先将下一条指令的位置压入函数调用栈，以便函数将来返回。然后跳转到签名为 ```signname ``` 所对应的函数开头开始执行指令。

* ```RET gran ```

  RET 指令:  有返回值的函数返回指令，```gran``` 指定返回值的粒度。指令执行前，一般需要将返回值以 ```gran``` 为粒度自行压入操作数栈，所以其实该指令只完成函数返回，指定 ```gran``` 粒度也只是虚拟机需要在操作数栈中越过返回值，找到并弹出函数返回地址，接着跳转至该处。

* ```NRET```

  NRET 指令:  无返回值的函数返回指令，该指令只完成函数返回，从函数调用栈顶弹出函数返回地址，接着跳转至该处。



### 虚拟机扩展功能 (VMEF)

* ```EFCALL "fn"```

  EFCALL 指令:  直接调用功能名字 (字符串) 为 ```fn``` 的虚拟机扩展功能，关于扩展功能如下。

    * VMEF 已提供的功能名以及作用:

    1. ```stderr_nl```:  将操作数栈顶的整数作为粒度 ```QW``` 弹出并输出到标准错误 (文本)
    2. ```stderr_ni```:  将操作数栈顶的整数作为粒度 ```DW``` 弹出并输出到标准错误 (文本)
    3. ```stderr_ns```:  将操作数栈顶的整数作为粒度 ```W``` 弹出并输出到标准错误 (文本)
    4. ```stderr_nb```:  将操作数栈顶的整数作为粒度 ```B``` 弹出并输出到标准错误 (文本)
    5. ```stderr_flt  ```:  将操作数栈顶的 ```FLT``` 弹出并输出到标准错误 (文本)
    6. ```stderr_dbl  ```:  将操作数栈顶的 ```DBL``` 弹出并输出到标准错误 (文本)
    7. ```stderr_c```:  将操作数栈顶的 ```B``` 弹出并作为字符的形式输出到标准错误
    8. ```stdout_s```:  将操作数栈顶的矢量句柄 (```DW```) 弹出，并将其转化为字符串打印至标准错误



    1. ```stdout_nl```:  将操作数栈顶的整数作为粒度 ```QW``` 弹出并输出到标准输出(文本)
    2. ```stdout_ni```:  将操作数栈顶的整数作为粒度 ```DW``` 弹出并输出到标准输出 (文本)
    3. ```stdout_ns```:  将操作数栈顶的整数作为粒度 ```W``` 弹出并输出到标准输出 (文本)
    4. ```stdout_nb```:  将操作数栈顶的整数作为粒度 ```B``` 弹出并输出到标准输出 (文本)
    5. ```stdout_flt  ```:  将操作数栈顶的 ```FLT``` 弹出并输出到标准输出 (文本)
    6. ```stdout_dbl  ```:  将操作数栈顶的 ```DBL``` 弹出并输出到标准输出 (文本)
    7. ```stdout_c```:  将操作数栈顶的 ```B``` 弹出并作为字符的形式输出到标准输出
    8. ```stdout_s```:  将操作数栈顶的矢量句柄 (```DW```) 弹出，并将其转化为字符串打印至标准输出

    

    1. ```stdin_nl```:  从标准输入转化一个 ```QW``` (从文本转化) 并压入操作数栈顶
    2. ```stdin_ni```:  从标准输入转化一个 ```DW``` (从文本转化) 并压入操作数栈顶
    3. ```stdin_ns```:  从标准输入转化一个 ```W``` (从文本转化) 并压入操作数栈顶
    4. ```stdin_nb```:  从标准输入转化一个 ```B``` (从文本转化) 并压入操作数栈顶
    5. ```stdin_flt  ```:  从标准输入转化一个 ```FLT``` (从文本转化) 并压入操作数栈顶
    6. ```stdin_dbl  ```:  从标准输入转化一个 ```DBL``` (从文本转化) 并压入操作数栈顶
    7. ```stdin_c```:  从标准输入读取一个 ```B``` (字符形式) 并压入操作数栈顶
    8. ```stdin_s```:  从标准输入读取一个字符串 (回车或 EOF 中止) 并转化为矢量 (char[]) 压入操作数栈顶



### 其他指令

* ```NOP```

  NOP 指令:  空指令，虚拟机不执行任何操作。

* ```HALT```

  HALT 指令:  虚拟机停机指令，虚拟机立即终止执行，并将操作数栈上的一个粒度为 ```DW``` 的数据作为系统返回值。



### 指令的字节码 (Little-Endian)

* 粒度编码:  VOID=0000, B=0001, W=0010, DW=0100, QW=1000, FLT=1011, DBL=1111

| 二进制码                                                | 功能   | 十六进制 / 说明              |
| ------------------------------------------------------- | ------ | ---------------------------- |
| 00000000                                                | NOP    | 00                           |
| 00000001                                                | HALT   | 01                           |
| 00000010  ... (字符串 Until 00000000)                   | EFCALL | 02 ... (字符串 Until 00)     |
| 00000011  rrrrrrrr rrrrrrrr rrrrrrrr rrrrrrrr           | CALL   | 03 RR RR RR RR (重定位地址)  |
| 00000100                                                | NRET   | 04                           |
| 00000101  gggg0000                                      | RET    | 05 G0                        |
| 00000110  gggghhhh                                      | RSZ    | 06 GH                        |
| 00001000  gggg0000                                      | BAND   | 08 G0                        |
| 00001001  gggg0000                                      | BOR    | 09 G0                        |
| 00001010  gggg0000                                      | BXOR   | 0A G0                        |
| 00001011  gggg0000                                      | BNOT   | 0B G0                        |
| 00001100  gggg0000                                      | SHL    | 0C G0                        |
| 00001101  gggg0000                                      | SHR    | 0D G0                        |
| 00001110  gggg0000                                      | SHRZ   | 0E G0                        |
| 00010000  gggg0000                                      | LT     | 10 G0                        |
| 00010001  gggg0000                                      | LE     | 11 G0                        |
| 00010010  gggg0000                                      | EQ     | 12 G0                        |
| 00010011  gggg0000                                      | NE     | 13 G0                        |
| 00010100  gggg0000                                      | GE     | 14 G0                        |
| 00010101  gggg0000                                      | GT     | 15 G0                        |
| 00011000                                                | LNOT   | 18                           |
| 00011001                                                | LOR    | 19                           |
| 00011010                                                | LAND   | 1A                           |
| 00100000  gggg0000                                      | ADD    | 20 G0                        |
| 00100001  gggg0000                                      | SUB    | 21 G0                        |
| 00100010  gggg0000                                      | MUL    | 22 G0                        |
| 00100011  gggg0000                                      | DIV    | 23 G0                        |
| 00100100  gggg0000                                      | MOD    | 24 G0                        |
| 00100101  gggg0000                                      | NEG    | 25 G0                        |
| 00110000                                                | OFFSET | 30                           |
| 00110001  gggg0000                                      | HPUSH  | 31 G0                        |
| 00110010  gggg0000                                      | HPOP   | 32 G0                        |
| 00110011                                                | LEN    | 33                           |
| 00110100  ggggdddd                                      | MKVEC  | 34 GD (维度D, 粒度G)         |
| 01000000  gggg0000  ... (视粒度而定)                    | IPUSH  | 40 G0 ... (视粒度而定)       |
| 01000001  gggg0000                                      | DUP    | 41 G0                        |
| 01010000  gggg0000  rrrrrrrr rrrrrrrr rrrrrrrr rrrrrrrr | DEF    | 50 G0 RR RR RR RR (变量偏移) |
| 01010001  gggg0000  rrrrrrrr rrrrrrrr rrrrrrrr rrrrrrrr | PUSH   | 51 G0 RR RR RR RR (变量偏移) |
| 01010010  gggg0000  rrrrrrrr rrrrrrrr rrrrrrrr rrrrrrrr | POP    | 52 G0 RR RR RR RR (变量偏移) |
| 01010011  gggg0000  rrrrrrrr rrrrrrrr rrrrrrrr rrrrrrrr | TOP    | 53 G0 RR RR RR RR (变量偏移) |
| 01100000  rrrrrrrr rrrrrrrr rrrrrrrr rrrrrrrr           | J      | 60 RR RR RR RR (重定位地址)  |
| 01100001  rrrrrrrr rrrrrrrr rrrrrrrr rrrrrrrr           | JT     | 61 RR RR RR RR (重定位地址)  |
| 01100010  rrrrrrrr rrrrrrrr rrrrrrrr rrrrrrrr           | JF     | 62 RR RR RR RR (重定位地址)  |

