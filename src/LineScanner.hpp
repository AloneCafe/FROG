#ifndef __LINESCAN_HEADER__
#define __LINESCAN_HEADER__

#include <stdexcept>
#include <utility>
#include <vector>
#include <iostream>

#include "CharScanner.hpp"

/* SingleLine 单纯的单行，存放此行所有字符的字符串将不包含换行符在内 */
using SingleLine = UtfString;

/* CombinedLine 组合行包含一个或多个单纯的单行。如果一行代码中有行延续符（反斜线接着换行符），
	那么此行与此行的下一行将共同构成 CombinedLine 对象，且下一行与下下一行的情况视有无行延续符都以此类推 */
class CombinedLine {
public:
    // 组合行的类型：终止记号或者程序行
    enum CombinedLineType {
        Terminator, ProgramLine
    };
private:
    CombinedLineType _type; // 组合行类型
    long _begin_lineno; // 起始行号
    std::vector<SingleLine> _single_lines; // 组合行的底层容器

public:
    CombinedLine() = default;
    CombinedLine(CombinedLineType type, long begin_lineno, const std::vector<SingleLine> & line)
            : _type(type), _begin_lineno(begin_lineno), _single_lines(line) { }
    
    CombinedLineType getType() const { return this->_type; }
    std::vector<SingleLine> & getCombinedLine() { return this->_single_lines; }
    SingleLine & getSingleLine(const long i) { return this->_single_lines[i]; }
    const SingleLine & getSingleLine(const long i) const { return this->_single_lines[i]; }
    long getSingleLineCount() const { return this->_single_lines.size(); }
    long getBeginLineNo() const { return this->_begin_lineno; }
    long getLength() const;
    long size() const;
    std::string toString();
    UtfCharInfo operator[](long pos) const;
};

/* CombinedLineDriver 提供了对于 CombinedLine 对象的一组流式操作的封装，
	除了一般情况下提供 isEof() 函数来判定流是否结束，对应的操作函数在流结束后被调用时，
	将返回 CHARACTER_EOF 以便于调用者进行判定 */
class CombinedLineDriver {
private:
    const CombinedLine & _combined_line;
    long _curr_pos;

public:
    explicit CombinedLineDriver(const CombinedLine & combined_line)
            : _combined_line(combined_line), _curr_pos(0) { }
    UtfCharInfo nextChar(); // 驱动获取下一字符，且指示器前进
    UtfCharInfo currentChar(); // 驱动获取当前字符
    UtfCharInfo previousChar(); // 驱动获取上一字符，且指示器回退
    void rollback(); // 令指示器回退
};

/* LineScanner 行扫描器能够对输入流进行扫描，每一次将扫描一个组合行，生成一个 CombinedLine 对象；
	LineScanner 负责将组合行对象存储到底层容器中，并提供了较为灵活的抽取方法以便于其他模块调用 */
class LineScanner {
private:
    std::vector<CombinedLine> _combined_lines; // 行扫描器的底层容器，保存扫描到的每一组合行
    long _curr_pos; // 行扫描器位置的指示器
    ValidCharScanner _vcs; // ValidCharScanner 对象，对字符流进行底层的行注释和块注释的过滤处理
    
    CombinedLine & realNextLine();
public:
    explicit LineScanner(std::ifstream & ifs)
            : _combined_lines(), _curr_pos(0), _vcs(ifs) { }
    explicit LineScanner(std::istream & is)
            : _combined_lines(), _curr_pos(0), _vcs(is) { }
    //LineScanner() : _combined_lines(), _curr_pos(0), _vcs(static_cast<std::ifstream &>(std::cin)) {}
    
    const CombinedLine & nextLine(); // 扫描器获取下一行，且指示器前进
    const CombinedLine & currentLine() const; // 扫描器获取当前行
    const CombinedLine & previousLine(); // 扫描器获取上一行，且指示器回退
    void rollback(); // 令指示器回退
    
    // 直接获取底层 vector 容器的函数
    auto getContainer() const -> const std::vector<CombinedLine> & { return this->_combined_lines; }
    
    // 运算符 >> 重载，用于获取下一行
    LineScanner & operator>>(CombinedLine & combined_line) { return (combined_line = this->nextLine()), *this; }
};

#endif
