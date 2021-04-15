#ifndef __AST_BUILD_HEADER__
#define __AST_BUILD_HEADER__

#include <cassert>
#include <functional>
#include <list>
#include <vector>

#include "Structure.hpp"
#include "Statements.hpp"
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
    void parseGlobalVarDefineStmt1(TokenIter & it, std::vector<LocalVar> & defGlobalVars,
        const GlobalVar & gModelVar) {

        if (it->isPunc<','>()) {
            ++it;
            parseGlobalVarDefineStmt(it, defGlobalVars, gModelVar); // 因为有相同的公共因子序列，故直接转调用

        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);

        } else {
            // 认为语句闭合，直接返回上层
            return;
        }
    }

    void parseGlobalVarDefineStmt(TokenIter & it, std::vector<GlobalVar> & defGlobalVars,
        const GlobalVar & gModelVar) {

        GlobalVar so;
        GlobalVar::copySameModifierAndType(gModelVar, so);

        if (it->isId()) {
            AST_REC_LOCATION1
            so.setName(LocatedUtfString::make(it->getId(), AST_ARG_LOCATION1));
            
            ++it;
            if (it->isPunc<'='>()) {
                ++it;
                // 此处判断是表达式列表还是单个表达式 (赋值优先级以及赋值以上的)
                if (it->isPunc<'{'>()) {
                    // 表达式列表
                    std::vector<GlobalVar::InitEntityPtr> entities;
                    _basicPsr.parseScopeObjInitEntityList(it, entities); // 注意列表中的元素也可能是一个列表
                    so.setInitList(entities);

                } else {
                    // 单个表达式
                    ExprPtr pExpr = _basicPsr._buildExprAssign(it);
                    so.setInitExpr(pExpr);
                }
                // 加入定义列表
                defGlobalVars.push_back(so);
                parseGlobalVarDefineStmt1(it, defGlobalVars, gModelVar);

            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);

            } else {
                // 此处说明定义了一个列表，先保存变量，然后直接进入递归解析
                defGlobalVars.push_back(so);
                parseGlobalVarDefineStmt1(it, defGlobalVars, gModelVar);
            }

        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);

        } else {
            AST_E(E_ILLEGAL_TOKEN); // 报错，不递进，交由上层处理
        }
    }

    /*
     * DefGloablVarStmt ->
     *  | DefType GlobalVarDefineStmt
     */
    StmtPtr buildDefGlobalVarStmt(TokenIter & it, FileStructurePtr pFileStu) {
        std::vector<GlobalVar> defScopeObjs;
        GlobalVar gModelVar;

        // 此处需要区别对待表达式和变量定义式，
        // 在需要解析变量定义式时使用 parseGlobalVarDefineStmt 函数

        VarType objType;
        _basicPsr.parseDefType(it, objType);
        gModelVar.setType(objType);

        if (it->isId()) {
            // 认定为变量定义语句，解析语句
            parseGlobalVarDefineStmt(it, defScopeObjs, gModelVar);

            for (const auto & e : defScopeObjs)
                pFileStu->addGlobalVar(e);

            // 完成，生成变量定义语句，存储语句
            return IStmt::newDefineStmt(defScopeObjs);

        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);

        } else {
            AST_E(E_LOSE_SEMICOLON); // 报错，此处可能缺少分号
            ++it;
        }

        return nullptr;
    }



    void parseFuncRetType(TokenIter & it, Function & mf) {
        VarType type;
        _basicPsr.parseDefType(it, type);
        mf.setRetType(type);
    }
    
    void parseFuncNameAndArgList(TokenIter & it, Function & mf) {
        if (it->isId()) {
            AST_REC_LOCATION1
            mf.setName(LocatedUtfString::make(it->getId(), AST_ARG_LOCATION1)); // 取得成员变量名字
            ++ it;
            if (it->isPunc<'('>()) {
                // 解析形式参数列表
                FormalArgList fal;
                _basicPsr._parseFormalArgList(it, fal);
                // 此处已经吸收了右括号
                
                mf.setFormalArgList(fal);
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_EXPECT_LP); // 报错，不递进，交由上层处理
            }
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            
        } else { // 不是 id, 暂且报错, 非法 token
            AST_E(E_ILLEGAL_TOKEN); // 报错, 不递进, 交由上层处理
        }
    }
    
    /*
     * FuncContent -> { Stmts }
     */
    void parseFuncContent(TokenIter & it, FunctionStructure & funcStu) {
        if (it->isPunc<'{'>()) {
            ++ it;
            
            StmtPtr pStmts = _basicPsr.buildStmts(it);
            funcStu.setStmtsPtr(pStmts);
            
            if (it->isPunc<'}'>()) {
                ++ it;
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_LOSE_RBRACE); // 报错，此处可能缺少右花括号
            }
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            
        } else { // 不是左花括号，暂且报错，非法 token
            AST_E(E_ILLEGAL_TOKEN); // 报错，不递进，交由上层处理
        }
    }
    
    /*
     * 直接吞掉整个定义式, 包含结尾右花括号在内
     */
    void parseFunc(TokenIter & it, FileStructurePtr pFileStu) {
        Function mf;
        // 处理成员函数
        parseFuncRetType(it, mf);                    // 解析返回值类型定义
        parseFuncNameAndArgList(it, mf);             // 解析成员函数名字与参数列表
        parseFuncContent(it, mf.getFuncStuRef());    // 解析成员函数定义的内容
        pFileStu->addFunc(mf);

        // 注册到全局函数
        if (!GlobalFuncMap::putFunc(mf.getName().toString(), mf._fal, mf)) {
            AST_E(E_FUNC_REDEFINED);
        }
    }
    
    template <int P>
    void ignoreUtilPuncOrEnd(TokenIter & it) {
        while (it->isEnd() && ! it->isPunc<P>()) { ++ it; }
    }
    
    FileStructurePtr parseTop(TokenIter it) {
        FileStructurePtr pFileStu = FileStructure::newFileStu(_filename);
        
        // 需要循环
        while (true) {
            if (it->isKwFunc()) {
                ++it;
                parseFunc(it, pFileStu); // 解析函数定义
                
            } else if (it->isEnd()) {
                break;

            } else {
                buildDefGlobalVarStmt(it, pFileStu); // 解析全局变量定义

                if (it->isPunc<';'>()) {
                    ++it;
                } else {
                    AST_E(E_LOSE_SEMICOLON);
                }
            }
        }
        return pFileStu;
    }

public:
    FileStructurePtr parse(const std::vector<Token> & tks) {
        FileStructurePtr pFileStu = parseTop(tks.cbegin());
        return pFileStu;
    }
    
    const std::vector<Error> & getErrList() { return _aed.getErrList(); }
    bool hasErr() const { return _aed.hasErr(); }
};

#endif
