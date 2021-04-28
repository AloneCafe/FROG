#ifndef __ST_PARSER_HEADER__
#define __ST_PARSER_HEADER__

#include <cassert>
#include <functional>
#include <list>
#include <vector>

#include "Structure.hpp"
#include "SyntaxTreeBasicParser.hpp"
#include "GlobalFuncMap.hpp"

class AstParser {
private:
    std::string _filename;
    AstErrorDriver<ErrorDriver> _aed;
    AstBasicParser _basicPsr;

public:
    AstParser(const std::string & filename) :
            _filename(filename),
            _aed(_filename),
            _basicPsr(_aed) { }
    
    AstParser() :
            _filename("<stdin>"),
            _aed(_filename),
            _basicPsr(_aed) { }

private:
    /*
     * GlobalVarDefineStmt -> id GlobalVarDefineStmt1
     *             | id = GlobalVarInitExprList GlobalVarDefineStmt1
     *             | id = ExprAssign GloablVarDefineStmt1
     *
     * GlobalVarDefineStmt1 -> , id GlobalVarDefineStmt1
     *             | , id = GlobalVarInitExprList GlobalVarDefineStmt1
     *             | , id = ExprAssign GlobalVarDefineStmt1
     *             | e
     */
    void parseGlobalVarDefineStmt1(
            TokenIter & it,
            std::vector<LocalVar> & defGlobalVars,
            const GlobalVar & gModelVar);

    void parseGlobalVarDefineStmt(
            TokenIter & it,
            std::vector<GlobalVar> & defGlobalVars,
            const GlobalVar & gModelVar);

    /*
     * DefGloablVarStmt ->
     *  | DefType GlobalVarDefineStmt
     */
    StmtPtr buildDefGlobalVarStmt(TokenIter & it, FileStructurePtr pFileStu);



    void parseFuncRetType(TokenIter & it, Function & mf);
    
    void parseFuncNameAndArgList(TokenIter & it, Function & mf);
    
    /*
     * FuncContent -> { Stmts }
     */
    void parseFuncContent(TokenIter & it, FunctionStructure & funcStu);
    
    /*
     * 直接吞掉整个定义式, 包含结尾右花括号在内
     */
    void parseFunc(TokenIter & it, FileStructurePtr pFileStu);
    
    /*
    template <int P>
    void ignoreUtilPuncOrEnd(TokenIter & it) {
        while (it->isEnd() && ! it->isPunc<P>()) { ++ it; }
    }*/
    
    FileStructurePtr parseTop(TokenIter it);

public:
    FileStructurePtr parse(const std::vector<Token> & tks);
    
    const std::vector<Error> & getErrList();
    
    bool hasErr() const;
};

#endif
