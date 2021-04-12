#ifndef __TOKENSCAN_HEADER__
#define __TOKENSCAN_HEADER__

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <unordered_set>
#include <map>

#include "LittleAssist.hpp"
#include "LineScanner.hpp"
#include "ErrorDriver.hpp"
#include "Token.hpp"

extern const std::map<std::string, TokenType> Keyword2TokenType;

extern const char * TokenType2String[];

namespace TokenHelper {
template <int Ts>
inline bool isPunc(const Token * pToken) {
    return pToken->getType() > TOKEN_ID && pToken->getType() < TOKEN_LITERAL_CHARSEQ;
}
template <>
inline bool isPunc<'@'>(const Token * pToken) { return pToken->isType(TOKEN_AT); }
template <>
inline bool isPunc<'#'>(const Token * pToken) { return pToken->isType(TOKEN_SINGLE_HASH); }
template <>
inline bool isPunc<'##'>(const Token * pToken) { return pToken->isType(TOKEN_DOUBLE_HASH); }
template <>
inline bool isPunc<'('>(const Token * pToken) { return pToken->isType(TOKEN_LP); }
template <>
inline bool isPunc<')'>(const Token * pToken) { return pToken->isType(TOKEN_RP); }
template <>
inline bool isPunc<'['>(const Token * pToken) { return pToken->isType(TOKEN_LBRACKET); }
template <>
inline bool isPunc<']'>(const Token * pToken) { return pToken->isType(TOKEN_RBRACKET); }
template <>
inline bool isPunc<'{'>(const Token * pToken) { return pToken->isType(TOKEN_LBRACE); }
template <>
inline bool isPunc<'}'>(const Token * pToken) { return pToken->isType(TOKEN_RBRACE); }
template <>
inline bool isPunc<'<'>(const Token * pToken) { return pToken->isType(TOKEN_LT); }
template <>
inline bool isPunc<'<='>(const Token * pToken) { return pToken->isType(TOKEN_LE); }
template <>
inline bool isPunc<'>'>(const Token * pToken) { return pToken->isType(TOKEN_GT); }
template <>
inline bool isPunc<'>='>(const Token * pToken) { return pToken->isType(TOKEN_GE); }
template <>
inline bool isPunc<'=='>(const Token * pToken) { return pToken->isType(TOKEN_EQ); }
template <>
inline bool isPunc<'!='>(const Token * pToken) { return pToken->isType(TOKEN_NE); }
template <>
inline bool isPunc<'!'>(const Token * pToken) { return pToken->isType(TOKEN_LOGIC_NOT); }
template <>
inline bool isPunc<'&&'>(const Token * pToken) { return pToken->isType(TOKEN_LOGIC_AND); }
template <>
inline bool isPunc<'||'>(const Token * pToken) { return pToken->isType(TOKEN_LOGIC_OR); }
template <>
inline bool isPunc<'~'>(const Token * pToken) { return pToken->isType(TOKEN_BIT_NOT); }
template <>
inline bool isPunc<'&'>(const Token * pToken) { return pToken->isType(TOKEN_BIT_AND); }
template <>
inline bool isPunc<'|'>(const Token * pToken) { return pToken->isType(TOKEN_BIT_OR); }
template <>
inline bool isPunc<'^'>(const Token * pToken) { return pToken->isType(TOKEN_BIT_XOR); }
template <>
inline bool isPunc<'+'>(const Token * pToken) { return pToken->isType(TOKEN_PLUS); }
template <>
inline bool isPunc<'-'>(const Token * pToken) { return pToken->isType(TOKEN_MINUS); }
template <>
inline bool isPunc<'*'>(const Token * pToken) { return pToken->isType(TOKEN_MUL); }
template <>
inline bool isPunc<'/'>(const Token * pToken) { return pToken->isType(TOKEN_DIV); }
template <>
inline bool isPunc<'%'>(const Token * pToken) { return pToken->isType(TOKEN_MOD); }
template <>
inline bool isPunc<'<<'>(const Token * pToken) { return pToken->isType(TOKEN_LSHIFT); }
template <>
inline bool isPunc<'>>'>(const Token * pToken) { return pToken->isType(TOKEN_RSHIFT); }
template <>
inline bool isPunc<'>>>'>(const Token * pToken) { return pToken->isType(TOKEN_RSHIFT_ZERO); }
template <>
inline bool isPunc<'?'>(const Token * pToken) { return pToken->isType(TOKEN_QUESTION); }
template <>
inline bool isPunc<':'>(const Token * pToken) { return pToken->isType(TOKEN_COLON); }
template <>
inline bool isPunc<';'>(const Token * pToken) { return pToken->isType(TOKEN_SEMICOLON); }
template <>
inline bool isPunc<','>(const Token * pToken) { return pToken->isType(TOKEN_COMMA); }
template <>
inline bool isPunc<'++'>(const Token * pToken) { return pToken->isType(TOKEN_PLUSPLUS); }
template <>
inline bool isPunc<'--'>(const Token * pToken) { return pToken->isType(TOKEN_MINUSMINUS); }
template <>
inline bool isPunc<'+='>(const Token * pToken) { return pToken->isType(TOKEN_ASSIGN_WITH_ADD); }
template <>
inline bool isPunc<'-='>(const Token * pToken) { return pToken->isType(TOKEN_ASSIGN_WITH_SUB); }
template <>
inline bool isPunc<'*='>(const Token * pToken) { return pToken->isType(TOKEN_ASSIGN_WITH_MUL); }
template <>
inline bool isPunc<'/='>(const Token * pToken) { return pToken->isType(TOKEN_ASSIGN_WITH_DIV); }
template <>
inline bool isPunc<'%='>(const Token * pToken) { return pToken->isType(TOKEN_ASSIGN_WITH_MOD); }
template <>
inline bool isPunc<'&='>(const Token * pToken) { return pToken->isType(TOKEN_ASSIGN_WITH_BIT_AND); }
template <>
inline bool isPunc<'|='>(const Token * pToken) { return pToken->isType(TOKEN_ASSIGN_WITH_BIT_OR); }
template <>
inline bool isPunc<'^='>(const Token * pToken) { return pToken->isType(TOKEN_ASSIGN_WITH_BIT_XOR); }
template <>
inline bool isPunc<'<<='>(const Token * pToken) { return pToken->isType(TOKEN_ASSIGN_WITH_LSHIFT); }
template <>
inline bool isPunc<'>>='>(const Token * pToken) { return pToken->isType(TOKEN_ASSIGN_WITH_RSHIFT); }
template <>
inline bool isPunc<'>>>='>(const Token * pToken) { return pToken->isType(TOKEN_ASSIGN_WITH_RSHIFT_ZERO); }
template <>
inline bool isPunc<'='>(const Token * pToken) { return pToken->isType(TOKEN_ASSIGN); }
template <>
inline bool isPunc<'.'>(const Token * pToken) { return pToken->isType(TOKEN_DOT); }
template <>
inline bool isPunc<'->'>(const Token * pToken) { return pToken->isType(TOKEN_ARROW); }
}

