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

struct Expression {
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
    using ExprPtr = std::shared_ptr<Expression>;
    using LeafScalar = TokenValue;
    using LeafType = VarType;
    
    long _lineno;
    long _colno;
    
    long lineno() const { return _lineno; }
    long colno() const { return _colno; }

private:
    static constexpr size_t UNLIMITED = 0;
    static constexpr size_t __opt2exprCnt[] = {
            1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            3, UNLIMITED, UNLIMITED, 1, 1, 1, 2
    };

public:
    size_t getExprCnt() const {
        return _nSubCnt;
    }

private:
    Operator _op;
    size_t _nSubCnt = 0;
    std::vector<ExprPtr> _subExprs; // 非叶子节点时
    LeafScalar _leafScalar;         // 叶子节点时
    LeafType _leafType;
    
    Expression() = default;

public:
    Operator getOp() const { return _op; }
    const ExprPtr & getSubExprPtr(size_t i) const { return _subExprs[i]; }
    const LeafScalar & getLeafScalar() const { return _leafScalar; }
    const LeafType & getLeafType() const { return _leafType; }
    const ExprPtr & operator[](size_t i) const { return _subExprs[i]; }
    
    static ExprPtr newCallExpr(ExprPtr callerExpr, const std::vector<ExprPtr> & argExprList, long lineno, long colno) {
        ExprPtr pExpr(new Expression());
        pExpr->_op = Operator::OPT_CALL;
        pExpr->_lineno = lineno;
        pExpr->_colno = colno;
        pExpr->_subExprs.push_back(callerExpr);
        std::for_each(argExprList.cbegin(), argExprList.cend(), [=](const auto & e) {
            pExpr->_subExprs.push_back(e);
        });
        pExpr->_nSubCnt = pExpr->_subExprs.size();
        return pExpr;
    }

    static ExprPtr newCommaExpr(const std::vector<ExprPtr> & commaExprList, long lineno, long colno) {
		ExprPtr pExpr(new Expression());
		pExpr->_op = Operator::OPT_CALL;
		pExpr->_lineno = lineno;
		pExpr->_colno = colno;
		std::for_each(commaExprList.cbegin(), commaExprList.cend(), [=](const auto & e) {
			pExpr->_subExprs.push_back(e);
			});
		pExpr->_nSubCnt = pExpr->_subExprs.size();
		return pExpr;
    }
    
    static ExprPtr newExpr(const Operator & op, const std::initializer_list<ExprPtr> & exprs, long lineno, long colno) {
        assert(op < Operator::DELIMITER_OPT_LEAF && "op 类型必须是非叶子节点");
        ExprPtr pExpr(new Expression());
        pExpr->_op = op;
        pExpr->_subExprs = exprs;
        pExpr->_nSubCnt = exprs.size();
        pExpr->_lineno = lineno;
        pExpr->_colno = colno;
        return pExpr;
    }
    
    static ExprPtr newLeafScalar(const Operator & op, const TokenValue & tv, long lineno, long colno) {
        assert(op > Operator::DELIMITER_OPT_LEAF && "op 类型必须是叶子节点");
        ExprPtr pExpr(new Expression());
        pExpr->_op = op;
        pExpr->_nSubCnt = 1;
        pExpr->_leafScalar = tv;
        pExpr->_lineno = lineno;
        pExpr->_colno = colno;
        return pExpr;
    }
    
    static ExprPtr newLeafType(const VarType & type, long lineno, long colno) {
        ExprPtr pExpr(new Expression());
        pExpr->_op = Operator::LEAF_TYPE;
        pExpr->_nSubCnt = 1;
        pExpr->_leafType = type;
        pExpr->_lineno = lineno;
        pExpr->_colno = colno;
        return pExpr;
    }
    
    static ExprPtr newLeafPure(const Operator & op, long lineno, long colno) {
        assert(op > Operator::DELIMITER_OPT_LEAF && "op 类型必须是叶子节点");
        ExprPtr pExpr(new Expression());
        pExpr->_op = op;
        pExpr->_nSubCnt = 1;
        pExpr->_lineno = lineno;
        pExpr->_colno = colno;
        return pExpr;
    }
    
    bool isLeaf() const { return _op > Operator::DELIMITER_OPT_LEAF; }
    bool isOpt() const { return _op < Operator::DELIMITER_OPT_LEAF; }
    
    Expression(const Expression &) = delete;
    Expression & operator=(const Expression &) = delete;
    
};

using ExprOp = typename Expression::Operator;
using ExprPtr = typename Expression::ExprPtr;

#endif
