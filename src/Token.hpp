#ifndef __TOKEN_HEADER__
#define __TOKEN_HEADER__

#include <algorithm>
#include <vector>
#include <cstring>

#include "TokenDefinition.hpp"


class Token {
private:
    enum TokenType _type;
    long _begin_lines;
    long _begin_cols;
    TokenValue _val;
    
private:
    bool _isPunc(int p) const;

public:
    Token() :
            _type(TOKEN_TERMINATOR),
            _begin_lines(0),
            _begin_cols(0),
            _val() { }
            
    Token(const enum TokenType & _type, const long lines, const long cols, TokenValue ptv) :
            _type(_type),
            _begin_lines(lines),
            _begin_cols(cols),
            _val(ptv) { }
            
    Token(const enum TokenType & _type, const long lines, const long cols) :
            _type(_type),
            _begin_lines(lines),
            _begin_cols(cols),
            _val() { }
            
    Token(const enum TokenType & _type, const long lines, const long cols, const std::string & id) :
            _type(_type),
            _begin_lines(lines),
            _begin_cols(cols)
    {
        _val._str = id;
    }
    
    bool operator==(const Token & tk) const;
    
    /*
    bool operator==(const char *str) const {
        return (_type == TOKEN_LITERAL_CHARSEQ ||
                _type == TOKEN_LITERAL_STRING ||
                _type == TOKEN_ID
                ? (strcmp(_val._cstr, str) == 0)
                : false
                );
    }*/
    
    template <int Ts>
    bool isPunc() const {
        return getType() > TOKEN_ID && getType() < TOKEN_LITERAL_CHARSEQ && _isPunc(Ts);
    }
    
    bool isEnd() const;
    
    bool isId(void) const;
    
    bool isId(const char * s, bool caseSensitive = true) const;
    
    bool isId(const std::string & s) const;
    
    bool isType(const TokenType & tt) const;
    
    bool isKw() const;
    
    bool isKwPrivate() const;
    
    bool isKwProtected() const;
    
    bool isKwPublic() const;
    
    bool isKwAbstract() const;
    
    bool isKwClass() const;
    
    bool isKwExtends() const;
    
    bool isKwFinal() const;
    
    bool isKwImplements() const;
    
    bool isKwInterface() const;
    
    bool isKwFunc() const;
    
    bool isKwNew() const;
    
    bool isKwStatic() const;
    
    bool isKwBreak() const;
    
    bool isKwContinue() const;
    
    bool isKwReturn() const;
    
    bool isKwDo() const;
    
    bool isKwWhile() const;
    
    bool isKwIf() const;
    
    bool isKwElse() const;
    
    bool isKwFor() const;
    
    bool isKwInstanceof() const;
    
    bool isKwSwitch() const;
    
    bool isKwCase() const;
    
    bool isKwDefault() const;
    
    bool isKwTry() const;
    
    bool isKwCatch() const;
    
    bool isKwThrow() const;
    
    bool isKwThrows() const;
    
    bool isKwFinally() const;
    
    bool isKwImport() const;
    
    bool isKwPackage() const;
    
    bool isKwBoolean() const;
    
    bool isKwByte() const;
    
    bool isKwChar() const;
    
    bool isKwDouble() const;
    
    bool isKwFloat() const;
    
    bool isKwInt() const;
    
    bool isKwLong() const;
    
    bool isKwShort() const;
    
    bool isKwNull() const;
    
    bool isKwTrue() const;
    
    bool isKwFalse() const;
    
    bool isKwSuper() const;
    
    bool isKwThis() const;
    
    bool isKwSizeof() const;
    
    bool isKwAsm() const;
    
    bool isKwVoid() const;
    
    bool isKwGoto() const;
    
    bool isKwConst() const;
    
    const enum TokenType & getType() const;
    
    const TokenValue & getVal() const;
    
    const std::string & getId() const;
    
    long lineno() const;
    
    long colno() const;
    
    std::string toDebugString() const;
};

using TokenIter = typename std::vector<Token>::const_iterator;

/*
template <int Ts>
bool isPunc(const Token * pToken) {
    return pToken->getType() > TOKEN_ID && pToken->getType() < TOKEN_LITERAL_CHARSEQ;
}*/

#endif