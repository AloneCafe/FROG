#ifndef __CHARSCAN_HEADER__
#define __CHARSCAN_HEADER__

#include <stdexcept>
#include <fstream>
#include "UTFSequence.hpp"
#include "LittleAssist.hpp"

/** 字符信息结构 */
struct UtfCharInfo {
    UtfChar _ch;
    long _lineno;
    long _colno;
    UtfCharInfo() :
        _ch(),
        _lineno(0),
        _colno(0) { }
        
    UtfCharInfo(UtfChar ch, long lineno, long colno) :
        _ch(ch),
        _lineno(lineno),
        _colno(colno) { }
    
    bool isAsciChar() const { return _ch.isAsciChar(); }
    
    bool isUtfCharSeq() const { return _ch.isUtfCharSeq(); }
    
    UtfChar operator()() { return _ch; }
    
    bool operator==(char c) { return _ch == c; }
    
    bool operator!=(char c) { return _ch != c; }
    
    bool isalpha() { return _ch.isAsciChar() && ::isalpha(_ch.getAsciChar()); }
    
    bool isdigit() { return _ch.isAsciChar() && ::isdigit(_ch.getAsciChar()); }
    
    bool isblank() { return _ch.isAsciChar() && ::isblank(_ch.getAsciChar()); }
    
    bool isalnum() { return _ch.isAsciChar() && ::isalnum(_ch.getAsciChar()); }
    
    bool iscntrl() { return _ch.isAsciChar() && ::iscntrl(_ch.getAsciChar()); }
    
    bool isgraph() { return _ch.isAsciChar() && ::isgraph(_ch.getAsciChar()); }
    
    bool isprint() { return _ch.isAsciChar() && ::isprint(_ch.getAsciChar()); }
    
    bool islower() { return _ch.isAsciChar() && ::islower(_ch.getAsciChar()); }
    
    bool isupper() { return _ch.isAsciChar() && ::isupper(_ch.getAsciChar()); }
    
    bool ispunct() { return _ch.isAsciChar() && ::ispunct(_ch.getAsciChar()); }
    
    bool isspace() { return _ch.isAsciChar() && ::isspace(_ch.getAsciChar()); }
    
    bool isxdigit() { return _ch.isAsciChar() && ::isxdigit(_ch.getAsciChar()); }
    
    bool isodigit() { return _ch.isAsciChar() && (_ch.getAsciChar() >= '0' && _ch.getAsciChar() <= '7'); }
    
    void toupper() { _ch.setAsciChar(::toupper(_ch.getAsciChar())); }
    
    void tolower() { _ch.setAsciChar(::tolower(_ch.getAsciChar())); }
    
    bool isEof() { return _ch.isAsciChar() && _ch.getAsciChar() == CHARACTER_EOF; }
    
    int xdigit2int();
    
    int odigit2int();
    
    int digit2int();
};

/** 基本字符扫描器 (BasicCharScanner)，便于用户从文件输入流对象中抽取字符等操作 */
class BasicCharScanner {
private:
    std::ifstream * _pIfs = nullptr;
    std::istream *_pIs = nullptr;
    long _curr_pos;
    UtfString _str;
    UtfChar & realNextChar();

public:
    int navGet() {
        return ((_pIfs != nullptr) ? (_pIfs->get()) : (_pIs->get()));
    }
    void navUnGet() {
        ((_pIfs != nullptr) ? (_pIfs->unget()) : (_pIs->unget()));
    }
    
public:
    explicit BasicCharScanner(std::ifstream & ifs)
            : _pIfs(&ifs), _curr_pos(0), _str() {}
    explicit BasicCharScanner(std::istream & is)
            : _pIs(&is), _curr_pos(0), _str() {}
    UtfChar & nextChar();
    UtfChar & currentChar();
    UtfChar & previousChar();
    void rollback();
    bool isEof();
};

/** 有效字符扫描器 (ValidCharScanner)，提供基于 BasicCharScanner 提供的基本
	底层操作, 且将忽略单行注释和块注释的字符抽取操作 */
class ValidCharScanner {
public:
    enum CommentState {
        NOCOMMENT = 1,
        BEGINNING_LINECOMMENT,
        BEGINNING_BLOCKCOMMENT,
        LINECOMMENT,
        LINECOMMENT_CONTINUE,
        BLOCKCOMMENT,
        ENDING_BLOCKCOMMENT
    };

private:
    BasicCharScanner _bcs; // BasicCharScanner 底层封装的字符扫描器
    
    StateIndicator<enum CommentState> _csi; // 注释状态指示器
    
    UtfString _str; // 存放忽略单行注释和块注释的字符
    long _curr_pos;
    
    UtfChar & realNextChar();

public:
    explicit ValidCharScanner(std::ifstream & ifs)
            : _bcs(ifs), _csi(NOCOMMENT), _str(), _curr_pos(0) { }
    explicit ValidCharScanner(std::istream & is)
            : _bcs(is), _csi(NOCOMMENT), _str(), _curr_pos(0) { }
            
    ValidCharScanner(std::ifstream & ifs, enum CommentState cs)
            : _bcs(ifs), _csi(cs), _str(), _curr_pos(0) { }
    ValidCharScanner(std::istream & is, enum CommentState cs)
            : _bcs(is), _csi(cs), _str(), _curr_pos(0) { }
    
    UtfChar & nextChar();
    UtfChar & currentChar();
    UtfChar & previousChar();
    void rollback();
    bool isEof();
};

#endif