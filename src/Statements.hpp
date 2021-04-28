#ifndef __STATEMENTS_HEADER__
#define __STATEMENTS_HEADER__

#include "ExprDefinition.hpp"
#include "LocalVarDefinition.hpp"

#define ONLY_STMT_FACTORY(CLASSNAME) \
private: \
friend struct IStmt; \
CLASSNAME(const CLASSNAME &) = delete; \
CLASSNAME & operator=(const CLASSNAME &) = delete; \
explicit CLASSNAME

enum class StmtType : int {
    PureExpr,
    Define,
    For,
    While,
    DoWhile,
    If,
    Switch,
    Break,
    Continue,
    Return,
    Stmts,
    InlineASM,
    Empty
};

struct IStmt;
using StmtPtr = std::shared_ptr<IStmt>;
using StmtsPtr = StmtPtr; // 因为语句组也是一种广义的语句

struct IfCondEntry {
    ExprPtr _pCondExpr = nullptr;
    StmtsPtr _pStmts = nullptr;
    
    explicit IfCondEntry(const ExprPtr & pCondExpr, const StmtsPtr & pStmts) {
        _pCondExpr = pCondExpr;
        _pStmts = pStmts;
    }
    //IfCondEntry(const IfCondEntry &) = delete;
    //IfCondEntry & operator=(const IfCondEntry &) = delete;
};

struct SwitchIndexEntry {
    enum class MarkType : int {
        Case = 0, Default
    } _type = MarkType::Case;
    ExprPtr _pCondExpr = nullptr; // 若成立则以 _stmtIdx 位置的语句作为跳转点
    size_t _stmtIdx = 0;

public:
    MarkType getMarkType() const { return _type; }
    explicit SwitchIndexEntry(const ExprPtr & pSrcExpr,
            const ExprPtr & pObjExpr,
            size_t stmtIdx) {
        
        _pCondExpr = Expr::newExpr(Expr::Operator::OPT_EQ,
                {pSrcExpr, pObjExpr}, pSrcExpr->_lineno, pSrcExpr->_colno);
        // 此处破例使用
        _stmtIdx = stmtIdx;
        _type = MarkType::Case;
    }
    
    explicit SwitchIndexEntry(size_t stmtIdx) {
        _stmtIdx = stmtIdx;
        _type = MarkType::Default;
    }
    
    //SwitchIndexEntry(const SwitchIndexEntry &) = delete;
    //SwitchIndexEntry & operator=(const SwitchIndexEntry &) = delete;
};

struct IStmt {
    ExprPtr _pDummyExpr = nullptr;
    virtual StmtType getStmtType() const = 0;
    
    void setDummyExpr(const ExprPtr & pExpr) { _pDummyExpr = pExpr; }
    const ExprPtr & getDummyExpr() { return _pDummyExpr; }
    
    static StmtPtr newPureExprStmt(const ExprPtr & pExpr);
    static StmtPtr newDefineStmt(const std::vector<LocalVar> & defScopeObjs);
    static StmtPtr newForStmt(const StmtPtr & pDefOrPureExprStmt,
            const StmtPtr & pCondStmt,
            const StmtPtr & pIncreStmt,
            const StmtsPtr & pStmts);
    static StmtPtr newWhileStmt(const ExprPtr & pCondPtr, const StmtsPtr & pStmts);
    static StmtPtr newDoWhileStmt(const StmtsPtr & pStmts, const ExprPtr & pCondPtr);
    static StmtPtr newIfStmt(const std::vector<IfCondEntry> & condEntries);
    static StmtPtr newSwitchStmt(const std::vector<SwitchIndexEntry> & idxEntries, const std::vector<StmtPtr> & stmts);
    static StmtPtr newBreakStmt(const ExprPtr & pDummyExpr);
    static StmtPtr newContinueStmt(const ExprPtr & pDummyPtr);
    static StmtPtr newReturnStmt(const ExprPtr & pRetExpr, const ExprPtr & pDummyPtr);
    static StmtPtr newReturnStmt(const ExprPtr & pDummyPtr);
    static StmtsPtr newStmts(const std::vector<StmtPtr> & stmts);
    static StmtPtr newInlineASM(const std::string & asmcodes, const ExprPtr & pDummyExpr);
    static StmtPtr newEmptyStmt();
};

struct Stmts : public IStmt {
    //ScopeType _scopeType = ScopeType::ScopeNormal; // 作用域类型
    std::vector<StmtPtr> _stmts;

public:
    StmtType getStmtType() const override {
        return StmtType::Stmts;
    }
    
    const std::vector<StmtPtr> & getStmtList() const {
        return _stmts;
    }

    ONLY_STMT_FACTORY(Stmts)(const std::vector<StmtPtr> stmts) {
        _stmts = stmts;
    }
};

struct InlineASM : public IStmt {
    //ScopeType _scopeType = ScopeType::ScopeNormal; // 作用域类型
    std::string _asmcodes;

public:
    StmtType getStmtType() const override {
        return StmtType::InlineASM;
    }
    
    const std::string & getASMCodes() const {
        return _asmcodes;
    }

