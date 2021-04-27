#ifndef __EXPR_DEF_HEADER__
#define __EXPR_DEF_HEADER__

#include <memory>
#include <algorithm>
#include <cassert>
#include <vector>
#include <sstream>

#include "ParserConfigure.hpp"
#include "SyntaxTreeErrorDriver.hpp"
#include "StructureBase.hpp"
#include "TokenDefinition.hpp"

struct Expr {
public:
    enum class Operator : int {
        OPT_LOGIC_NOT = 0,        // ! E0
        OPT_BIT_NOT,              // ~ E0
        OPT_FRONT_PLUSPLUS,        // ++ E0
        OPT_FRONT_MINUSMINUS,      // -- E0
        OPT_BACK_PLUSPLUS,        // E0 ++
        OPT_BACK_MINUSMINUS,      // E0 --
        
        OPT_DOT,                  // E0 . E1
        OPT_INDEX,                // E0 [ E1 ]
        
        OPT_LT,                   // E0 < E1
        OPT_LE,                   // E0 <= E1
        OPT_GT,                   // E0 > E1
        OPT_GE,                   // E0 >= E1
        OPT_EQ,                   // E0 == E1
        OPT_NE,                   // E0 != E1
        
        OPT_LOGIC_AND,            // E0 && E1
        OPT_LOGIC_OR,             // E0 || E1
        OPT_BIT_AND,              // E0 & E1
        OPT_BIT_OR,                  // E0 | E1
        OPT_BIT_XOR,              // E0 ^ E1
        
        OPT_PLUS,                 // E0 + E1
        OPT_MINUS,                // E0 - E1
        OPT_MUL,                  // E0 * E1
        OPT_DIV,                  // E0 / E1
        OPT_MOD,                  // E0 % E1
        OPT_LSHIFT,               // E0 << E1
        OPT_RSHIFT,               // E0 >> E1
        OPT_RSHIFT_ZERO,          // E0 >>> E1
        
        OPT_ASSIGN_WITH_ADD,         // E0 += E1
        OPT_ASSIGN_WITH_SUB,         // E0 -= E1
        OPT_ASSIGN_WITH_MUL,         // E0 *= E1
        OPT_ASSIGN_WITH_DIV,         // E0 /= E1
        OPT_ASSIGN_WITH_MOD,         // E0 %= E1
        OPT_ASSIGN_WITH_BIT_AND,     // E0 &= E1
        OPT_ASSIGN_WITH_BIT_OR,      // E0 |= E1
        OPT_ASSIGN_WITH_BIT_XOR,     // E0 ^= E1
        OPT_ASSIGN_WITH_LSHIFT,      // E0 <<= E1
        OPT_ASSIGN_WITH_RSHIFT,      // E0 >>= E1
        OPT_ASSIGN_WITH_RSHIFT_ZERO, // E0 >>>= E1
        OPT_ASSIGN,                  // E0 = E1
        
        OPT_COND,    // E0 ? E1 : E2
        OPT_CALL,    // E0 ( E1, E2, ..., En )
        OPT_COMMA,   // E0, E1
        OPT_NEGATIVE, // - E0
        OPT_POSITIVE, // + E0
        OPT_NEW,
        OPT_TYPE_CONVERT, // @ E0 ( E1 )
        
        DELIMITER_OPT_LEAF, // 分隔定义，用于防御性 debug 断言
        
        LEAF_ID,
        LEAF_INT_LITERAL,
        LEAF_LONG_LITERAL,
        LEAF_FLOAT_LITERAL,
        LEAF_DOUBLE_LITERAL,
        LEAF_LONG_DOUBLE_LITERAL,
        LEAF_CHAR_LITERAL,
        LEAF_STRING_LITERAL,
        
        LEAF_NULL_LITERAL,
        LEAF_TRUE_LITERAL,
        LEAF_FALSE_LITERAL,
        LEAF_THIS,
        LEAF_SUPER,
        LEAF_TYPE, // 用于类型转换，或者将来某种用途
    };
    
    using ExprPtr = std::shared_ptr<Expr>;
    using LeafScalar = TokenValue;
    using LeafType = VarType;
    
    long _lineno;
    long _colno;
    
    long lineno() const;
    long colno() const;

private:
    static size_t UNLIMITED;
    static const size_t __opt2exprCnt[];

public:
    size_t getExprCnt() const;

private:
    Operator _op;
    size_t _nSubCnt = 0;
    std::vector<ExprPtr> _subExprs; // 非叶子节点时
    LeafScalar _leafScalar;         // 叶子节点时
    LeafType _leafType;
    
    Expr() = default;

public:
    Operator getOp() const;
    
    const ExprPtr & getSubExprPtr(size_t i) const;
    
    const LeafScalar & getLeafScalar() const;
    
    const LeafType & getLeafType() const;
    
    const ExprPtr & operator[](size_t i) const;
    
    static ExprPtr newCallExpr(ExprPtr callerExpr, const std::vector<ExprPtr> & argExprList, long lineno, long colno);

    static ExprPtr newCommaExpr(const std::vector<ExprPtr> & commaExprList, long lineno, long colno);
    
    static ExprPtr newExpr(const Operator & op, const std::initializer_list<ExprPtr> & exprs, long lineno, long colno);
    
    static ExprPtr newLeafScalar(const Operator & op, const TokenValue & tv, long lineno, long colno);
    
    static ExprPtr newLeafType(const VarType & type, long lineno, long colno);
    
    static ExprPtr newLeafPure(const Operator & op, long lineno, long colno);
    
    bool isLeaf() const;
    
    bool isOpt() const;
    
    Expr(const Expr &) = delete;
    
    Expr & operator=(const Expr &) = delete;
    
};

using ExprOp = typename Expr::Operator;
using ExprPtr = typename Expr::ExprPtr;

#endif
