#include "TokenScanner.hpp"
#include "Token.hpp"

template <>
inline bool isPunc<'@'>(const Token * pToken) {
    return pToken->isType(TOKEN_AT);
}

template <>
inline bool isPunc<'#'>(const Token * pToken) {
    return pToken->isType(TOKEN_SINGLE_HASH);
}

template <>
inline bool isPunc<'##'>(const Token * pToken) {
    return pToken->isType(TOKEN_DOUBLE_HASH);
}

template <>
inline bool isPunc<'('>(const Token * pToken) {
    return pToken->isType(TOKEN_LP);
}

template <>
inline bool isPunc<')'>(const Token * pToken) {
    return pToken->isType(TOKEN_RP);
}

template <>
inline bool isPunc<'['>(const Token * pToken) {
    return pToken->isType(TOKEN_LBRACKET);
}

template <>
inline bool isPunc<']'>(const Token * pToken) {
    return pToken->isType(TOKEN_RBRACKET);
}

template <>
inline bool isPunc<'{'>(const Token * pToken) {
    return pToken->isType(TOKEN_LBRACE);
}

template <>
inline bool isPunc<'}'>(const Token * pToken) {
    return pToken->isType(TOKEN_RBRACE);
}

template <>
inline bool isPunc<'<'>(const Token * pToken) {
    return pToken->isType(TOKEN_LT);
}

template <>
inline bool isPunc<'<='>(const Token * pToken) {
    return pToken->isType(TOKEN_LE);
}

template <>
inline bool isPunc<'>'>(const Token * pToken) {
    return pToken->isType(TOKEN_GT);
}

template <>
inline bool isPunc<'>='>(const Token * pToken) {
    return pToken->isType(TOKEN_GE);
}

template <>
inline bool isPunc<'=='>(const Token * pToken) {
    return pToken->isType(TOKEN_EQ);
}

template <>
inline bool isPunc<'!='>(const Token * pToken) {
    return pToken->isType(TOKEN_NE);
}

template <>
inline bool isPunc<'!'>(const Token * pToken) {
    return pToken->isType(TOKEN_LOGIC_NOT);
}

template <>
inline bool isPunc<'&&'>(const Token * pToken) {
    return pToken->isType(TOKEN_LOGIC_AND);
}

template <>
inline bool isPunc<'||'>(const Token * pToken) {
    return pToken->isType(TOKEN_LOGIC_OR);
}

template <>
inline bool isPunc<'~'>(const Token * pToken) {
    return pToken->isType(TOKEN_BIT_NOT);
}

template <>
inline bool isPunc<'&'>(const Token * pToken) {
    return pToken->isType(TOKEN_BIT_AND);
}

template <>
inline bool isPunc<'|'>(const Token * pToken) {
    return pToken->isType(TOKEN_BIT_OR);
}
template <>
inline bool isPunc<'^'>(const Token * pToken) {
    return pToken->isType(TOKEN_BIT_XOR);
}
template <>
inline bool isPunc<'+'>(const Token * pToken) {
    return pToken->isType(TOKEN_PLUS);
}
template <>
inline bool isPunc<'-'>(const Token * pToken) {
    return pToken->isType(TOKEN_MINUS);
}
template <>
inline bool isPunc<'*'>(const Token * pToken) {
    return pToken->isType(TOKEN_MUL);
}
template <>
inline bool isPunc<'/'>(const Token * pToken) {
    return pToken->isType(TOKEN_DIV);
}
template <>
inline bool isPunc<'%'>(const Token * pToken) {
    return pToken->isType(TOKEN_MOD);
}
template <>
inline bool isPunc<'<<'>(const Token * pToken) {
    return pToken->isType(TOKEN_LSHIFT);
}
template <>
inline bool isPunc<'>>'>(const Token * pToken) {
    return pToken->isType(TOKEN_RSHIFT);
}
template <>
inline bool isPunc<'>>>'>(const Token * pToken) {
    return pToken->isType(TOKEN_RSHIFT_ZERO);
}
template <>
inline bool isPunc<'?'>(const Token * pToken) {
    return pToken->isType(TOKEN_QUESTION);
}

template <>
inline bool isPunc<':'>(const Token * pToken) {
    return pToken->isType(TOKEN_COLON);
}

template <>
inline bool isPunc<';'>(const Token * pToken) {
    return pToken->isType(TOKEN_SEMICOLON);
}

template <>
inline bool isPunc<','>(const Token * pToken) {
    return pToken->isType(TOKEN_COMMA);
}

template <>
inline bool isPunc<'++'>(const Token * pToken) {
    return pToken->isType(TOKEN_PLUSPLUS);
}

template <>
inline bool isPunc<'--'>(const Token * pToken) {
    return pToken->isType(TOKEN_MINUSMINUS);
}

template <>
inline bool isPunc<'+='>(const Token * pToken) {
    return pToken->isType(TOKEN_ASSIGN_WITH_ADD);
}

template <>
inline bool isPunc<'-='>(const Token * pToken) {
    return pToken->isType(TOKEN_ASSIGN_WITH_SUB);
}

