#include <vector>


#include "ExprDefinition.hpp"

uint32_t Expr::UNLIMITED = 0;

const uint32_t Expr::__opt2exprCnt[] = {
        1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, UNLIMITED, UNLIMITED, 1, 1, 1, 2
};

long Expr::lineno() const {
    return _lineno;
}

long Expr::colno() const {
    return _colno;
}

uint32_t Expr::getExprCnt() const {
    return _nSubCnt;
}

Expr::Operator Expr::getOp() const {
    return _op;
}

const ExprPtr & Expr::getSubExprPtr(uint32_t i) const {
    return _subExprs[i];
}

const Expr::LeafScalar & Expr::getLeafScalar() const {
    return _leafScalar;
}

const Expr::LeafType & Expr::getLeafType() const {
    return _leafType;
}

const ExprPtr & Expr::operator[](uint32_t i) const {
    return _subExprs[i];
}

ExprPtr Expr::newCallExpr(ExprPtr callerExpr, const std::vector<ExprPtr> & argExprList, long lineno, long colno) {
    ExprPtr pExpr(new Expr());
    pExpr->_op = Operator::OPT_CALL;
    pExpr->_lineno = lineno;
    pExpr->_colno = colno;
    pExpr->_subExprs.push_back(callerExpr);
    std::for_each(argExprList.cbegin(), argExprList.cend(), [=](const ExprPtr & e) {
        pExpr->_subExprs.push_back(e);
    });
    pExpr->_nSubCnt = pExpr->_subExprs.size();
    return pExpr;
}

ExprPtr Expr::newCommaExpr(const std::vector<ExprPtr> & commaExprList, long lineno, long colno) {
    ExprPtr pExpr(new Expr());
    pExpr->_op = Operator::OPT_CALL;
    pExpr->_lineno = lineno;
    pExpr->_colno = colno;
    std::for_each(commaExprList.cbegin(), commaExprList.cend(), [=](const ExprPtr & e) {
        pExpr->_subExprs.push_back(e);
    });
    pExpr->_nSubCnt = pExpr->_subExprs.size();
    return pExpr;
}

ExprPtr Expr::newExpr(const Operator & op, const std::initializer_list<ExprPtr> & exprs, long lineno, long colno) {
    assert(op < Operator::DELIMITER_OPT_LEAF && "op 类型必须是非叶子节点");
    ExprPtr pExpr(new Expr());
    pExpr->_op = op;
    pExpr->_subExprs = exprs;
    pExpr->_nSubCnt = exprs.size();
    pExpr->_lineno = lineno;
    pExpr->_colno = colno;
    return pExpr;
}

ExprPtr Expr::newLeafScalar(const Operator & op, const TokenValue & tv, long lineno, long colno) {
    assert(op > Operator::DELIMITER_OPT_LEAF && "op 类型必须是叶子节点");
    ExprPtr pExpr(new Expr());
    pExpr->_op = op;
    pExpr->_nSubCnt = 1;
    pExpr->_leafScalar = tv;
    pExpr->_lineno = lineno;
    pExpr->_colno = colno;
    return pExpr;
}

ExprPtr Expr::newLeafType(const VarType & type, long lineno, long colno) {
    ExprPtr pExpr(new Expr());
    pExpr->_op = Operator::LEAF_TYPE;
    pExpr->_nSubCnt = 1;
    pExpr->_leafType = type;
    pExpr->_lineno = lineno;
    pExpr->_colno = colno;
    return pExpr;
}

ExprPtr Expr::newLeafPure(const Operator & op, long lineno, long colno) {
    assert(op > Operator::DELIMITER_OPT_LEAF && "op 类型必须是叶子节点");
    ExprPtr pExpr(new Expr());
    pExpr->_op = op;
    pExpr->_nSubCnt = 1;
    pExpr->_lineno = lineno;
    pExpr->_colno = colno;
    return pExpr;
}

bool Expr::isLeaf() const {
    return _op > Operator::DELIMITER_OPT_LEAF;
}

bool Expr::isOpt() const {
    return _op < Operator::DELIMITER_OPT_LEAF;
}