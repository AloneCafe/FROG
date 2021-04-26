#include "TokenScanner.hpp"

#define POST_PT_E(x) postErr((x), _filename, lineno, colno)

const char * TokenType2String[] = {
        /*[TOKEN_TERMINATOR] = */ "TERMINATOR",
        /*[TOKEN_ID] = */ "ID",
        /*[TOKEN_AT] = */ "@",
        /*[TOKEN_SINGLE_HASH] = */ "#",
        /*[TOKEN_DOUBLE_HASH] = */ "##",
        
        /*[TOKEN_LP] = */ "(",
        /*[TOKEN_RP] = */ ")",
        /*[TOKEN_LBRACKET] = */ "[",
        /*[TOKEN_RBRACKET] = */ "]",
        /*[TOKEN_LBRACE] = */ "{",
        /*[TOKEN_RBRACE] = */ "}",
        /*[TOKEN_LT] = */ "<",
        /*[TOKEN_LE] = */ "<=",
        /*[TOKEN_GT] = */ ">",
        /*[TOKEN_GE] = */ ">=",
        /*[TOKEN_EQ] = */ "==",
        /*[TOKEN_NE] = */ "!=",
        /*[TOKEN_LOGIC_NOT] = */ "!",
        /*[TOKEN_LOGIC_AND] = */ "&&",
        /*[TOKEN_LOGIC_OR] = */ "||",
        /*[TOKEN_BIT_NOT] = */ "~",
        //[TOKEN_ADDR_OF] = WIDE("&"),
        /*[TOKEN_BIT_AND] = */ "&",
        /*[TOKEN_BIT_OR] = */ "|",
        /*[TOKEN_BIT_XOR] = */ "^",
        /*[TOKEN_PLUS] = */ "+",
        /*[TOKEN_MINUS] = */ "-",
        //[TOKEN_DEREFERENCE] = WIDE("*"),
        /*[TOKEN_MUL] = */ "*",
        /*[TOKEN_DIV] = */ "/",
        /*[TOKEN_MOD] = */ "%",
        /*[TOKEN_LSHIFT] = */ "<<",
        /*[TOKEN_RSHIFT] = */ ">>",
        /*[TOKEN_RSHIFT_ZERO] = */ ">>>",
        /*[TOKEN_QUESTION] = */ "?",
        /*[TOKEN_COLON] = */ ":",
        /*[TOKEN_SEMICOLON] = */ ";",
        /*[TOKEN_COMMA] = */ ",",
        
        /*[TOKEN_PLUSPLUS] = */ "++",
        /*[TOKEN_MINUSMINUS] = */ "--",
        //[TOKEN_SIZEOF] = WIDE("sizeof"),
        //[TOKEN_DEFINED] = "defined",
        
        /*[TOKEN_ASSIGN_WITH_ADD] = */ "+=",
        /*[TOKEN_ASSIGN_WITH_SUB] = */ "-=",
        /*[TOKEN_ASSIGN_WITH_MUL] = */ "*=",
        /*[TOKEN_ASSIGN_WITH_DIV] = */ "/=",
        /*[TOKEN_ASSIGN_WITH_MOD] = */ "%=",
        /*[TOKEN_ASSIGN_WITH_BIT_AND] = */ "&=",
        /*[TOKEN_ASSIGN_WITH_BIT_OR] = */ "|=",
        /*[TOKEN_ASSIGN_WITH_BIT_XOR] = */ "^=",
        /*[TOKEN_ASSIGN_WITH_LSHIFT] = */ "<<=",
        /*[TOKEN_ASSIGN_WITH_RSHIFT] = */ ">>=",
        /*[TOKEN_ASSIGN_WITH_RSHIFT_ZERO] = */ ">>>=",
        /*[TOKEN_ASSIGN] = */ "=",
        
        /*[TOKEN_DOT] = */ ".",
        /*[TOKEN_ARROW] = */ "->",
        
        /*[TOKEN_LITERAL_CHARSEQ] = */ "CHAR",
        /*[TOKEN_LITERAL_STRING] = */ "STRING",
        /*[TOKEN_LITERAL_INT] = */ "NUM(d)",
        /*[TOKEN_LITERAL_LONG] = */ "NUM(l)",
        /*[TOKEN_LITERAL_UNSIGNED_INT] = */ "NUM(ud)",
        /*[TOKEN_LITERAL_UNSIGNED_LONG] = */ "NUM(ul)",
        /*[TOKEN_LITERAL_FLOAT] = */ "NUM(f)",
        /*[TOKEN_LITERAL_DOUBLE] = */ "NUM(lf)",
        /*[TOKEN_LITERAL_LONG_DOUBLE] = */ "NUM(llf)",
        
        /*[TOKEN_PLAINTEXT]*/ "PLAINTEXT",
        /*[TOKEN_PLAINTEXT]*/ "BLANKSPACE",
        /*[TOKEN_PLAINTEXT]*/ "BLANKTAB",
        
          "KW(private)",
          "KW(protected)",
          "KW(public)",
          "KW(abstract)",
          "KW(class)",
          "KW(extends)",
          "KW(final)",
          "KW(implements)",
          "KW(interface)",
          "KW(func)",
          "KW(new)",
          "KW(static)",
          "KW(break)",
          "KW(continue)",
          "KW(return)",
          "KW(do)",
          "KW(while)",
          "KW(if)",
          "KW(else)",
          "KW(for)",
          "KW(instanceof)",
          "KW(switch)",
          "KW(case)",
          "KW(default)",
          "KW(try)",
          "KW(catch)",
          "KW(throw)",
          "KW(throws)",
          "KW(finally)",
          "KW(import)",
          "KW(package)",
          "KW(boolean)",
          "KW(byte)",
          "KW(char)",
          "KW(double)",
          "KW(float)",
          "KW(int)",
          "KW(long)",
          "KW(short)",
          "KW(null)",
          "KW(true)",
          "KW(false)",
          "KW(super)",
          "KW(this)",
          "KW(asm)",
          "KW(void)",
          "RW(goto)",
          "RW(const)",
};

