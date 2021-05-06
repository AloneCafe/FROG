#ifndef __AST_EXPRESSION_HEADER__
#define __AST_EXPRESSION_HEADER__

#include <sstream>
#include <cassert>

#include "ParserConfigure.hpp"
#include "SyntaxTreeErrorDriver.hpp"
#include "ExprDefinition.hpp"
#include "Statements.hpp"

class AstParser;

class AstBasicParser {
    friend class AstParser;
private:
    AstErrorDriver<ErrorDriver> & _aed;
    
    explicit AstBasicParser(AstErrorDriver<ErrorDriver> & aed) :
        _aed(aed) { }
    
    void parseLowLvTypeList_(TokenIter & it,
            std::vector<LocatedUtfString> & refLowLvTypeList);
    
    void parseLowLvTypeList(TokenIter & it,
            std::vector<LocatedUtfString> & refLowLvTypeList);
    
    /*
     * LowLvType -> id
     *            | scalar-type
     */
    void parseLowLvType(TokenIter & it, LocatedUtfString & lowLvType);
    
    void parseNoNumIndexList(TokenIter & it, int & degree);
    
    /*
     * DefType -> LowLvType NoNumIndexList
     */
    void parseDefType(TokenIter & it, VarType & type);
    
    /*
     * FormalArgList -> DefType id FormalArgList1
     *              | e
     * FormalArgList1 -> , DefType id FormalArgList1
     *              | e
     */
    void _parseFormalArgList1(TokenIter & it, std::vector<FormalArg> & args);
    
    void _parseFormalArgList(TokenIter & it, std::vector<FormalArg> & args);
    
    /*
     * RealArgList -> ( ExprAssign RealArgList1 )
     *              | ( )
     * RealArgList1 -> , ExprAssign RealArgList1
     *              | e
     */
    void _parseRealArgList1(TokenIter & it, std::vector<ExprPtr> & args);
    
    void _parseRealArgList(TokenIter & it, std::vector<ExprPtr> & args);
    
    /*
     * ExprDotList -> id . ExprDotList
     *             | id
     */
    /*
    ExprPtr _buildExprDotList(TokenIter & it);
    */
    
    /*
     * ExprIndexList -> [ Expr ] ExprIndexList
     */
    ExprPtr _buildExprIndexList(TokenIter & it, ExprPtr pLeft);
    
    /*
     * ExprSuffix -> ( Expr )
     *             | @ DefType ( Expr )   // 强制类型转换
     *             | id ExprIndexList
     *             | id ( RealArgList ) ExprIndexList // 此处括号是隐含的
     *             | any-literal
     *             | true
     *             | false
     *             | sizeof ( Expr )
     * ///// 注意: 函数调用的参数表达式是由逗号分隔开的一组参数列表，
     * ///// 列表中每一项是比逗号运算符优先级更低 (即赋值及以下) 的表达式组成，
     * ///// 也就是以 " ( ExprAssign, ExprAssign, ... , ExprAssign ) " 的形式组成
     *
     * ///// ExprIndexList 表示 0~N 个下标运算符组合而成的链
     * /////
     *
     */
    ExprPtr _buildExprSuffix(TokenIter & it);
    
    /*
     * ExprUnary -> ExprSuffix ++
     *            | ExprSuffix --
     *            | e
     */
    ExprPtr _buildExprUnary(TokenIter & it);
    
    /*
     * ExprUnaryR2L -> ++ ExprUnaryR2L
     *               | -- ExprUnaryR2L
     *               | -  ExprUnaryR2L
     *               | +  ExprUnaryR2L
     *               | ~  ExprUnaryR2L
     *               | !  ExprUnaryR2L
     *               | ExprUnary
     */
    ExprPtr _buildExprUnaryR2L(TokenIter & it);
    
    /*
     * ExprMulDivMod -> ExprUnaryR2L ExprMulDivMod1
     * ExprMulDivMod1 -> * ExprUnaryR2L ExprMulDivMod1
     *                 | / ...
     *                 | % ...
     *                 | e
     */
    ExprPtr _buildExprMulDivMod1(TokenIter & it, ExprPtr pLeft);
    
    ExprPtr _buildExprMulDivMod(TokenIter & it);
    
    /*
     * ExprAddSub -> ExprMulDivMod ExprAddSub1
     * ExprAddSub1 -> + ExprMulDivMod ExprAddSub1
     *              | - ExprMulDivMod ExprAddSub1
     *              | e
     */
    ExprPtr _buildExprAddSub1(TokenIter & it, ExprPtr pLeft);
    
    ExprPtr _buildExprAddSub(TokenIter & it);
    
    /*
     * ExprShift -> ExprAddSub ExprShift1
     * ExprShift1 -> >> ExprAddSub ExprShift1
     *             | >>> ...
     *             | <<  ...
     *             | e
     */
    ExprPtr _buildExprShift1(TokenIter & it, ExprPtr pLeft);
    
    ExprPtr _buildExprShift(TokenIter & it);
    
    /*
     * ExprRelation -> ExprShift ExprRelation1
     * ExprRelation1 -> >  ExprShift ExprRelation1
     *                | >= ...
     *                | <  ...
     *                | >= ...
     *                | e
     */
    ExprPtr _buildExprRelation1(TokenIter & it, ExprPtr pLeft);
    
    ExprPtr _buildExprRelation(TokenIter & it);
    
    /*
     * ExprEqNe -> ExprRelation ExprEqNe1
     * ExprEqNe1 -> == ExprRelation ExprEqNe1
     *            | != ExprRelation ExprEqNe1
     *            | ε
     */
    ExprPtr _buildExprEqNe1(TokenIter & it, ExprPtr pLeft);
    