// 预处理单词扫描器状态枚举
enum TokenScannerState {
    TS_INITIAL = 1,
    TS_ACCEPTING_HASH,
    TS_ACCEPTING_ID,
    TS_ACCEPTING_PLUS,
    TS_ACCEPTING_MINUS,
    TS_ACCEPTING_MUL,
    TS_ACCEPTING_DIV,
    TS_ACCEPTING_MOD,
    TS_ACCEPTING_ASSIGN,
    TS_ACCEPTING_LT,
    TS_ACCEPTING_GT,
    TS_ACCEPTING_BIT_AND,
    TS_ACCEPTING_BIT_OR,
    TS_ACCEPTING_LSHIFT,
    TS_ACCEPTING_RSHIFT,
    TS_ACCEPTING_RSHIFT_ZERO,
    TS_ACCEPTING_XOR,
    TS_ACCEPTING_LOGIC_NOT,
    TS_ACCEPTING_LITERAL_STRING,
    TS_ACCEPTING_LITERAL_CHAR,
    TS_ACCEPTING_LITERAL_STRING_ESCAPE_QUOT,
    TS_ACCEPTING_LITERAL_CHAR_ESCAPE_QUOT,
    TS_ACCEPTING_LITERAL_NUMBER,
    TS_ACCEPTING_DECIMAL_PART_BEGIN_WITH_DOT,
    TS_ACCEPTING_DECIMAL_PART_BEGIN_WITH_DIGIT,
    TS_ACCEPTING_BEGIN_WITH_ZERO,
    TS_ACCEPTING_BEGIN_WITH_NO_ZERO,
    TS_ACCEPTING_SUFFIX_U,
    TS_ACCEPTING_SUFFIX_L,
    TS_ACCEPTED_HEX_SEQ_HEADER,
    TS_ACCEPTING_OCT_SEQ,
    TS_ACCEPTING_DECIMAL_BEGIN_WITH_ZERO,
    TS_ACCEPTING_HEX_SEQ,
    TS_ACCEPTING_SUFFIX_UL,
    TS_ACCEPTING_EXP_PART,
    TS_ACCEPTING_EXP_WITH_SIGN,
    TS_ACCEPTING_EXP_DEFAULT
};

