#ifndef __TOKEN_DEF_HEADER__
#define __TOKEN_DEF_HEADER__

#include <cstdint>
#include <string>

// 最长标识符字符长度限制

#define __CHARSEQ_MAXSIZE             128

// 数据类型约定
#define __INTERNAL_TYPE_CHAR              int8_t
#define __INTERNAL_TYPE_SHORT             int16_t
#define __INTERNAL_TYPE_INT               int32_t
#define __INTERNAL_TYPE_LONG              int64_t

#define __INTERNAL_TYPE_FLOAT             float
#define __INTERNAL_TYPE_DOUBLE            double
#define __INTERNAL_TYPE_LONG_DOUBLE       long double

#define __INTERNAL_TYPE_UNSIGNED_CHAR     uint8_t
#define __INTERNAL_TYPE_UNSIGNED_SHORT    uint16_t
#define __INTERNAL_TYPE_UNSIGNED_INT      uint32_t
#define __INTERNAL_TYPE_UNSIGNED_LONG     uint64_t

#define __INTERNAL_TYPE_VOID              void_t

enum TokenType {
    TOKEN_TERMINATOR,
    TOKEN_ID,                   // identifier
    TOKEN_AT,            // cstr       // '@'
    TOKEN_SINGLE_HASH,          // '#'
    TOKEN_DOUBLE_HASH,          // '##'
    
    TOKEN_LP,                   // '('
    TOKEN_RP,                   // ')'
    TOKEN_LBRACKET,             // '['
    TOKEN_RBRACKET,             // ']'
    TOKEN_LBRACE,               // '{'
    TOKEN_RBRACE,               // '}'
    TOKEN_LT,                   // '<'
    TOKEN_LE,                   // '<='
    TOKEN_GT,                   // '>'
    TOKEN_GE,                   // '>='
    TOKEN_EQ,                   // '=='
    TOKEN_NE,                   // '!='
    TOKEN_LOGIC_NOT,            // '!'
    TOKEN_LOGIC_AND,            // '&&'
    TOKEN_LOGIC_OR,             // '||'
    TOKEN_BIT_NOT,              // '~'
    TOKEN_ADDR_OF,              // '&'
    TOKEN_BIT_AND =             // 取地址 same as '&'
    TOKEN_ADDR_OF,
    TOKEN_BIT_OR,               // '|'
    TOKEN_BIT_XOR,              // '^'
    TOKEN_PLUS,                 // '+'
    TOKEN_MINUS,                // '-'
    TOKEN_DEREFERENCE,          // '*'
    TOKEN_MUL =
    TOKEN_DEREFERENCE,      // 解引用 same as '*'
    TOKEN_DIV,                  // '/'
    TOKEN_MOD,                  // '%'
    TOKEN_LSHIFT,               // '<<'
    TOKEN_RSHIFT,               // '>>'
    TOKEN_RSHIFT_ZERO,          // '>>>'
    TOKEN_QUESTION,             // '?'
    TOKEN_COLON,                // ':'
    TOKEN_SEMICOLON,            // ';'
    TOKEN_COMMA,                // ','
    
    TOKEN_PLUSPLUS,               // '++'
    TOKEN_MINUSMINUS,             // '--'
    //TOKEN_SIZEOF,               // 'sizeof' 取数据类型宽度
    //TOKEN_DEFINED,              // 'defined' 作用于宏，判定宏是否定义
    
    TOKEN_ASSIGN_WITH_ADD,         // '+='
    TOKEN_ASSIGN_WITH_SUB,         // '-='
    TOKEN_ASSIGN_WITH_MUL,         // '*='
    TOKEN_ASSIGN_WITH_DIV,         // '/='
    TOKEN_ASSIGN_WITH_MOD,         // '%='
    TOKEN_ASSIGN_WITH_BIT_AND,     // '&='
    TOKEN_ASSIGN_WITH_BIT_OR,      // '|='
    TOKEN_ASSIGN_WITH_BIT_XOR,     // '^='
    TOKEN_ASSIGN_WITH_LSHIFT,      // '<<='
    TOKEN_ASSIGN_WITH_RSHIFT,      // '>>='
    TOKEN_ASSIGN_WITH_RSHIFT_ZERO, // '>>>='
    TOKEN_ASSIGN,                  // '='
    
