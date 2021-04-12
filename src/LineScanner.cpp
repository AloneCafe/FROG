#include "LineScanner.hpp"

long CombinedLine::getLength() const {
    long character_count = 0;
    for (auto single_line : this->_single_lines) character_count += single_line.length();
    return character_count;
}

long CombinedLine::size() const {
    return this->getLength();
}

std::string CombinedLine::toString() {
    UtfString res;
    long len = getLength();
    for (long i = 0; i < len; i ++) res.push((*this)[i]._ch);
    return res.toString();
}

UtfCharInfo CombinedLine::operator[](long pos) const {
    if (pos < 0) throw std::out_of_range("下标访问越界"); // 排除下标为负数的情形
    long single_lines_count = this->getSingleLineCount();
    long lineno = this->getBeginLineNo();
    for (long i = 0; i < single_lines_count; i ++) {
        long len = this->getSingleLine(i).length();
        if (pos < len) { // 在本行内
            // 构造并返回字符信息对象
            UtfChar uc = const_cast<SingleLine &>(getSingleLine(i))[pos];
            // i 作为行号，pos 作为按 UTF-8 字符序列计算的列号
            return UtfCharInfo(uc, i, pos);
        } else { // 在本行外则取差，并在下一行进行判断
            pos -= len;
            continue;
        }
    }
    // 下标超过实际组合行总长度
    throw std::out_of_range("下标访问越界");
}

UtfCharInfo CombinedLineDriver::nextChar() {
    const long len = _combined_line.size();
    if (_curr_pos < 0)
        throw std::out_of_range("指示器所处的当前位置非法");
    if (_curr_pos >= len) {
        return UtfCharInfo(CHARACTER_EOF, 0, 0);
    }
    UtfCharInfo res = _combined_line[_curr_pos ++];
    return res;
}

UtfCharInfo CombinedLineDriver::currentChar() {
    const long len = _combined_line.size();
    if (_curr_pos < 1)
        throw std::out_of_range("指示器所处的当前位置非法");
    if (_curr_pos > len)
        return UtfCharInfo(CHARACTER_EOF, 0, 0);
    UtfCharInfo res = _combined_line[_curr_pos - 1];
    return res;
}

UtfCharInfo CombinedLineDriver::previousChar() {
    const long len = _combined_line.size();
    if (_curr_pos < 2)
        throw std::out_of_range("指示器所处的当前位置非法");
    if (_curr_pos > len)
        return UtfCharInfo(CHARACTER_EOF, 0, 0);
    UtfCharInfo res = _combined_line[-- _curr_pos - 1];
    return res;
}

void CombinedLineDriver::rollback() {
    if (_curr_pos < 1)
        throw std::out_of_range("指示器已至行扫描器起始位置");
    _curr_pos --;
}

CombinedLine & LineScanner::realNextLine() {
    std::vector<SingleLine> single_line_vec;
    SingleLine single_line;
    
    // 核心分行的算法
    for (UtfChar uc1 = '\0'; (uc1 = _vcs.nextChar()) != CHARACTER_EOF;) {
        if (uc1 == '\\') {
            UtfChar uc2 = _vcs.nextChar();
            if (uc2 == '\n') {
                single_line_vec.push_back(single_line);
                single_line.clear();
            } else {
                single_line.push(uc1);
                _vcs.rollback();
            }
        } else if (uc1 != '\n') {
            single_line.push(uc1);
        } else { // 遇到换行符
            single_line_vec.push_back(single_line);
            single_line.clear();
            break;
        }
    }
    
    // 遭遇 CHARACTER_EOF 之后，需捕获 single_line 缓冲区的剩余字符
    if (! single_line.empty()) single_line_vec.push_back(single_line);
    
    long begin_lineno; // 计算当前组合行的起始行号
    if (! this->_combined_lines.empty()) {
        auto last_combined_line_it = (this->_combined_lines.end() - 1);
        begin_lineno = last_combined_line_it->getBeginLineNo() + last_combined_line_it->getSingleLineCount();
    } else {
        begin_lineno = 0;
    }
    
    // 先假定是普通源程序组合行
    auto combined_line_type = CombinedLine::CombinedLineType::ProgramLine;
    
    // 判断是否一行单行也没有读取到，即空的源文件
    if (single_line_vec.empty()) {
        _combined_lines.emplace_back(CombinedLine::CombinedLineType::Terminator, begin_lineno, single_line_vec);
        this->_curr_pos ++;
        return _combined_lines.back();
    }
    
    // 消除最后一行 SingleLine 末尾的空格 TODO refactor
    SingleLine & last_single_line = single_line_vec.back();
    for (long i = long(last_single_line.length()) - 1; i >= 0; i --) {
        if (last_single_line[i].isAsciChar() && isblank(last_single_line[i].getAsciChar())) last_single_line.pop();
        else break;
    }
    
    // 在此构造并保存 CombinedLine 对象
    CombinedLine combine_line(combined_line_type, begin_lineno, single_line_vec);
    _combined_lines.push_back(combine_line);
    this->_curr_pos ++;
    return _combined_lines.back();
}

const CombinedLine & LineScanner::nextLine() {
    // 如果指示器到了末尾，则向输入流获取新的一行，
    // 否则取指示器 _curr_pos 指向的一行
    long len = this->_combined_lines.size();
    if (this->_curr_pos > len || this->_curr_pos < 0)
        throw std::out_of_range("指示器所处的当前位置非法");
    if (this->_curr_pos == len)
        return this->realNextLine();
    const CombinedLine & combine_line = _combined_lines[this->_curr_pos ++];
    return combine_line;
}

const CombinedLine & LineScanner::currentLine() const {
    long len = this->_combined_lines.size();
    if (this->_curr_pos > len || this->_curr_pos < 1)
        throw std::out_of_range("指示器所处的当前位置非法");
    const CombinedLine & combine_line = _combined_lines[this->_curr_pos - 1];
    return combine_line;
}

const CombinedLine & LineScanner::previousLine() {
    long len = this->_combined_lines.size();
    if (this->_curr_pos > len || this->_curr_pos < 2)
        throw std::out_of_range("指示器所处的当前位置非法");
    const CombinedLine & combine_line = _combined_lines[-- this->_curr_pos - 1];
    return combine_line;
}

void LineScanner::rollback() {
    if (this->_curr_pos < 1)
        throw std::out_of_range("指示器已至行扫描器起始位置");
    this->_curr_pos --;
}