const std::map<std::string, TokenType> Keyword2TokenType = {
        std::make_pair("private", TOKEN_KW_PRIVATE),
        std::make_pair("protected", TOKEN_KW_PROTECTED),
        std::make_pair("public", TOKEN_KW_PUBLIC),
        std::make_pair("abstract", TOKEN_KW_ABSTRACT),
        std::make_pair("class", TOKEN_KW_CLASS),
        std::make_pair("extends", TOKEN_KW_EXTENDS),
        std::make_pair("final", TOKEN_KW_FINAL),
        std::make_pair("implements", TOKEN_KW_IMPLEMENTS),
        std::make_pair("interface", TOKEN_KW_INTERFACE),
        std::make_pair("func", TOKEN_KW_FUNC),
        std::make_pair("new", TOKEN_KW_NEW),
        std::make_pair("static", TOKEN_KW_STATIC),
        std::make_pair("break", TOKEN_KW_BREAK),
        std::make_pair("continue", TOKEN_KW_CONTINUE),
        std::make_pair("return", TOKEN_KW_RETURN),
        std::make_pair("do", TOKEN_KW_DO),
        std::make_pair("while", TOKEN_KW_WHILE),
        std::make_pair("if", TOKEN_KW_IF),
        std::make_pair("else", TOKEN_KW_ELSE),
        std::make_pair("for", TOKEN_KW_FOR),
        std::make_pair("instanceof", TOKEN_KW_INSTANCEOF),
        std::make_pair("switch", TOKEN_KW_SWITCH),
        std::make_pair("case", TOKEN_KW_CASE),
        std::make_pair("default", TOKEN_KW_DEFAULT),
        std::make_pair("try", TOKEN_KW_TRY),
        std::make_pair("catch", TOKEN_KW_CATCH),
        std::make_pair("throw", TOKEN_KW_THROW),
        std::make_pair("throws", TOKEN_KW_THROWS),
        std::make_pair("finally", TOKEN_KW_FINALLY),
        std::make_pair("import", TOKEN_KW_IMPORT),
        std::make_pair("package", TOKEN_KW_PACKAGE),
        std::make_pair("boolean", TOKEN_KW_BOOLEAN),
        std::make_pair("byte", TOKEN_KW_BYTE),
        std::make_pair("char", TOKEN_KW_CHAR),
        std::make_pair("double", TOKEN_KW_DOUBLE),
        std::make_pair("float", TOKEN_KW_FLOAT),
        std::make_pair("int", TOKEN_KW_INT),
        std::make_pair("long", TOKEN_KW_LONG),
        std::make_pair("short", TOKEN_KW_SHORT),
        std::make_pair("null", TOKEN_KW_NULL),
        std::make_pair("true", TOKEN_KW_TRUE),
        std::make_pair("false", TOKEN_KW_FALSE),
        std::make_pair("super", TOKEN_KW_SUPER),
        std::make_pair("this", TOKEN_KW_THIS),
        std::make_pair("asm", TOKEN_KW_ASM),
        std::make_pair("void", TOKEN_KW_VOID),
        std::make_pair("goto", TOKEN_RW_GOTO),
        std::make_pair("const", TOKEN_RW_CONST)};

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