    TOKEN_DOT,                     // '.'
    TOKEN_ARROW,                   // '->'
    
    TOKEN_LITERAL_CHARSEQ, // cstr              // 字符字面值
    TOKEN_LITERAL_STRING,  // cstr              // 字符串字面值
    TOKEN_LITERAL_INT,                   // 32位int整数字面值
    TOKEN_LITERAL_LONG,                  // 64位long整数字面值
    TOKEN_LITERAL_UNSIGNED_INT,          // 32位无符号int整数字面值
    TOKEN_LITERAL_UNSIGNED_LONG,         // 64位无符号long整数字面值
    TOKEN_LITERAL_FLOAT,                 // 单精度浮点字面值(x86上32位)
    TOKEN_LITERAL_DOUBLE,                // 双精度浮点字面值(x86上64位)
    TOKEN_LITERAL_LONG_DOUBLE,           // 长双精度浮点字面值(x86上80位)
    
    TOKEN_PLAINTEXT,
    TOKEN_BLANKSPACE,
    TOKEN_BLANKTAB,
    
    TOKEN_KW_PRIVATE,
    TOKEN_KW_PROTECTED,
    TOKEN_KW_PUBLIC,
    TOKEN_KW_ABSTRACT,
    TOKEN_KW_CLASS,
    TOKEN_KW_EXTENDS,
    TOKEN_KW_FINAL,
    TOKEN_KW_IMPLEMENTS,
    TOKEN_KW_INTERFACE,
    TOKEN_KW_FUNC,
    TOKEN_KW_NEW,
    TOKEN_KW_STATIC,
    TOKEN_KW_BREAK,
    TOKEN_KW_CONTINUE,
    TOKEN_KW_RETURN,
    TOKEN_KW_DO,
    TOKEN_KW_WHILE,
    TOKEN_KW_IF,
    TOKEN_KW_ELSE,
    TOKEN_KW_FOR,
    TOKEN_KW_INSTANCEOF,
    TOKEN_KW_SWITCH,
    TOKEN_KW_CASE,
    TOKEN_KW_DEFAULT,
    TOKEN_KW_TRY,
    TOKEN_KW_CATCH,
    TOKEN_KW_THROW,
    TOKEN_KW_THROWS,
    TOKEN_KW_FINALLY,
    TOKEN_KW_IMPORT,
    TOKEN_KW_PACKAGE,
    TOKEN_KW_BOOLEAN,
    TOKEN_KW_BYTE,
    TOKEN_KW_CHAR,
    TOKEN_KW_DOUBLE,
    TOKEN_KW_FLOAT,
    TOKEN_KW_INT,
    TOKEN_KW_LONG,
    TOKEN_KW_SHORT,
    TOKEN_KW_NULL,
    TOKEN_KW_TRUE,
    TOKEN_KW_FALSE,
    TOKEN_KW_SUPER,
    TOKEN_KW_THIS,
    TOKEN_KW_ASM,
    TOKEN_KW_VOID,
    TOKEN_RW_GOTO,
    TOKEN_RW_CONST
};

struct TokenValue {
    std::string _str;
    union {
		__INTERNAL_TYPE_INT _d;
		__INTERNAL_TYPE_LONG _l;
		__INTERNAL_TYPE_CHAR _c;
		__INTERNAL_TYPE_SHORT _s;

		__INTERNAL_TYPE_LONG_DOUBLE _llf;
		__INTERNAL_TYPE_DOUBLE _lf;
		//__INTERNAL_TYPE_FLOAT _f;

		__INTERNAL_TYPE_UNSIGNED_CHAR _uc;
		__INTERNAL_TYPE_UNSIGNED_INT _ud;
		__INTERNAL_TYPE_UNSIGNED_LONG _ul;
		__INTERNAL_TYPE_UNSIGNED_SHORT _us;

		__INTERNAL_TYPE_UNSIGNED_LONG _max_integer;
		__INTERNAL_TYPE_LONG_DOUBLE _max_float;
    } u;
};

#endif
