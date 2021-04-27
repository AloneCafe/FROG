#ifndef __TOKEN_HEADER__
#define __TOKEN_HEADER__

#include <algorithm>
#include "TokenDefinition.hpp"

class Token; // 前置声明，积累在这里
namespace TokenHelper {
template <int Ts>
bool isPunc(const Token *);
}

class Token {
private:
    enum TokenType _type;
    long _begin_lines;
    long _begin_cols;
    TokenValue _val;

public:
    Token()
            : _type(TOKEN_TERMINATOR), _begin_lines(0), _begin_cols(0), _val() { }
    Token(const enum TokenType & _type, const long lines, const long cols, TokenValue ptv)
            : _type(_type), _begin_lines(lines), _begin_cols(cols), _val(ptv) {
    }
    Token(const enum TokenType & _type, const long lines, const long cols)
            : _type(_type), _begin_lines(lines), _begin_cols(cols), _val() {
    }
    Token(const enum TokenType & _type, const long lines, const long cols, const std::string & id)
            : _type(_type), _begin_lines(lines), _begin_cols(cols) {
        _val._str = id;
    }
    
    bool operator==(const Token & tk) const {
        return (_type == tk._type) &&
                (_type == TOKEN_LITERAL_CHARSEQ ||
                        _type == TOKEN_LITERAL_STRING ||
                        _type == TOKEN_ID
                 ? (_val._str == tk._val._str)
                 : (memcmp(&tk._val, &_val, sizeof(tk._val)) == 0)
                );
    }
    
    /*
    bool operator==(const char *str) const {
        return (_type == TOKEN_LITERAL_CHARSEQ ||
                _type == TOKEN_LITERAL_STRING ||
                _type == TOKEN_ID
                ? (strcmp(_val._cstr, str) == 0)
                : false
                );
    }*/
    
    inline bool isEnd() const {
        return _type == TOKEN_TERMINATOR;
    }
    
    inline bool isId(void) const {
        return _type == TOKEN_ID;
    }
    
    inline bool isId(const char * s, bool caseSensitive = true) const {
        if (caseSensitive)
            return _type == TOKEN_ID && (_val._str == s);
        else {
            if (_type != TOKEN_ID) return false;
            if (_val._str.length() != strlen(s)) return false;
            for (size_t i = 0; i < _val._str.length(); ++i) {
                if (toupper(s[i]) != toupper(_val._str[i])) {
                    return false;
                }
            }
            return true;
        }
    }
    
    inline bool isId(const std::string & s) const {
        return isId(s.c_str());
    }
    
    inline bool isType(const TokenType & tt) const {
        return _type == tt;
    }
    
    inline bool isKw() const {
        return _type >= TOKEN_KW_PRIVATE && _type <= TOKEN_RW_CONST;
    }
    
    template <int Ts>
    bool isPunc() const { return TokenHelper::isPunc<Ts>(this); }
    
    inline bool isKwPrivate() const { return _type == TOKEN_KW_PRIVATE; }
    inline bool isKwProtected() const { return _type == TOKEN_KW_PROTECTED; }
    inline bool isKwPublic() const { return _type == TOKEN_KW_PUBLIC; }
    inline bool isKwAbstract() const { return _type == TOKEN_KW_ABSTRACT; }
    inline bool isKwClass() const { return _type == TOKEN_KW_CLASS; }
    inline bool isKwExtends() const { return _type == TOKEN_KW_EXTENDS; }
    inline bool isKwFinal() const { return _type == TOKEN_KW_FINAL; }
    inline bool isKwImplements() const { return _type == TOKEN_KW_IMPLEMENTS; }
    inline bool isKwInterface() const { return _type == TOKEN_KW_INTERFACE; }
    inline bool isKwFunc() const { return _type == TOKEN_KW_FUNC; }
    inline bool isKwNew() const { return _type == TOKEN_KW_NEW; }
    inline bool isKwStatic() const { return _type == TOKEN_KW_STATIC; }
    inline bool isKwBreak() const { return _type == TOKEN_KW_BREAK; }
    inline bool isKwContinue() const { return _type == TOKEN_KW_CONTINUE; }
    inline bool isKwReturn() const { return _type == TOKEN_KW_RETURN; }
    inline bool isKwDo() const { return _type == TOKEN_KW_DO; }
    inline bool isKwWhile() const { return _type == TOKEN_KW_WHILE; }
    inline bool isKwIf() const { return _type == TOKEN_KW_IF; }
    inline bool isKwElse() const { return _type == TOKEN_KW_ELSE; }
    inline bool isKwFor() const { return _type == TOKEN_KW_FOR; }
    inline bool isKwInstanceof() const { return _type == TOKEN_KW_INSTANCEOF; }
    inline bool isKwSwitch() const { return _type == TOKEN_KW_SWITCH; }
    inline bool isKwCase() const { return _type == TOKEN_KW_CASE; }
    inline bool isKwDefault() const { return _type == TOKEN_KW_DEFAULT; }
    inline bool isKwTry() const { return _type == TOKEN_KW_TRY; }
    inline bool isKwCatch() const { return _type == TOKEN_KW_CATCH; }
    inline bool isKwThrow() const { return _type == TOKEN_KW_THROW; }
    inline bool isKwThrows() const { return _type == TOKEN_KW_THROWS; }
    inline bool isKwFinally() const { return _type == TOKEN_KW_FINALLY; }
    inline bool isKwImport() const { return _type == TOKEN_KW_IMPORT; }
    inline bool isKwPackage() const { return _type == TOKEN_KW_PACKAGE; }
    inline bool isKwBoolean() const { return _type == TOKEN_KW_BOOLEAN; }
    inline bool isKwByte() const { return _type == TOKEN_KW_BYTE; }
    inline bool isKwChar() const { return _type == TOKEN_KW_CHAR; }
    inline bool isKwDouble() const { return _type == TOKEN_KW_DOUBLE; }
    inline bool isKwFloat() const { return _type == TOKEN_KW_FLOAT; }
    inline bool isKwInt() const { return _type == TOKEN_KW_INT; }
    inline bool isKwLong() const { return _type == TOKEN_KW_LONG; }
    inline bool isKwShort() const { return _type == TOKEN_KW_SHORT; }
    inline bool isKwNull() const { return _type == TOKEN_KW_NULL; }
    inline bool isKwTrue() const { return _type == TOKEN_KW_TRUE; }
    inline bool isKwFalse() const { return _type == TOKEN_KW_FALSE; }
    inline bool isKwSuper() const { return _type == TOKEN_KW_SUPER; }
    inline bool isKwThis() const { return _type == TOKEN_KW_THIS; }
    inline bool isKwAsm() const { return _type == TOKEN_KW_ASM; }
    inline bool isKwVoid() const { return _type == TOKEN_KW_VOID; }
    inline bool isKwGoto() const { return _type == TOKEN_RW_GOTO; }
    inline bool isKwConst() const { return _type == TOKEN_RW_CONST; }
    
    const enum TokenType & getType() const;
    const TokenValue & getVal() const;
    const std::string & getId() const;
    long lineno() const;
    long colno() const;
    std::string toDebugString() const;
};

using TokenIter = typename std::vector<Token>::const_iterator;

#endif