    ExprPtr _buildExprEqNe(TokenIter & it);
    
    /*
     * ExprBitAnd -> ExprEqNe ExprBitAnd1
     * ExprBitAnd1 -> & ExprEqNe ExprBitAnd1
     *               | ε
     */
    ExprPtr _buildExprBitAnd1(TokenIter & it, ExprPtr pLeft);
    
    ExprPtr _buildExprBitAnd(TokenIter & it);
    
    /*
     * ExprBitXor -> ExprBitAnd ExprBitXor1
     * ExprBitXor1 -> ^ ExprBitAnd ExprBitXor1
     *               | ε
     */
    ExprPtr _buildExprBitXor1(TokenIter & it, ExprPtr pLeft);
    
    ExprPtr _buildExprBitXor(TokenIter & it);
    
    /*
     * ExprBitOr -> ExprBitXor ExprBitOr1
     * ExprBitOr1 -> || ExprBitXor ExprBitOr1
     *               | ε
     */
    ExprPtr _buildExprBitOr1(TokenIter & it, ExprPtr pLeft);
    
    ExprPtr _buildExprBitOr(TokenIter & it);
    
    /*
     * ExprLogicAnd -> ExprBitOr ExprLogicAnd1
     * ExprLogicAnd1 -> && ExprBitOr ExprLogicAnd1
     *               | ε
     */
    ExprPtr _buildExprLogicAnd1(TokenIter & it, ExprPtr pLeft);
    
    ExprPtr _buildExprLogicAnd(TokenIter & it);
    
    /*
     * ExprLogicOr -> ExprLogicAnd ExprLogicOr1
     * ExprLogicOr1 -> || ExprLogicAnd ExprLogicOr1
     *               | ε
     */
    ExprPtr _buildExprLogicOr1(TokenIter & it, ExprPtr pLeft);
    
    ExprPtr _buildExprLogicOr(TokenIter & it);
    
    /*
     * ExprCond -> ExprLogicOr ? Expr : ExprCond
     *          | ExprLogicOr
     */
    ExprPtr _buildExprCond(TokenIter & it);
    
    /*
     * ExprAssign -> ExprCond ExprAssign1
     * ExprAssign1 -> { += | -= | *= | /= | %= | &= |
     * |= | ^= | <<= | >>= | >>>= | = } ExprAssign
     *          | ε
     *
     */
    ExprPtr _buildExprAssign1(TokenIter & it, ExprPtr pLeft);
    
    ExprPtr _buildExprAssign(TokenIter & it);
    
    /*
     * ExprComma -> ExprAssign ExprComma1
     * ExprComma1 -> , ExprAssign ExprComma1
     *              | ε
     */
    /*
    ExprPtr _buildExprComma1(TokenIter & it, ExprPtr pLeft);
    */
    
    /*
     * CommaList ->   ExprAssign CommaList1
     *              | ExprAssign
     * CommaList1 ->  , ExprAssign CommaList1
     *              | e
     */
    void _parseCommaList1(TokenIter & it, std::vector<ExprPtr> & args);
    
    ExprPtr _buildExprComma(TokenIter & it);
    
    ExprPtr buildExpression(TokenIter & it);
    
    /*
     * ScopeObjInitEntityList -> { ExprAssign ScopeObjInitEntityList1 }
     *                         | { }
     *                         | { ScopeObjInitEntityList ScopeObjInitEntityList1 }
     *
     * ScopeObjInitEntityList1 -> , ExprAssign ScopeObjInitEntityList1
     *                          | , ScopeObjInitEntityList ScopeObjInitEntityList1
     *                          | e
     */
    void parseScopeObjInitEntityList1(TokenIter & it,
            std::vector<LocalVar::InitEntityPtr> & entities);
    
    void parseScopeObjInitEntityList(TokenIter & it,
            std::vector<LocalVar::InitEntityPtr> & entities);
    
    /*
     * ScopeObjDefineStmt -> id ScopeObjDefineStmt1
     *             | id = ScopeObjInitExprList ScopeObjDefineStmt1
     *             | id = ExprAssign ScopeObjDefineStmt1
     *
     * ScopeObjDefineStmt1 -> , id ScopeObjDefineStmt1
     *             | , id = ScopeObjInitExprList ScopeObjDefineStmt1
     *             | , id = ExprAssign ScopeObjDefineStmt1
     *             | e
     */
    void parseScopeObjDefineStmt1(
            TokenIter & it,
            std::vector<LocalVar> & defScopeObjs,
            const LocalVar & modelScopeObj);
    
    void parseScopeObjDefineStmt(
            TokenIter & it,
            std::vector<LocalVar> & defScopeObjs,
            const LocalVar & modelScopeObj);
    
    /*
     * DefOrPureExprStmt ->
     *  | DefType ScopeObjDefineStmt
     *  | Expr
     */
    StmtPtr buildDefOrPureExprStmt(TokenIter & it);
    
    /* Stmt ->
     *  | ;
     *  | for ( DefOrPureExprStmt | e ; Expr | e ; Expr | e ) { Stmts }
     *  | while ( Expr ) { Stmts }
     *  | do { Stmts } while ( Expr ) ;
     *  | if ...
     *  | switch ...
     *  | return Expr ;
     *  | return ;
     *  | break ;
     *  | continue ;
     *  | DefOrPureExprStmt ;    // else
     *  | { Stmts }
     *
     * DefOrPureExprStmt ->
     *  | DefType ScopeObjDefineStmt
     *  | Expr
     *
     * Stmts -> Stmt Stmts
     *       | e
     */
    StmtPtr buildStmt(TokenIter & it);
    
    StmtsPtr buildStmts(TokenIter & it);
};

#endif