    ONLY_STMT_FACTORY(InlineASM)(const std::string & asmcodes, const ExprPtr & pDummyExpr) {
        _asmcodes = asmcodes;
        setDummyExpr(pDummyExpr);
    }
};

struct EmptyStmt : public IStmt {
public:
    StmtType getStmtType() const override {
        return StmtType::Empty;
    }

    ONLY_STMT_FACTORY(EmptyStmt)() = default;
};

struct PureExprStmt : public IStmt {
    ExprPtr _pExpr = nullptr;

public:
    StmtType getStmtType() const override {
        return StmtType::PureExpr;
    }

    ONLY_STMT_FACTORY(PureExprStmt)(const ExprPtr & pExpr) {
        _pExpr = pExpr;
    }
};

struct DefineStmt : public IStmt {
    std::vector<LocalVar> _defScopeObjs;

public:
    StmtType getStmtType() const override {
        return StmtType::Define;
    }
    
    const std::vector<LocalVar> & getDefScopeObjs() const {
        return _defScopeObjs;
    }

    ONLY_STMT_FACTORY(DefineStmt)(const std::vector<LocalVar> & defScopeObjs) {
        _defScopeObjs = defScopeObjs;
    }
};

struct ForStmt : public IStmt {
    StmtPtr _defStmtOrInitExpr = nullptr; // 可能是定义语句或者纯表达式
    StmtPtr _pCondStmt = nullptr;
    StmtPtr _pIncreStmt = nullptr;
    StmtsPtr _pStmts = nullptr;
    
    StmtType getStmtType() const override {
        return StmtType::For;
    }
    
    const StmtPtr & getFirstStmt() const {
        return _defStmtOrInitExpr;
    }
    
    const StmtPtr & getSecondStmt() const {
        return _pCondStmt;
    }
    
    const StmtPtr & getThirdStmt() const {
        return _pIncreStmt;
    }

    ONLY_STMT_FACTORY(ForStmt)(const StmtPtr & defStmtOrInitExpr,
            const StmtPtr & pCondStmt,
            const StmtPtr & pIncreStmt,
            const StmtsPtr & pStmts) {
        _defStmtOrInitExpr = defStmtOrInitExpr;
        _pCondStmt = pCondStmt;
        _pIncreStmt = pIncreStmt;
        _pStmts = pStmts;
    }
};

struct WhileStmt : public IStmt {
    ExprPtr _pCondExpr = nullptr;
    StmtsPtr _pStmts = nullptr;
    
    StmtType getStmtType() const override {
        return StmtType::While;
    }

    ONLY_STMT_FACTORY(WhileStmt)(const ExprPtr & pCondPtr, const StmtsPtr & pStmts) {
        _pCondExpr = pCondPtr;
        _pStmts = pStmts;
    }
};

struct DoWhileStmt : public IStmt {
    StmtsPtr _pStmts = nullptr;
    ExprPtr _pCondExpr = nullptr;
    
    StmtType getStmtType() const override {
        return StmtType::DoWhile;
    }

    ONLY_STMT_FACTORY(DoWhileStmt)(const StmtsPtr & pStmts, const ExprPtr & pCondPtr) {
        _pCondExpr = pCondPtr;
        _pStmts = pStmts;
    }
};

struct IfStmt : public IStmt {
    std::vector<IfCondEntry> _condEntries;
    
    StmtType getStmtType() const override {
        return StmtType::If;
    }

    ONLY_STMT_FACTORY(IfStmt)(const std::vector<IfCondEntry> & condEntries) {
        _condEntries = condEntries;
    }
};

struct SwitchStmt : public IStmt {
    std::vector<SwitchIndexEntry> _idxEntries;
    StmtsPtr _pStmts = nullptr;
    
    StmtType getStmtType() const override {
        return StmtType::Switch;
    }

    ONLY_STMT_FACTORY(SwitchStmt)(const std::vector<SwitchIndexEntry> & idxEntries,
            const std::vector<StmtPtr> & stmts) {
        _idxEntries = idxEntries;
        _pStmts = IStmt::newStmts(stmts);
    }
};

struct BreakStmt : public IStmt {
    StmtType getStmtType() const override {
        return StmtType::Break;
    }

    ONLY_STMT_FACTORY(BreakStmt)(const ExprPtr & pDummyExpr) {
        setDummyExpr(pDummyExpr);
    }
};

struct ContinueStmt : public IStmt {
    StmtType getStmtType() const override {
        return StmtType::Continue;
    }

    ONLY_STMT_FACTORY(ContinueStmt)(const ExprPtr & pDummyExpr) {
        setDummyExpr(pDummyExpr);
}

};

struct ReturnStmt : public IStmt {
    ExprPtr _pRetExpr = nullptr;
    StmtType getStmtType() const override {
        return StmtType::Return;
    }

    ONLY_STMT_FACTORY(ReturnStmt)(const ExprPtr & pRetExpr, const ExprPtr & pDummyExpr) {
        _pRetExpr = pRetExpr;
        setDummyExpr(pDummyExpr);
    }
};

#endif