template <>
inline bool isPunc<'*='>(const Token * pToken) {
    return pToken->isType(TOKEN_ASSIGN_WITH_MUL);
}

template <>
inline bool isPunc<'/='>(const Token * pToken) {
    return pToken->isType(TOKEN_ASSIGN_WITH_DIV);
}

template <>
inline bool isPunc<'%='>(const Token * pToken) {
    return pToken->isType(TOKEN_ASSIGN_WITH_MOD);
}

template <>
inline bool isPunc<'&='>(const Token * pToken) {
    return pToken->isType(TOKEN_ASSIGN_WITH_BIT_AND);
}

template <>
inline bool isPunc<'|='>(const Token * pToken) {
    return pToken->isType(TOKEN_ASSIGN_WITH_BIT_OR);
}

template <>
inline bool isPunc<'^='>(const Token * pToken) {
    return pToken->isType(TOKEN_ASSIGN_WITH_BIT_XOR);
}

template <>
inline bool isPunc<'<<='>(const Token * pToken) {
    return pToken->isType(TOKEN_ASSIGN_WITH_LSHIFT);
}

template <>
inline bool isPunc<'>>='>(const Token * pToken) {
    return pToken->isType(TOKEN_ASSIGN_WITH_RSHIFT);
}

template <>
inline bool isPunc<'>>>='>(const Token * pToken) {
    return pToken->isType(TOKEN_ASSIGN_WITH_RSHIFT_ZERO);
}

template <>
inline bool isPunc<'='>(const Token * pToken) {
    return pToken->isType(TOKEN_ASSIGN);
}

template <>
inline bool isPunc<'.'>(const Token * pToken) {
    return pToken->isType(TOKEN_DOT);
}

template <>
inline bool isPunc<'->'>(const Token * pToken) {
    return pToken->isType(TOKEN_ARROW);
}


const enum TokenType & Token::getType() const { return this->_type; }

const TokenValue & Token::getVal() const { return this->_val; }

const std::string & Token::getId() const { return _val._str; }

long Token::lineno() const { return this->_begin_lines; }

long Token::colno() const { return this->_begin_cols; }

std::string Token::toDebugString() const {
    switch (_type) {
    default:
        return StringOperator::format(R"([<'%s'>, %ld, %ld])",
                TokenType2String[_type], _begin_lines,
                _begin_cols);
    case TOKEN_ID:
        return StringOperator::format(R"([<'%s', '%s'>, %ld, %ld])",
                TokenType2String[_type], _val._str.c_str(),
                _begin_lines, _begin_cols);
    case TOKEN_LITERAL_CHARSEQ:
        return StringOperator::format(R"([<'%s', '%s'>, %ld, %ld])",
                TokenType2String[_type], _val._str.c_str(),
                _begin_lines, _begin_cols);
    case TOKEN_LITERAL_STRING:
        return StringOperator::format(R"([<'%s', '%s'>, %ld, %ld])",
                TokenType2String[_type], _val._str.c_str(),
                _begin_lines, _begin_cols);
    case TOKEN_LITERAL_INT:
        return StringOperator::format(R"([<'%s', '%d'>, %ld, %ld])",
                TokenType2String[_type], _val.u._d,
                _begin_lines, _begin_cols);
    case TOKEN_LITERAL_LONG:
        return StringOperator::format(R"([<'%s', '%ld'>, %ld, %ld])",
                TokenType2String[_type], _val.u._l,
                _begin_lines, _begin_cols);
    case TOKEN_LITERAL_UNSIGNED_INT:
        return StringOperator::format(R"([<'%s', '%u'>, %ld, %ld])",
                TokenType2String[_type], _val.u._ud,
                _begin_lines, _begin_cols);
    case TOKEN_LITERAL_UNSIGNED_LONG:
        return StringOperator::format(R"([<'%s', '%lu'>, %ld, %ld])",
                TokenType2String[_type], _val.u._ul,
                _begin_lines, _begin_cols);
    case TOKEN_LITERAL_FLOAT:
        return StringOperator::format(R"([<'%s', '%E'>, %ld, %ld])",
                TokenType2String[_type], _val.u._lf,
                _begin_lines, _begin_cols);
    case TOKEN_LITERAL_DOUBLE:
        return StringOperator::format(R"([<'%s', '%lE'>, %ld, %ld])",
                TokenType2String[_type], _val.u._lf,
                _begin_lines, _begin_cols);
    case TOKEN_LITERAL_LONG_DOUBLE:
        return StringOperator::format(R"([<'%s', '%llE'>, %ld, %ld])",
                TokenType2String[_type], _val.u._llf,
                _begin_lines, _begin_cols);
    }
}
bool Token::operator==(const Token & tk) const {
    return (_type == tk._type) &&
            (_type == TOKEN_LITERAL_CHARSEQ ||
                    _type == TOKEN_LITERAL_STRING ||
                    _type == TOKEN_ID
             ? (_val._str == tk._val._str)
             : (memcmp(&tk._val, &_val, sizeof(tk._val)) == 0)
            );
}