Token & TokenScanner::realNextToken() {
    StateIndicator<TokenScannerState> si(TS_INITIAL); // 状态指示对象
    
    // 一些辅助局部变量
    UtfString utfStrBuff; // 字符串缓冲区
    bool exponent_sign = false;
    long exponent = 0; // 指数缓冲区
    
    TokenValue ptv_temp{};
    long lineno = 0, colno = 0;
    
    // 状态机逻辑代码
    while (true) {
        // 先抽取一个字符
        UtfCharInfo uci = _cld.nextChar();
        switch (si()) {
        case TS_INITIAL:
            // 记录单词起始的位置
            lineno = uci._lineno + _begin_lineno;
            colno = uci._colno + _begin_colno;
            
            // 如果扫描器工作在纯文本模式，则接收纯文本, 直到 EOF
            if (isReadPlainText()) {
                while (! uci.isEof()) {
                    utfStrBuff.push(uci());
                    uci = _cld.nextChar();
                }
                _token_seq.emplace_back(TOKEN_PLAINTEXT, lineno, colno,
                        utfStrBuff.toString());
                return _token_seq.back();
            }
            
            // 如果扫描器工作在纯文本（字符阻断）模式，则接收纯文本, 直到阻断字符或者
            // EOF 该方式返回结果不吸收阻断字符
            if (isReadPlainTextUntil()) {
                // 如果该字符不是 EOF，也不是阻断字符中的任何一个
                std::unordered_set<char>::iterator ite = _until_char_list.end();
                while (! uci.isEof() && uci().isAsciChar() &&
                        (ite = _until_char_list.find(uci().getAsciChar())) ==
                                _until_char_list.end()) {
                    utfStrBuff.push(uci());
                    uci = _cld.nextChar();
                }
                
                if (ite !=
                        _until_char_list
                                .end()) // 这说明中止是由于读取到阻断字符，那么，不吸收阻断字符
                    _cld.rollback();
                
                _token_seq.emplace_back(TOKEN_PLAINTEXT, lineno, colno,
                        utfStrBuff.toString());
                return _token_seq.back();
            }
            
            if (uci.isblank() || uci == '\r') {
                // 如果扫描器工作在普通读取+空白字符模式，那么遇到空白字符也需返回
                if (isReadNormalWithBlank()) {
                    if (uci == '\t') { // 制表符空白
                        _token_seq.emplace_back(TOKEN_BLANKTAB, lineno, colno);
                        return _token_seq.back();
                    } else if (uci == ' ') { // 空格空白
                        _token_seq.emplace_back(TOKEN_BLANKSPACE, lineno, colno);
                        return _token_seq.back();
                    }
                    // 包括 '\t' 和 ' ' 情况全部覆盖
                }
                // 普通读取模式则忽略空白
                si.stateKeep(); // 接收到空白字符，状态保持
                continue;
            } else if (uci == '#') {
                si.stateTransfer(TS_ACCEPTING_HASH); // 接收到'#'，状态转移
            } else if (uci.isalpha() || uci == '_' || uci.isUtfCharSeq()) {
                // 遭遇字母或者 UTF-8 多字节字符则吸收，状态转移
                utfStrBuff.push(uci());
                si.stateTransfer(TS_ACCEPTING_ID);
            } else if (uci.isdigit()) {
                _cld.rollback(); // 先回退字符，后转换状态
                si.stateTransfer(TS_ACCEPTING_LITERAL_NUMBER);
            } else if (uci == '\"') {
                si.stateTransfer(TS_ACCEPTING_LITERAL_STRING);
            } else if (uci == '\'') {
                si.stateTransfer(TS_ACCEPTING_LITERAL_CHAR);
            } else if (uci == '!') {
                si.stateTransfer(TS_ACCEPTING_LOGIC_NOT);
            } else if (uci == '~') {
                _token_seq.emplace_back(TOKEN_BIT_NOT, lineno, colno);
                return _token_seq.back();
            } else if (uci == '@') {
                _token_seq.emplace_back(TOKEN_AT, lineno, colno);
                return _token_seq.back();
            } else if (uci == '?') {
                _token_seq.emplace_back(TOKEN_QUESTION, lineno, colno);
                return _token_seq.back();
            } else if (uci == ':') {
                _token_seq.emplace_back(TOKEN_COLON, lineno, colno);
                return _token_seq.back();
            } else if (uci == ';') {
                _token_seq.emplace_back(TOKEN_SEMICOLON, lineno, colno);
                return _token_seq.back();
            } else if (uci == ',') {
                _token_seq.emplace_back(TOKEN_COMMA, lineno, colno);
                return _token_seq.back();
            } else if (uci == '(') {
                _token_seq.emplace_back(TOKEN_LP, lineno, colno);
                return _token_seq.back();
            } else if (uci == ')') {
                _token_seq.emplace_back(TOKEN_RP, lineno, colno);
                return _token_seq.back();
            } else if (uci == '{') {
                _token_seq.emplace_back(TOKEN_LBRACE, lineno, colno);
                return _token_seq.back();
            } else if (uci == '}') {
                _token_seq.emplace_back(TOKEN_RBRACE, lineno, colno);
                return _token_seq.back();
            } else if (uci == '[') {
                _token_seq.emplace_back(TOKEN_LBRACKET, lineno, colno);
                return _token_seq.back();
            } else if (uci == ']') {
                _token_seq.emplace_back(TOKEN_RBRACKET, lineno, colno);
                return _token_seq.back();
            } else if (uci == '.') { // 暂且进入识别以 . 开头的浮点序列的状态
                utfStrBuff += "0.";
                si.stateTransfer(TS_ACCEPTING_DECIMAL_PART_BEGIN_WITH_DOT);
            } else if (uci == '+') {
                si.stateTransfer(TS_ACCEPTING_PLUS);
            } else if (uci == '-') {
                si.stateTransfer(TS_ACCEPTING_MINUS);
            } else if (uci == '*') {
                si.stateTransfer(TS_ACCEPTING_MUL);
            } else if (uci == '/') {
                si.stateTransfer(TS_ACCEPTING_DIV);
            } else if (uci == '%') {
                si.stateTransfer(TS_ACCEPTING_MOD);
            } else if (uci == '=') {
                si.stateTransfer(TS_ACCEPTING_ASSIGN);
            } else if (uci == '>') {
                si.stateTransfer(TS_ACCEPTING_GT);
            } else if (uci == '<') {
                si.stateTransfer(TS_ACCEPTING_LT);
            } else if (uci == '&') {
                si.stateTransfer(TS_ACCEPTING_BIT_AND);
            } else if (uci == '|') {
                si.stateTransfer(TS_ACCEPTING_BIT_OR);
            } else if (uci == '^') {
                si.stateTransfer(TS_ACCEPTING_XOR);
            } else if (uci.isEof()) {
                goto empty_ret;
            } else {                         // 非法字符
                _ed.POST_PT_E(E_INVALID_CHAR); // 产生一个错误, 并且忽略该字符
            }
            break;
        
        case TS_ACCEPTING_LITERAL_NUMBER:
            // 此处有前瞻判断, 后续有状态转移，故无需处理一些特殊情形
            if (uci == '0') { // '0'
                ptv_temp.u._max_integer = 0;
                si.stateTransfer(TS_ACCEPTING_BEGIN_WITH_ZERO);
            } else if (uci.isdigit()) { // '1' ~ '9'
                ptv_temp.u._max_integer = ptv_temp.u._max_integer * 10 + uci.digit2int();
                si.stateTransfer(TS_ACCEPTING_BEGIN_WITH_NO_ZERO);
            }
            break;
        
        case TS_ACCEPTING_DECIMAL_PART_BEGIN_WITH_DOT: // 以 . 开头的浮点数序列
            if (uci.isdigit()) { // 暂且先用字符串缓冲将小数部分保存起来，以后再做到浮点数的转换
                utfStrBuff.push(uci());
                // 当前状态如果接受了一个数字字符，就与以数字开头的浮点序列之间没有什么不同之处
                si.stateTransfer(TS_ACCEPTING_DECIMAL_PART_BEGIN_WITH_DIGIT);
            } else if (uci.isEof()) { // 认为是 .
                _token_seq.emplace_back(TOKEN_DOT, lineno, colno);
                return _token_seq.back();
            } else { // 认为是 .
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_DOT, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_DECIMAL_PART_BEGIN_WITH_DIGIT: // 以数字开头的浮点序列
            if (uci.isdigit()) { // 暂且先用字符串缓冲将小数部分保存起来，以后再做到浮点数的转换
                utfStrBuff.push(uci());
                si.stateKeep();
            } else if (uci == 'e' || uci == 'E') { // 处理指数
                si.stateTransfer(TS_ACCEPTING_EXP_PART);
            } else if (uci == 'l' || uci == 'L') { // 认定为 long double 字面量, 返回
                __INTERNAL_TYPE_LONG_DOUBLE temp;
                sscanf(utfStrBuff.toString().c_str(), "%Lf", &temp);
                ptv_temp.u._llf = ptv_temp.u._max_integer + temp; // 整数与小数部分拼接
                _token_seq.emplace_back(TOKEN_LITERAL_LONG_DOUBLE, lineno, colno,
                        ptv_temp);
                return _token_seq.back();
            } else if (uci == 'f' || uci == 'F') { // 认定为 float 字面量, 返回
                __INTERNAL_TYPE_FLOAT temp;
                sscanf(utfStrBuff.toString().c_str(), "%f", &temp);
                ptv_temp.u._lf = ptv_temp.u._max_integer + temp; // 整数与小数部分拼接
                _token_seq.emplace_back(TOKEN_LITERAL_FLOAT, lineno, colno, ptv_temp);
                return _token_seq.back();
            } else if (uci.isEof()) { // 类似 12345. 的形式，认定为 double 字面量
                __INTERNAL_TYPE_DOUBLE temp;
                sscanf(utfStrBuff.toString().c_str(), "%lf", &temp);
                ptv_temp.u._lf = ptv_temp.u._max_integer + temp; // 整数与小数部分拼接
                _token_seq.emplace_back(TOKEN_LITERAL_DOUBLE, lineno, colno, ptv_temp);
                return _token_seq.back();
            } else {
                _cld.rollback();
                // 作为 double 字面量构造单词对象并返回
                __INTERNAL_TYPE_DOUBLE temp;
                sscanf(utfStrBuff.toString().c_str(), "%lf", &temp);
                ptv_temp.u._lf = ptv_temp.u._max_integer + temp; // 整数与小数部分拼接
                _token_seq.emplace_back(TOKEN_LITERAL_DOUBLE, lineno, colno, ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_EXP_PART:
            if (uci == '-') { // E 后面接着 - 号
                exponent_sign = true;
                si.stateTransfer(TS_ACCEPTING_EXP_WITH_SIGN);
            } else if (uci == '+') { // E 后面接着 + 号
                exponent_sign = false;
                si.stateTransfer(TS_ACCEPTING_EXP_WITH_SIGN);
            } else if (uci.isdigit()) { // E 后面直接接着数字
                exponent_sign = false;
                exponent = uci.digit2int();
                si.stateTransfer(TS_ACCEPTING_EXP_DEFAULT);
            } else if (uci.isEof()) { // 到达字符流末尾, 指数部分必须包含数字
                _ed.POST_PT_E(E_INVALID_EXP);
            } else {
                // 错误, 指数部分必须包含数字
                _cld.rollback();
                _ed.POST_PT_E(E_INVALID_EXP); // 产生一个错误
                // TODO 暂且认定为不带指数的小数形式, 作为 double
                // 字面量构造单词对象并返回
                __INTERNAL_TYPE_DOUBLE temp;
                sscanf(utfStrBuff.toString().c_str(), "%lf", &temp);
                ptv_temp.u._lf = ptv_temp.u._max_integer + temp; // 整数与小数部分拼接
                _token_seq.emplace_back(TOKEN_LITERAL_DOUBLE, lineno, colno, ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_EXP_WITH_SIGN:
            if (uci.isdigit()) {
                exponent = exponent * 10 + uci.digit2int();
                si.stateTransfer(TS_ACCEPTING_EXP_DEFAULT);
            } else if (uci.isEof()) {
                _ed.POST_PT_E(E_INVALID_EXP);
            } else {
                // 不能只有单个加号/减号, 否则算错误
                _cld.rollback();
                _ed.POST_PT_E(E_INVALID_EXP); // 产生一个错误
                // TODO 暂且认定为不带指数的小数形式, 作为 double
                // 字面量构造单词对象并返回
                __INTERNAL_TYPE_DOUBLE temp;
                sscanf(utfStrBuff.toString().c_str(), "%lf", &temp);
                ptv_temp.u._lf = ptv_temp.u._max_integer + temp; // 整数与小数部分拼接
                _token_seq.emplace_back(TOKEN_LITERAL_DOUBLE, lineno, colno, ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_EXP_DEFAULT:
            if (uci.isdigit()) {
                exponent = exponent * 10 + uci.digit2int();
                si.stateKeep();
            } else if (uci == 'l' ||
                    uci == 'L') { // 认定为带指数的 long double 字面量, 返回
                __INTERNAL_TYPE_LONG_DOUBLE temp;
                sscanf(utfStrBuff.toString().c_str(), "%Lf", &temp);
                temp += ptv_temp.u._max_integer; // 整数与小数部分拼接
                ptv_temp.u._llf = (exponent_sign) ? temp / long(std::pow(10, exponent))
                                                : temp * long(std::pow(10, exponent));
                _token_seq.emplace_back(TOKEN_LITERAL_LONG_DOUBLE, lineno, colno,
                        ptv_temp);
                return _token_seq.back();
            } else if (uci == 'f' ||
                    uci == 'F') { // 认定为带指数的 float 字面量, 返回
                __INTERNAL_TYPE_DOUBLE temp; // 暂且取 double, 后来转换为 float
                sscanf(utfStrBuff.toString().c_str(), "%lf", &temp);
                temp += ptv_temp.u._max_integer; // 整数与小数部分拼接
                ptv_temp.u._lf = (exponent_sign) ? temp / long(std::pow(10, exponent))
                                               : temp * long(std::pow(10, exponent));
                _token_seq.emplace_back(TOKEN_LITERAL_FLOAT, lineno, colno, ptv_temp);
                return _token_seq.back();
            } else if (uci.isEof()) { // 认定为带指数的 double 字面量, 返回
                __INTERNAL_TYPE_DOUBLE temp;
                sscanf(utfStrBuff.toString().c_str(), "%lf", &temp);
                temp += ptv_temp.u._max_integer; // 整数与小数部分拼接
                ptv_temp.u._lf = (exponent_sign) ? temp / long(std::pow(10, exponent))
                                               : temp * long(std::pow(10, exponent));
                _token_seq.emplace_back(TOKEN_LITERAL_DOUBLE, lineno, colno, ptv_temp);
                return _token_seq.back();
            } else { // 认定为带指数的 double 字面量, 字符回退并返回
                _cld.rollback(); // 字符回退
                __INTERNAL_TYPE_DOUBLE temp;
                sscanf(utfStrBuff.toString().c_str(), "%lf", &temp);
                temp += ptv_temp.u._max_integer; // 整数与小数部分拼接
                ptv_temp.u._lf = (exponent_sign) ? temp / long(std::pow(10, exponent))
                                               : temp * long(std::pow(10, exponent));
                _token_seq.emplace_back(TOKEN_LITERAL_DOUBLE, lineno, colno, ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_BEGIN_WITH_NO_ZERO:
            if (uci.isdigit()) { // '0' ~ '9'
                ptv_temp.u._max_integer = ptv_temp.u._max_integer * 10 + uci.digit2int();
                si.stateKeep(); // 持续读十进制序列
            } else if (uci == '.') {
                utfStrBuff += "0.";
                si.stateTransfer(TS_ACCEPTING_DECIMAL_PART_BEGIN_WITH_DIGIT);
            } else if (uci == 'E' || uci == 'e') {
                // 小数部分定为 0.0
                utfStrBuff = "0.0";
                si.stateTransfer(TS_ACCEPTING_EXP_PART);
            } else if (uci == 'U' || uci == 'u') {
                si.stateTransfer(TS_ACCEPTING_SUFFIX_U);
            } else if (uci == 'L' || uci == 'l') {
                si.stateTransfer(TS_ACCEPTING_SUFFIX_L);
            } else if (uci.isEof()) {
                // 构造对象并返回 (int)
                _token_seq.emplace_back(TOKEN_LITERAL_INT, lineno, colno, ptv_temp);
                return _token_seq.back();
            } else {
                _cld.rollback();
                // 构造对象并返回 (int)
                _token_seq.emplace_back(TOKEN_LITERAL_INT, lineno, colno, ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_BEGIN_WITH_ZERO:
            if (uci == 'x' || uci == 'X') {
                si.stateTransfer(TS_ACCEPTED_HEX_SEQ_HEADER);
            } else if (uci.isodigit()) { // '0' ~ '7'
                // 暂且先用10为基数来存放序列，因为可能序列是以0开头的浮点数
                // 即使不是以0开头的浮点数字面量，而是以0开头的八进制序列
                // 暂且这样存放，在以后也可以方便做转换
                ptv_temp.u._max_integer = ptv_temp.u._max_integer * 10 + uci.odigit2int();
                si.stateTransfer(TS_ACCEPTING_OCT_SEQ);
            } else if (uci.isdigit()) { // '8' ~ '9'
                ptv_temp.u._max_integer = ptv_temp.u._max_integer * 10 + uci.digit2int();
                si.stateTransfer(TS_ACCEPTING_DECIMAL_BEGIN_WITH_ZERO);
            } else if (uci == '.') {
                utfStrBuff += "0."; // bug fixed: 0. 开头会出问题
                si.stateTransfer(TS_ACCEPTING_DECIMAL_PART_BEGIN_WITH_DIGIT);
            } else if (uci == 'E' || uci == 'e') {
                // 小数部分定为 0.0
                utfStrBuff = "0.0";
                si.stateTransfer(TS_ACCEPTING_EXP_PART);
            } else if (uci.isEof()) { // 处理漏网之鱼
                _token_seq.emplace_back(TOKEN_LITERAL_INT, lineno, colno, ptv_temp);
                return _token_seq.back();
            } else { // 确定这是字面量0
                _cld.rollback();
                // 构造对象并返回
                _token_seq.emplace_back(TOKEN_LITERAL_INT, lineno, colno, ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_OCT_SEQ:
            if (uci.isodigit()) { // '0' ~ '7'
                ptv_temp.u._max_integer = ptv_temp.u._max_integer * 10 + uci.odigit2int();
                si.stateKeep();           // 持续读八进制序列
            } else if (uci.isdigit()) { // '8' ~ '9'
                ptv_temp.u._max_integer = ptv_temp.u._max_integer * 10 + uci.digit2int();
                si.stateTransfer(TS_ACCEPTING_DECIMAL_BEGIN_WITH_ZERO);
            } else if (uci == 'U' || uci == 'u') {
                // 到达后缀，说明这是八进制序列，进行序列转换，10转8
                ptv_temp.u._max_integer =
                        SeqConverter::DigitSeq::dec2oct(ptv_temp.u._max_integer);
                si.stateTransfer(TS_ACCEPTING_SUFFIX_U);
            } else if (uci == 'L' || uci == 'l') {
                // 到达后缀，说明这是八进制序列，进行序列转换，10转8
                ptv_temp.u._max_integer =
                        SeqConverter::DigitSeq::dec2oct(ptv_temp.u._max_integer);
                si.stateTransfer(TS_ACCEPTING_SUFFIX_L);
            } else if (uci == 'E' || uci == 'e') {
                // 小数部分定为 0.0
                utfStrBuff = "0.0";
                si.stateTransfer(TS_ACCEPTING_EXP_PART);
            } else if (uci.isEof()) {
                // 确定这是八进制序列, 进行序列转换, 10转8, 构造对象并返回
                ptv_temp.u._max_integer =
                        SeqConverter::DigitSeq::dec2oct(ptv_temp.u._max_integer);
                _token_seq.emplace_back(TOKEN_LITERAL_INT, lineno, colno, ptv_temp);
                return _token_seq.back();
            } else {
                _cld.rollback();
                // 确定这是八进制序列，进行序列转换，10转8，构造对象并返回
                ptv_temp.u._max_integer =
                        SeqConverter::DigitSeq::dec2oct(ptv_temp.u._max_integer);
                _token_seq.emplace_back(TOKEN_LITERAL_INT, lineno, colno, ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_DECIMAL_BEGIN_WITH_ZERO:
            if (uci == '.') {
                utfStrBuff += "0.";
                si.stateTransfer(TS_ACCEPTING_DECIMAL_PART_BEGIN_WITH_DIGIT);
            } else if (uci.isdigit()) {
                // 持续读数字字符，除非碰到数字字符或者
                // '.'(完成状态转移)，若碰到其他情况，则产生错误
                ptv_temp.u._max_integer = ptv_temp.u._max_integer * 10 + uci.digit2int();
                si.stateKeep();
            } else if (uci == 'E' || uci == 'e') {
                // 小数部分定为 0.0
                utfStrBuff = "0.0";
                si.stateTransfer(TS_ACCEPTING_EXP_PART);
            } else if (uci.isEof()) {
                _ed.POST_PT_E(E_INVALID_OCTDIGIT_SEQ);
            } else { // 非法的八进制数字序列, 读出其他字符
                _cld.rollback();
                _ed.POST_PT_E(E_INVALID_OCTDIGIT_SEQ);
                // TODO 产生一个错误,
                // 并且暂且认定这是八进制序列，进行序列转换，10转8，构造对象并返回
                ptv_temp.u._max_integer =
                        SeqConverter::DigitSeq::dec2oct(ptv_temp.u._max_integer);
                _token_seq.emplace_back(TOKEN_LITERAL_INT, lineno, colno, ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTED_HEX_SEQ_HEADER:
            if (uci.isxdigit()) {
                ptv_temp.u._max_integer = uci.xdigit2int();
                si.stateTransfer(TS_ACCEPTING_HEX_SEQ);
            } else if (uci.isEof()) { // 若遇到 EOF，则説明此十六进制序列是非法的
                _ed.POST_PT_E(E_INCMP_HEXDIGIT_SEQ);
            } else { // 若遇到非数字字符的其他情形，则説明此十六进制序列是非法的
                _cld.rollback();
                _ed.POST_PT_E(E_INCMP_HEXDIGIT_SEQ);
                // TODO 产生一个错误, 暂且认定为 0
                _token_seq.emplace_back(TOKEN_LITERAL_INT, lineno, colno, ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_HEX_SEQ:
            if (uci.isxdigit()) { // 纍计十六进制序列为整数数据
                ptv_temp.u._max_integer = ptv_temp.u._max_integer * 0x10 + uci.xdigit2int();
                si.stateKeep(); // 持续读十六进制数字
            } else if (uci == 'U' || uci == 'u') {
                si.stateTransfer(TS_ACCEPTING_SUFFIX_U);
            } else if (uci == 'L' || uci == 'l') {
                si.stateTransfer(TS_ACCEPTING_SUFFIX_L);
            } else if (uci.isEof()) { // 遇到 EOF 则构造 Token 对象
                // 构造单词对象并返回 (int)
                _token_seq.emplace_back(TOKEN_LITERAL_INT, lineno, colno, ptv_temp);
                return _token_seq.back();
            } else {
                _cld.rollback();
                // 构造单词对象并返回 (int)
                _token_seq.emplace_back(TOKEN_LITERAL_INT, lineno, colno, ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_SUFFIX_U:
            if (uci == 'L' || uci == 'l') { // 状态转移去适配 unsigned long
                si.stateTransfer(TS_ACCEPTING_SUFFIX_UL);
            } else if (uci.isEof()) { // 遇到 EOF 则构造 Token 对象 unsigned int
                // 构造单词对象并返回 (unsigned int)
                _token_seq.emplace_back(TOKEN_LITERAL_UNSIGNED_INT, lineno, colno,
                        ptv_temp);
                return _token_seq.back();
            } else {
                _cld.rollback();
                // 构造单词对象并返回 (unsigned int)
                _token_seq.emplace_back(TOKEN_LITERAL_UNSIGNED_INT, lineno, colno,
                        ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_SUFFIX_L:
            if (uci == 'U' || uci == 'u') { // 状态转移去适配 unsigned long
                si.stateTransfer(TS_ACCEPTING_SUFFIX_UL);
            } else if (uci.isEof()) { // 遇到 EOF 则构造 Token 对象 long
                // 构造单词对象并返回 (long)
                _token_seq.emplace_back(TOKEN_LITERAL_LONG, lineno, colno, ptv_temp);
                return _token_seq.back();
            } else {
                _cld.rollback();
                // 构造单词对象并返回 (long)
                _token_seq.emplace_back(TOKEN_LITERAL_LONG, lineno, colno, ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_SUFFIX_UL:
            if (uci.isEof()) { // 遇到 EOF 则构造 Token 对象 unsigned long
                // 构造单词对象并返回 (unsigned long)
                _token_seq.emplace_back(TOKEN_LITERAL_UNSIGNED_LONG, lineno, colno,
                        ptv_temp);
                return _token_seq.back();
            } else { // 遇到其他字符则回退
                _cld.rollback();
                // 构造单词对象并返回 (unsigned long)
                _token_seq.emplace_back(TOKEN_LITERAL_UNSIGNED_LONG, lineno, colno,
                        ptv_temp);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_LITERAL_STRING:
            if (uci == '\\') {
                utfStrBuff.push(uci());
                si.stateTransfer(TS_ACCEPTING_LITERAL_STRING_ESCAPE_QUOT);
            } else if (uci == '\"') { // 字符串结束
                std::string s = utfStrBuff.toString();
                if (s.size() >= __CHARSEQ_MAXSIZE) {
                    _ed.POST_PT_E(E_STRING_TOO_LONG);
                }
                _token_seq.emplace_back(TOKEN_LITERAL_STRING, lineno, colno,
                        SeqConverter::toNative(s));
                return _token_seq.back();
            } else if (uci.isEof()) {
                // 字符串尚未闭合文件就到末尾了
                _ed.POST_PT_E(E_INCMP_STRING_SEQ);
            } else {
                utfStrBuff.push(uci());
            }
            break;
        
        case TS_ACCEPTING_LITERAL_STRING_ESCAPE_QUOT:
            if (uci.isEof()) {
                // 字符串尚未闭合, 文件就到末尾了，且这种情况下，转义序列也没闭合
                _ed.POST_PT_E(E_INCMP_STRING_SEQ);
            } else {
                utfStrBuff.push(uci());
                si.stateRollback();
            }
            break;
        
        case TS_ACCEPTING_LITERAL_CHAR:
            if (uci == '\\') {
                utfStrBuff.push(uci());
                si.stateTransfer(TS_ACCEPTING_LITERAL_CHAR_ESCAPE_QUOT);
            } else if (uci == '\'') { // 字符结束
                std::string s = utfStrBuff.toString();
                if (s.size() >= __CHARSEQ_MAXSIZE) {
                    _ed.POST_PT_E(E_CHARSEQ_TOO_LONG);
                }
                _token_seq.emplace_back(TOKEN_LITERAL_CHARSEQ, lineno, colno,
                        SeqConverter::toNative(s));
                return _token_seq.back();
            } else if (uci.isEof()) {
                // 字符尚未闭合, 文件就到末尾了
                _ed.POST_PT_E(E_INCMP_CHAR_SEQ);
            } else {
                utfStrBuff.push(uci());
            }
            break;
        
        case TS_ACCEPTING_LITERAL_CHAR_ESCAPE_QUOT:
            if (uci.isEof()) {
                // 字符尚未闭合, 文件就到末尾了，且这种情况下，转义序列也没闭合
                _ed.POST_PT_E(E_INCMP_CHAR_SEQ);
            } else {
                utfStrBuff.push(uci());
                si.stateRollback();
            }
            break;
        
        case TS_ACCEPTING_LOGIC_NOT:
            if (uci == '=') { // 不等于
                _token_seq.emplace_back(TOKEN_NE, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定为逻辑非
                _token_seq.emplace_back(TOKEN_LOGIC_NOT, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是逻辑非，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_LOGIC_NOT, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_XOR:
            if (uci == '=') { // 按位异或赋值
                _token_seq.emplace_back(TOKEN_ASSIGN_WITH_BIT_XOR, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定为按位异或
                _token_seq.emplace_back(TOKEN_BIT_XOR, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是按位异或，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_BIT_XOR, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_BIT_AND:
            if (uci == '=') { // 按位与赋值
                _token_seq.emplace_back(TOKEN_ASSIGN_WITH_BIT_AND, lineno, colno);
                return _token_seq.back();
            } else if (uci == '&') { // 逻辑与
                _token_seq.emplace_back(TOKEN_LOGIC_AND, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定爲按位与
                _token_seq.emplace_back(TOKEN_BIT_AND, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是按位与，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_BIT_AND, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_BIT_OR:
            if (uci == '=') { // 按位或赋值
                _token_seq.emplace_back(TOKEN_ASSIGN_WITH_BIT_OR, lineno, colno);
                return _token_seq.back();
            } else if (uci == '|') { // 逻辑或
                _token_seq.emplace_back(TOKEN_LOGIC_OR, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定为位或
                _token_seq.emplace_back(TOKEN_BIT_OR, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是按位或，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_BIT_OR, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_ASSIGN:
            if (uci == '=') { // 等于
                _token_seq.emplace_back(TOKEN_EQ, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定为赋值
                _token_seq.emplace_back(TOKEN_ASSIGN, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是赋值，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_ASSIGN, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_GT:
            if (uci == '=') { // 大于等于
                _token_seq.emplace_back(TOKEN_GE, lineno, colno);
                return _token_seq.back();
            } else if (uci == '>') { // 可能是右移位或者右移位赋值
                si.stateTransfer(TS_ACCEPTING_RSHIFT);
            } else if (uci.isEof()) { // 到达文件末尾，认定为大于号
                _token_seq.emplace_back(TOKEN_GT, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是大于号，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_GT, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_RSHIFT:
            if (uci == '=') { // 右移位赋值
                _token_seq.emplace_back(TOKEN_ASSIGN_WITH_RSHIFT, lineno, colno);
                return _token_seq.back();
            } else if (uci == '>') { // 符号位补零的右移位或者符号位补零的右移位赋值
                si.stateTransfer(TS_ACCEPTING_RSHIFT_ZERO);
            } else if (uci.isEof()) { // 到达文件末尾，认定为右移位
                _token_seq.emplace_back(TOKEN_RSHIFT, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是右移位，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_RSHIFT, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_RSHIFT_ZERO:
            if (uci == '=') { // 符号位补零的右移位赋值
                _token_seq.emplace_back(TOKEN_ASSIGN_WITH_RSHIFT_ZERO, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定为符号位补零的右移位
                _token_seq.emplace_back(TOKEN_RSHIFT_ZERO, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是符号位补零的右移位，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_RSHIFT_ZERO, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_LT:
            if (uci == '=') { // 小于等于
                _token_seq.emplace_back(TOKEN_LE, lineno, colno);
                return _token_seq.back();
            } else if (uci == '<') { // 可能是左移位或者左移位赋值
                si.stateTransfer(TS_ACCEPTING_LSHIFT);
            } else if (uci.isEof()) { // 到达文件末尾，认定为小于号
                _token_seq.emplace_back(TOKEN_LT, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是小于号，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_LT, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_LSHIFT:
            if (uci == '=') { // 左移位赋值
                _token_seq.emplace_back(TOKEN_ASSIGN_WITH_LSHIFT, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定为左移位
                _token_seq.emplace_back(TOKEN_LSHIFT, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是左移位，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_LSHIFT, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_PLUS:
            if (uci == '+') { // 自增
                _token_seq.emplace_back(TOKEN_PLUSPLUS, lineno, colno);
                return _token_seq.back();
            } else if (uci == '=') { // 加赋值
                _token_seq.emplace_back(TOKEN_ASSIGN_WITH_ADD, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定为加号
                _token_seq.emplace_back(TOKEN_PLUS, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是单个加号，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_PLUS, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_MINUS:
            if (uci == '-') { // 自减
                _token_seq.emplace_back(TOKEN_MINUSMINUS, lineno, colno);
                return _token_seq.back();
            } else if (uci == '>') { // 间接成员
                _token_seq.emplace_back(TOKEN_ARROW, lineno, colno);
                return _token_seq.back();
            } else if (uci == '=') { // 减赋值
                _token_seq.emplace_back(TOKEN_ASSIGN_WITH_SUB, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定为减号
                _token_seq.emplace_back(TOKEN_MINUS, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是单个减号，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_MINUS, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_MUL:
            if (uci == '=') { // 乘赋值
                _token_seq.emplace_back(TOKEN_ASSIGN_WITH_MUL, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定为乘号
                _token_seq.emplace_back(TOKEN_MUL, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是单个乘号，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_MUL, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_DIV:
            if (uci == '=') { // 除赋值
                _token_seq.emplace_back(TOKEN_ASSIGN_WITH_DIV, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定为除号
                _token_seq.emplace_back(TOKEN_DIV, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是单个除号，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_DIV, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_MOD:
            if (uci == '=') { // 取模赋值
                _token_seq.emplace_back(TOKEN_ASSIGN_WITH_MOD, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定为取模
                _token_seq.emplace_back(TOKEN_MOD, lineno, colno);
                return _token_seq.back();
            } else { // 如果是其他字符，说明是取模，状态和指示器回退
                _cld.rollback();
                _token_seq.emplace_back(TOKEN_MOD, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_HASH:
            if (uci == '#') {
                // 接收到'#'，说明该单词是双井号，构造单词、保存并返回
                _token_seq.emplace_back(TOKEN_DOUBLE_HASH, lineno, colno);
                return _token_seq.back();
            } else if (uci.isEof()) { // 到达文件末尾，认定为单井号
                _token_seq.emplace_back(TOKEN_SINGLE_HASH, lineno, colno);
                return _token_seq.back();
            } else {
                // 接收到其他字符，说明该单词已读取完，是单井号，状态指示器回退
                _cld.rollback();
                
                // 构造单词、保存并返回
                _token_seq.emplace_back(TOKEN_SINGLE_HASH, lineno, colno);
                return _token_seq.back();
            }
            break;
        
        case TS_ACCEPTING_ID:
            if (uci.isalnum() || uci == '_') {
                // 遭遇字母则吸收，状态保持
                utfStrBuff.push(uci());
                si.stateKeep();
            } else if (uci.isEof()) { // 到达文件末尾，认定为 ID 或者 keyword /
                // reserved word
                // 构造单词、保存并返回
                std::string strBuff = utfStrBuff.toString();
                auto it = Keyword2TokenType.find(strBuff);
                if (it == Keyword2TokenType.end()) {
                    if (strBuff.size() >= __CHARSEQ_MAXSIZE) { // ID 过长，报错
                        _ed.POST_PT_E(E_ID_TOO_LONG);
                    }
                    _token_seq.emplace_back(TOKEN_ID, lineno, colno, strBuff);
                } else
                    _token_seq.emplace_back(it->second, lineno, colno);
                return _token_seq.back();
                
            } else {
                // 接收到其他字符，说明该单词已读取完，认定为 ID 或者 keyword / reserved
                // word
                _cld.rollback();
                // 构造单词、保存并返回
                std::string strBuff = utfStrBuff.toString();
                auto it = Keyword2TokenType.find(strBuff);
                if (it == Keyword2TokenType.end()) {
                    if (strBuff.size() >= __CHARSEQ_MAXSIZE) { // ID 过长，报错
                        _ed.POST_PT_E(E_ID_TOO_LONG);
                    }
                    _token_seq.emplace_back(TOKEN_ID, lineno, colno, strBuff);
                } else
                    _token_seq.emplace_back(it->second, lineno, colno);
                return _token_seq.back();
            }
            break;
        }
    } // 状态机循环结束
    
    empty_ret:
    // 空单词，代表扫描器在本次推进中没有扫描到有效的单词
    _token_seq.emplace_back(TOKEN_TERMINATOR, lineno, colno);
    return _token_seq.back();
}

const Token & TokenScanner::nextToken() {
    // 如果指示器到了末尾，则向输入流获取新单词，
    // 否则取指示器 _curr_pos 指向的一个单词
    long len = _token_seq.size();
    if (this->_curr_pos > len || this->_curr_pos < 0)
        throw std::out_of_range("指示器所处的当前位置非法");
    if (this->_curr_pos == len) {
        this->realNextToken();
    }
    const Token & pt = _token_seq[this->_curr_pos ++];
    return pt;
}

const Token & TokenScanner::currentToken() const {
    long len = _token_seq.size();
    if (this->_curr_pos > len || this->_curr_pos < 1)
        throw std::out_of_range("指示器所处的当前位置非法");
    return _token_seq[this->_curr_pos - 1];
}

const Token & TokenScanner::previousToken() {
    long len = _token_seq.size();
    if (this->_curr_pos > len || this->_curr_pos < 2)
        throw std::out_of_range("指示器所处的当前位置非法");
    return _token_seq[-- this->_curr_pos - 1];
}

void TokenScanner::forward() {
    long len = _token_seq.size();
    if (this->_curr_pos > len)
        throw std::out_of_range("指示器已至行扫描器末尾位置");
    this->_curr_pos ++;
}

void TokenScanner::rollback() {
    if (this->_curr_pos < 2)
        throw std::out_of_range("指示器已至行扫描器起始位置");
    this->_curr_pos --;
}