/* 作用于行的单词扫描器
    输入组合行对象，输出单词序列 */
class TokenScanner {
private:
    // 当前扫描的文件名字
    std::string _filename;
    // 起始扫描的行和列
    long _begin_lineno;
    long _begin_colno;
    
    CombinedLineDriver _cld;
    ErrorDriver _ed;
    
    std::vector<Token> _token_seq; // 保存单词序列的底层容器
    long _curr_pos; // 单词序列位置的指示器
    
    enum ReadAction { // 决定扫描器的工作方式
        READ_ACTION_NORMAL,
        READ_ACTION_PLAINTEXT,
        READ_ACTION_PLAINTEXT_UNTIL,
        READ_ACTION_NORMAL_WITH_BLANK
    } _read_action;
    
    std::unordered_set<char> _until_char_list;

public:
    using CommentState = ValidCharScanner::CommentState;

private:
    Token & realNextToken();

public:
    explicit TokenScanner(const CombinedLine & combined_line, const std::string & filename)
            : _filename(filename), _begin_lineno(combined_line.getBeginLineNo()), _begin_colno(0),
              _cld(combined_line), _ed(), _token_seq(), _curr_pos(0), _read_action(READ_ACTION_NORMAL) {
    }
    explicit TokenScanner(const CombinedLine & combined_line)
            : _filename("<stdin>"), _begin_lineno(combined_line.getBeginLineNo()), _begin_colno(0),
              _cld(combined_line), _ed(), _token_seq(), _curr_pos(0), _read_action(READ_ACTION_NORMAL) {
    }
    TokenScanner() = delete;
    TokenScanner(const TokenScanner &) = delete;
    virtual ~TokenScanner() = default;
    
    void setReadPlainText() { _read_action = READ_ACTION_PLAINTEXT; }
    void setReadNormal() { _read_action = READ_ACTION_NORMAL; }
    void setReadNormalWithBlank() { _read_action = READ_ACTION_NORMAL_WITH_BLANK; }
    void setReadPlainTextUntil(
            std::initializer_list<char> until) { _until_char_list = until, _read_action = READ_ACTION_NORMAL_WITH_BLANK; }
    
    bool isReadPlainText() { return _read_action == READ_ACTION_PLAINTEXT; }
    bool isReadNormal() { return _read_action == READ_ACTION_NORMAL; }
    bool isReadNormalWithBlank() { return _read_action == READ_ACTION_NORMAL_WITH_BLANK; }
    bool isReadPlainTextUntil() { return _read_action == READ_ACTION_PLAINTEXT_UNTIL; }
    
    const Token & nextToken(); // 扫描器获取下一单词，且指示器前进
    const Token & currentToken() const; // 扫描器获取当前单词
    const Token & previousToken(); // 扫描器获取上一单词，且指示器回退
    
    
    void forward(); // 令指示器前进
    void rollback(); // 令指示器回退
    
    // 直接获取底层 vector 容器的函数
    auto getContainer() const -> const std::vector<Token> & { return _token_seq; }
    
    // 是否发生过错误
    bool hasErr() const { return _ed.hasErr(); }
    
    // 获取错误列表的函数
    auto getErrList() const -> decltype(_ed.getErrList()) { return _ed.getErrList(); }
    
    // 运算符 >> 重载，用于获取下一单词
    TokenScanner & operator>>(Token & token) { return (token = this->nextToken()), *this; }
    
};

#endif