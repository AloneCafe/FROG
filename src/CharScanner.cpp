#include "CharScanner.hpp"

UtfChar & BasicCharScanner::realNextChar() {
    char c = navGet();
    char utfCharSeqBuff[SEQMAXLEN];
    if (c != CHARACTER_EOF && c & 0B10000000) {
        // 1zzzzzzz, 各种 UTF-8 字符
        if ((c & 0B11100000) == 0B11000000) {
            utfCharSeqBuff[0] = c;
            utfCharSeqBuff[1] = navGet();
        } else if ((c & 0B11110000) == 0B11100000) {
            utfCharSeqBuff[0] = c;
            utfCharSeqBuff[1] = navGet();
            utfCharSeqBuff[2] = navGet();
        } else if ((c & 0B11111000) == 0B11110000) {
            utfCharSeqBuff[0] = c;
            utfCharSeqBuff[1] = navGet();
            utfCharSeqBuff[2] = navGet();
            utfCharSeqBuff[3] = navGet();
        }
    } else {
        // 0zzzzzzz, 简单 ASCII 字符
        utfCharSeqBuff[0] = c;
    }
    UtfChar uc(utfCharSeqBuff);
    _str.push(uc);
    UtfChar & res = _str[_curr_pos ++];
    return res;
}

UtfChar & BasicCharScanner::nextChar() {
    // 如果指示器到了末尾，则向输入流获取新的字符，
    // 否则取指示器 _curr_pos 指向的字符
    const long len = _str.length();
    if (_curr_pos > len || _curr_pos < 0)
        throw std::out_of_range("指示器所处的当前位置非法");
    if (_curr_pos == len)
        return realNextChar();
    UtfChar & res = _str[_curr_pos ++];
    return res;
}

UtfChar & BasicCharScanner::currentChar() {
    const long len = _str.length();
    if (_curr_pos > len || _curr_pos < 1)
        throw std::out_of_range("指示器所处的当前位置非法");
    UtfChar & res = _str[_curr_pos - 1];
    return res;
}

UtfChar & BasicCharScanner::previousChar() {
    const long len = _str.length();
    if (_curr_pos > len || _curr_pos < 2)
        throw std::out_of_range("指示器所处的当前位置非法");
    UtfChar & res = _str[-- _curr_pos - 1];
    return res;
}

void BasicCharScanner::rollback() {
    if (_curr_pos < 1)
        throw std::out_of_range("指示器已至行扫描器起始位置");
    _curr_pos --;
}

bool BasicCharScanner::isEof() {
    if (_curr_pos < long(_str.length())) return false;
    char c = navGet();
    if (c != CHARACTER_EOF) {
        navUnGet();
        return false;
    }
    return true;
}

