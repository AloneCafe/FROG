#include "Statements.hpp"

// 实现工厂函数
StmtPtr IStmt::newPureExprStmt(const ExprPtr & pExpr) {
    StmtPtr pStmt(new PureExprStmt(pExpr));
    return pStmt;
}

StmtPtr IStmt::newDefineStmt(const std::vector<LocalVar> & defScopeObjs) {
    StmtPtr pStmt(new DefineStmt(defScopeObjs));
    return pStmt;
}

StmtPtr IStmt::newForStmt(const StmtPtr & pDefOrPureExprStmt,
        const StmtPtr & pCondStmt,
        const StmtPtr & pIncreStmt,
        const StmtsPtr & pStmts) {
    StmtPtr pStmt(new ForStmt(pDefOrPureExprStmt, pCondStmt, pIncreStmt, pStmts));
    return pStmt;
}

StmtPtr IStmt::newWhileStmt(const ExprPtr & pCondPtr, const StmtsPtr & pStmts) {
    StmtPtr pStmt(new WhileStmt(pCondPtr, pStmts));
    return pStmt;
}

StmtPtr IStmt::newDoWhileStmt(const StmtsPtr & pStmts, const ExprPtr & pCondPtr) {
    StmtPtr pStmt(new DoWhileStmt(pStmts, pCondPtr));
    return pStmt;
}

StmtPtr IStmt::newIfStmt(const std::vector<IfCondEntry> & condEntries) {
    StmtPtr pStmt(new IfStmt(condEntries));
    return pStmt;
}

StmtPtr IStmt::newSwitchStmt(const std::vector<SwitchIndexEntry> & idxEntries, const std::vector<StmtPtr> & stmts) {
    StmtPtr pStmt(new SwitchStmt(idxEntries, stmts));
    return pStmt;
}

StmtPtr IStmt::newBreakStmt(const ExprPtr & pDummyExpr) {
    StmtPtr pStmt(new BreakStmt(pDummyExpr));
    return pStmt;
}

StmtPtr IStmt::newContinueStmt(const ExprPtr & pDummyPtr) {
    StmtPtr pStmt(new ContinueStmt(pDummyPtr));
    return pStmt;
}

StmtPtr IStmt::newReturnStmt(const ExprPtr & pRetExpr, const ExprPtr & pDummyExpr) {
    StmtPtr pStmt(new ReturnStmt(pRetExpr, pDummyExpr));
    return pStmt;
}

StmtPtr IStmt::newReturnStmt(const ExprPtr & pDummyExpr) {
    StmtPtr pStmt(new ReturnStmt(nullptr, pDummyExpr));
    return pStmt;
}

StmtsPtr IStmt::newStmts(const std::vector<StmtPtr> & stmts) {
    StmtsPtr pStmts(new Stmts(stmts));
    return pStmts;
}

StmtPtr IStmt::newEmptyStmt() {
    StmtPtr pStmt(new EmptyStmt());
    return pStmt;
}

StmtPtr IStmt::newInlineASM(const std::string & asmcodes, const ExprPtr & pDummyExpr) {
    StmtPtr pStmt(new InlineASM(asmcodes, pDummyExpr));
    return pStmt;
}