bool Token::isEnd() const {
    return _type == TOKEN_TERMINATOR;
}

bool Token::isId(void) const {
    return _type == TOKEN_ID;
}

bool Token::isId(const char * s, bool caseSensitive) const {
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
bool Token::isId(const std::string & s) const {
    return isId(s.c_str());
}
bool Token::isType(const TokenType & tt) const {
    return _type == tt;
}
bool Token::isKw() const {
    return _type >= TOKEN_KW_PRIVATE && _type <= TOKEN_RW_CONST;
}

bool Token::isKwPrivate() const {
    return _type == TOKEN_KW_PRIVATE;
}

bool Token::isKwProtected() const {
    return _type == TOKEN_KW_PROTECTED;
}

bool Token::isKwPublic() const {
    return _type == TOKEN_KW_PUBLIC;
}

bool Token::isKwAbstract() const {
    return _type == TOKEN_KW_ABSTRACT;
}

bool Token::isKwClass() const {
    return _type == TOKEN_KW_CLASS;
}

bool Token::isKwExtends() const {
    return _type == TOKEN_KW_EXTENDS;
}

bool Token::isKwFinal() const {
    return _type == TOKEN_KW_FINAL;
}

bool Token::isKwImplements() const {
    return _type == TOKEN_KW_IMPLEMENTS;
}

bool Token::isKwInterface() const {
    return _type == TOKEN_KW_INTERFACE;
}

bool Token::isKwFunc() const {
    return _type == TOKEN_KW_FUNC;
}

bool Token::isKwNew() const {
    return _type == TOKEN_KW_NEW;
}

bool Token::isKwStatic() const {
    return _type == TOKEN_KW_STATIC;
}

bool Token::isKwBreak() const {
    return _type == TOKEN_KW_BREAK;
}

bool Token::isKwContinue() const {
    return _type == TOKEN_KW_CONTINUE;
}

bool Token::isKwReturn() const {
    return _type == TOKEN_KW_RETURN;
}

bool Token::isKwDo() const {
    return _type == TOKEN_KW_DO;
}

bool Token::isKwWhile() const {
    return _type == TOKEN_KW_WHILE;
}

bool Token::isKwIf() const {
    return _type == TOKEN_KW_IF;
}

bool Token::isKwElse() const {
    return _type == TOKEN_KW_ELSE;
}

bool Token::isKwFor() const {
    return _type == TOKEN_KW_FOR;
}

bool Token::isKwInstanceof() const {
    return _type == TOKEN_KW_INSTANCEOF;
}

bool Token::isKwSwitch() const {
    return _type == TOKEN_KW_SWITCH;
}

bool Token::isKwCase() const {
    return _type == TOKEN_KW_CASE;
}

bool Token::isKwDefault() const {
    return _type == TOKEN_KW_DEFAULT;
}

bool Token::isKwTry() const {
    return _type == TOKEN_KW_TRY;
}

bool Token::isKwCatch() const {
    return _type == TOKEN_KW_CATCH;
}

bool Token::isKwThrow() const {
    return _type == TOKEN_KW_THROW;
}

bool Token::isKwThrows() const {
    return _type == TOKEN_KW_THROWS;
}

bool Token::isKwFinally() const {
    return _type == TOKEN_KW_FINALLY;
}

bool Token::isKwImport() const {
    return _type == TOKEN_KW_IMPORT;
}

bool Token::isKwPackage() const {
    return _type == TOKEN_KW_PACKAGE;
}

bool Token::isKwBoolean() const {
    return _type == TOKEN_KW_BOOLEAN;
}

bool Token::isKwByte() const {
    return _type == TOKEN_KW_BYTE;
}

bool Token::isKwChar() const {
    return _type == TOKEN_KW_CHAR;
}

bool Token::isKwDouble() const {
    return _type == TOKEN_KW_DOUBLE;
}

bool Token::isKwFloat() const {
    return _type == TOKEN_KW_FLOAT;
}

bool Token::isKwInt() const {
    return _type == TOKEN_KW_INT;
}

bool Token::isKwLong() const {
    return _type == TOKEN_KW_LONG;
}

bool Token::isKwShort() const {
    return _type == TOKEN_KW_SHORT;
}

bool Token::isKwNull() const {
    return _type == TOKEN_KW_NULL;
}

bool Token::isKwTrue() const {
    return _type == TOKEN_KW_TRUE;
}

bool Token::isKwFalse() const {
    return _type == TOKEN_KW_FALSE;
}

bool Token::isKwSuper() const {
    return _type == TOKEN_KW_SUPER;
}

bool Token::isKwThis() const {
    return _type == TOKEN_KW_THIS;
}

bool Token::isKwAsm() const {
    return _type == TOKEN_KW_ASM;
}

bool Token::isKwVoid() const {
    return _type == TOKEN_KW_VOID;
}

bool Token::isKwGoto() const {
    return _type == TOKEN_RW_GOTO;
}

bool Token::isKwConst() const {
    return _type == TOKEN_RW_CONST;
}