UtfChar & ValidCharScanner::realNextChar() {
    UtfChar uc1 = _bcs.nextChar(), uc2;
    // 如果已在注释中，则返回空格字符
    switch (_csi()) {
    case LINECOMMENT:
        if (uc1 == '\\') { // 判断换行出现的转义情形
            if (isEof()) {
                goto label_ret;
            }
            uc2 = _bcs.nextChar();
            if (uc2 == '\n') {
                _csi.stateTransfer(LINECOMMENT_CONTINUE);
            }
            _bcs.rollback(); // 退回 uc2 取出的字符
            uc1 = ' ';
        } else if (uc1 == '\n') {
            _csi.stateTransfer(NOCOMMENT);
            // 遇到换行，行注释结束，且换行字符不做替换（还是输出 '\n'）
        } else if (uc1 == CHARACTER_EOF) {
            goto label_ret; // 在此处已修复bug, 可能使得字符流读取死循环
            // 在注释中,也可能遇到 EOF, 此时不应该执行空格替换
        } else { // 其他字符在注释中，全部替换成空格
            uc1 = ' ';
        }
        goto label_ret;
    
    case LINECOMMENT_CONTINUE:
        // uc1 必定是 '\n'
        _csi.stateTransfer(LINECOMMENT); // 行注释影响延续行的情形
        goto label_ret;
    
    case BLOCKCOMMENT:
        if (uc1 == '*') {
            if (isEof()) { // 块注释将一直延续到后续行，直到遇到闭合标记
                uc1 = ' ';
                goto label_ret;
            }
            uc2 = _bcs.nextChar();
            if (uc2 == '/') {
                _csi.stateTransfer(ENDING_BLOCKCOMMENT); // 正在处理闭合标记
            }
            _bcs.rollback(); // 退回 uc2 取出的字符
            uc1 = ' ';
            goto label_ret; // 已经确定是否是闭合标记，返回空格字符
        } else if (uc1 == CHARACTER_EOF) {
            goto label_ret; // 在此处已修复bug, 可能使得字符流读取死循环
            // 在注释中,也可能遇到 EOF, 此时不应该执行空格替换
        } else if (uc1 == '\n') { // 在块注释中换行则不做替换（还是输出 '\n'）
            goto label_ret;
        } else {
            uc1 = ' ';
            goto label_ret;
        }
    
    case BEGINNING_LINECOMMENT:
        // uc1 必定是 '/'
        _csi.stateTransfer(LINECOMMENT);
        uc1 = ' ';
        goto label_ret;
    
    case BEGINNING_BLOCKCOMMENT:
        // uc1 必定是 '*'
        _csi.stateTransfer(BLOCKCOMMENT);
        uc1 = ' ';
        goto label_ret;
    
    case ENDING_BLOCKCOMMENT:
        // uc1 必定是 '/'
        _csi.stateTransfer(NOCOMMENT);
        uc1 = ' ';
        goto label_ret; // 闭合标记，返回空格字符
    }
    
    if (isEof()) goto label_ret; // 如果扫描器只能扫到一个字符(uc1)，这后面当然不可能是注释，直接返回该字符
    
    uc2 = _bcs.nextChar();
    
    if (uc1 == '/') {
        if (uc2 == '/') {
            _csi.stateTransfer(BEGINNING_LINECOMMENT);
            uc1 = ' '; // 已经确定此处是注释开头，返回空格字符
        } else if (uc2 == '*') {
            _csi.stateTransfer(BEGINNING_BLOCKCOMMENT);
            uc1 = ' '; // 已经确定此处是注释开头，返回空格字符
        }
        _bcs.rollback(); // 退回 uc2 取出的字符
    } else {
        _bcs.rollback(); // 退回 uc2 取出的字符
    }
    
    label_ret:
    if (_curr_pos >= long(_str.length()))
        _str.push(uc1);
    else
        _str[_curr_pos] = uc1;
    UtfChar & res = _str[_curr_pos ++];
    return res;
}

UtfChar & ValidCharScanner::nextChar() {
    // 如果指示器到了末尾，则向 BasicCharScanner 获取新的字符，
    // 否则取指示器 _curr_pos 指向的字符
    const long len = _str.length();
    if (_curr_pos > len || _curr_pos < 0)
        throw std::out_of_range("指示器所处的当前位置非法");
    if (_curr_pos == len)
        return realNextChar();
    UtfChar & res = _str[_curr_pos ++];
    return res;
}

UtfChar & ValidCharScanner::currentChar() {
    const long len = _str.length();
    if (_curr_pos > len || _curr_pos < 1)
        throw std::out_of_range("指示器所处的当前位置非法");
    UtfChar & res = _str[_curr_pos - 1];
    return res;
}

UtfChar & ValidCharScanner::previousChar() {
    const long len = _str.length();
    if (_curr_pos > len || _curr_pos < 2)
        throw std::out_of_range("指示器所处的当前位置非法");
    UtfChar & res = _str[-- _curr_pos - 1];
    return res;
}

void ValidCharScanner::rollback() {
    if (_curr_pos < 1)
        throw std::out_of_range("指示器已至行扫描器起始位置");
    _curr_pos --;
}

bool ValidCharScanner::isEof() {
    if (_curr_pos < long(_str.length())) return false;
    UtfChar uc = _bcs.nextChar();
    if (uc != CHARACTER_EOF) {
        _bcs.rollback();
        return false;
    }
    return true;
}

int UtfCharInfo::xdigit2int() {
    char ch = _ch.getAsciChar();
    switch (ch) {
    case 'a':
    case 'A':
        return 0xa;
    case 'b':
    case 'B':
        return 0xb;
    case 'c':
    case 'C':
        return 0xc;
    case 'd':
    case 'D':
        return 0xd;
    case 'e':
    case 'E':
        return 0xe;
    case 'f':
    case 'F':
        return 0xf;
    default:
        if (ch >= '0' && ch <= '9') return ch - '0';
    }
    return 0; // 源字符不满足条件, 返回 0
}

int UtfCharInfo::odigit2int() {
    char ch = _ch.getAsciChar();
    if (ch >= '0' && ch <= '7') return ch - '0';
    return 0; // 源字符不满足条件, 返回 0
}

int UtfCharInfo::digit2int() {
    char ch = _ch.getAsciChar();
    if (ch >= '0' && ch <= '9') return ch - '0';
    return 0; // 源字符不满足条件, 返回 0
}
