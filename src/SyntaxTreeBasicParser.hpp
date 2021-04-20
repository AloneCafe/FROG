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
    
    explicit AstBasicParser(AstErrorDriver<ErrorDriver> & aed)
            : _aed(aed) { }
    
    void parseLowLvTypeList_(TokenIter & it, std::vector<LocatedUtfString> & refLowLvTypeList) {
        for (;;) {
            if (it->isPunc<','>()) {
                ++it;
                LocatedUtfString lowLvObjType;
                parseLowLvType(it, lowLvObjType);
                refLowLvTypeList.push_back(lowLvObjType);
                
            } else if (it->isEnd()) {
                // 错误: 意外的结尾
                AST_E(E_UNEXPECTED_EOF);
                break;
                
            } else {
                // 此处直接返回: 遇到非逗号，就暂且闭合列表
                break;
            }
        }
    }
    
    void parseLowLvTypeList(TokenIter & it, std::vector<LocatedUtfString> & refLowLvTypeList) {
        LocatedUtfString lowLvObjType;
        parseLowLvType(it, lowLvObjType);
        refLowLvTypeList.push_back(lowLvObjType);
        parseLowLvTypeList_(it, refLowLvTypeList);
    }
    
    /*
     * LowLvType -> id
     *            | scalar-type
     */
    void parseLowLvType(TokenIter & it, LocatedUtfString & lowLvType) {
        AST_REC_LOCATION1
        /*if (it->isId()) {
            lowLvType = LocatedUtfString::make(it->getId(), AST_ARG_LOCATION1);
            ++ it;
            
        } else*/ if (it->isKwVoid()) {
            lowLvType = LocatedUtfString::make("void", AST_ARG_LOCATION1);
            ++it;
            
        } else if (it->isKwByte()) {
            lowLvType = LocatedUtfString::make("byte", AST_ARG_LOCATION1);
            ++it;
            
        } else if (it->isKwChar()) {
            lowLvType = LocatedUtfString::make("char", AST_ARG_LOCATION1);
            ++it;
            
        } else if (it->isKwDouble()) {
            lowLvType = LocatedUtfString::make("double", AST_ARG_LOCATION1);
            ++it;
            
        } else if (it->isKwFloat()) {
            lowLvType = LocatedUtfString::make("float", AST_ARG_LOCATION1);
            ++it;
            
        } else if (it->isKwInt()) {
            lowLvType = LocatedUtfString::make("int", AST_ARG_LOCATION1);
            ++it;
            
        } else if (it->isKwLong()) {
            lowLvType = LocatedUtfString::make("long", AST_ARG_LOCATION1);
            ++it;
            
        } else if (it->isKwShort()) {
            lowLvType = LocatedUtfString::make("short", AST_ARG_LOCATION1);
            ++it;
            
        } else if (it->isKwBoolean()) {
            lowLvType = LocatedUtfString::make("boolean", AST_ARG_LOCATION1);
            ++it;
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
        } else {
            // 错误: 非法的token
            AST_E(E_ILLEGAL_TOKEN); // 先报错，迭代器不变，交由上层
        }
    }
    
    void parseNoNumIndexList(TokenIter & it, int & degree) {
        if (it->isPunc<'['>()) {
            ++it;
            if (it->isPunc<']'>()) {
                ++it;
                ++degree;
                parseNoNumIndexList(it, degree);
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_LOSE_RBRACKET);
            }
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            
        } else {
            // 直接闭合
        }
    }
    
    /*
     * DefType -> LowLvType NoNumIndexList
     */
    void parseDefType(TokenIter & it, VarType & type) {
        LocatedUtfString lowLvType;
        int degree = 0; // 先给 getDegreeRef 置零
        
        assert(type.getLowLvType().empty());
        
        parseLowLvType(it, lowLvType);
        parseNoNumIndexList(it, degree);
        
        type.setLowLvType(lowLvType);
        type.setDegree(degree);
    }
    
    /*
     * FormalArgList -> DefType id FormalArgList1
     *              | e
     * FormalArgList1 -> , DefType id FormalArgList1
     *              | e
     */
    void _parseFormalArgList1(TokenIter & it, std::vector<FormalArg> & args) {
        if (it->isPunc<','>()) {
            ++it;
            if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else { // 解析是否是 id 或者标量关键字，然后紧接着 id
                FormalArg fa;
                
                VarType type;
                // id 或者标量关键字，表示参数类型
                // 此函数吃掉所有与类型有关的token，直接到达下一个希望的 id（参数名）
                parseDefType(it, type);
                fa.setType(type);
                
                if (it->isId()) {
                    AST_REC_LOCATION1 // id，表示参数名
                    fa.setName(LocatedUtfString::make(it->getId(), AST_ARG_LOCATION1));
                    
                } else if (it->isEnd()) {
                    AST_E(E_UNEXPECTED_EOF);
                    
                } else {
                    AST_E(E_ILLEGAL_FORMAL_ARG_NAME); // 报错，非法的形式参数名
                }
                ++it;
                
                args.push_back(fa);
                _parseFormalArgList1(it, args);
            }
            
        } else if (it->isPunc<')'>()) {
            ++it; // 吃掉右括号
            // 闭合，返回
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            
        } else { // 报错: 不可接受的 token
            AST_E(E_ILLEGAL_TOKEN);
        }
    }
    
    void _parseFormalArgList(TokenIter & it, std::vector<FormalArg> & args) {
        assert(it->isPunc<'('>());
        ++it;
        
        if (it->isPunc<')'>()) {
            ++it; // 吃掉右括号
            // 碰到 ')' 右括号，说明此参数列表为空，也就是e，直接返回
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            
        } else { // 解析是否是 id 或者标量关键字，然后紧接着 id
            FormalArg fa;
            
            VarType type;
            // id 或者标量关键字，表示参数类型
            // 此函数吃掉所有与类型有关的token，直接到达下一个希望的 id（参数名）
            parseDefType(it, type);
            fa.setType(type);
            
            if (it->isId()) {
                AST_REC_LOCATION1 // id，表示参数名
                fa.setName(LocatedUtfString::make(it->getId(), AST_ARG_LOCATION1));
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_ILLEGAL_FORMAL_ARG_NAME); // 报错，非法的形式参数名
            }
            ++it;
            
            args.push_back(fa);
            _parseFormalArgList1(it, args);
        }
    }
    
    /*
     * RealArgList -> ( ExprAssign RealArgList1 )
     *              | ( )
     * RealArgList1 -> , ExprAssign RealArgList1
     *              | e
     */
    void _parseRealArgList1(TokenIter & it, std::vector<ExprPtr> & args) {
        if (it->isPunc<','>()) {
            ++it;
            if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else { // 进入自递归
                ExprPtr pExpr = _buildExprAssign(it);
                args.push_back(pExpr);
                _parseRealArgList1(it, args);
            }
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            
        } else {
            return; // 不递进，暂且返回给上层
        }
    }
    
    void _parseRealArgList(TokenIter & it, std::vector<ExprPtr> & args) {
        assert(it->isPunc<'('>());
        ++it;
        
        if (it->isPunc<')'>()) {
            ++it; // 吃掉右括号
            // 碰到 ')' 右括号，说明此参数列表为空，也就是e，直接返回
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            
        } else { // 进入后续递归
            ExprPtr pExpr = _buildExprAssign(it);
            args.push_back(pExpr);
            
            _parseRealArgList1(it, args);
            
            if (it->isPunc<')'>()) {
                ++it; // 吃掉右括号
                // 闭合，返回
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else { // 报错: 不可接受的 token
                AST_E(E_ILLEGAL_TOKEN);
            }
        }
    }
    
    /*
     * ExprDotList -> id . ExprDotList
     *             | id
     */
    /*
    ExprPtr _buildExprDotList(TokenIter & it) {
        assert(it->isId());
        AST_REC_LOCATION1
        ExprPtr pLeft = Expression::newLeafScalar(ExprOp::LEAF_ID, it->getVal(), AST_ARG_LOCATION1);
        ++ it;
        if (it->isPunc<'.'>()) {
            AST_REC_LOCATION1
            ++ it;
            ExprPtr pRight = _buildExprDotList(it);
            ExprPtr pCon = Expression::newExpr(ExprOp::OPT_DOT, {pLeft, pRight}, AST_ARG_LOCATION1);
            return pCon;
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else {
            return pLeft;
        }
    }
    */
    
    /*
     * ExprIndexList -> [ Expr ] ExprIndexList
     */
    ExprPtr _buildExprIndexList(TokenIter & it, ExprPtr pLeft) {
        assert(it->isPunc<'['>());
        AST_REC_LOCATION1
        ++it;
        
        ExprPtr pRight = buildExpression(it);
        
        // pLeft 主要是左侧 (通常是 ExprDotList 或者 ExprIndexList)
        // 此处左右合并为完整的二元 OPT_INDEX 表达式
        ExprPtr pNewLeft = Expression::newExpr(ExprOp::OPT_INDEX, {pLeft, pRight}, AST_ARG_LOCATION1);
        
        if (it->isPunc<']'>()) {
            ++it;
            
            if (it->isPunc<'['>()) {
                return _buildExprIndexList(it, pNewLeft);
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                return pNewLeft; // 报错，暂且返回表达式
                
            } else {
                return pNewLeft; // 构造完成，返回表达式
            }
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft; // 构造不完整，返回左侧原表达式
            
        } else {
            AST_E(E_LOSE_RBRACKET);
            return pLeft; // 构造不完整，返回左侧原表达式
        }
        
    }
    
    /*
     * ExprSuffix -> ( Expr )
     *             | @ DefType ( Expr )   // 强制类型转换
     *             | id ExprIndexList
     *             | id ( RealArgList ) ExprIndexList // 此处括号是隐含的
     *             | any-literal
     *             | true
     *             | false
     * ///// 注意: 函数调用的参数表达式是由逗号分隔开的一组参数列表，
     * ///// 列表中每一项是比逗号运算符优先级更低 (即赋值及以下) 的表达式组成，
     * ///// 也就是以 " ( ExprAssign, ExprAssign, ... , ExprAssign ) " 的形式组成
     *
     * ///// ExprIndexList 表示 0~N 个下标运算符组合而成的链
     * /////
     *
     */
    ExprPtr _buildExprSuffix(TokenIter & it) {
        AST_REC_LOCATION1
        if (it->isPunc<'@'>()) {
            ++it;
            VarType objType;
            parseDefType(it, objType);
            ExprPtr pLeft = Expression::newLeafType(objType, AST_ARG_LOCATION1);
            if (it->isPunc<'('>()) {
                AST_REC_LOCATION2
                ++it;
                ExprPtr pRight = buildExpression(it);
                if (it->isPunc<')'>()) {
                    ++it;
                    ExprPtr pCon = Expression::newExpr(ExprOp::OPT_TYPE_CONVERT, {pLeft, pRight}, AST_ARG_LOCATION2);
                    return pCon;
                    
                } else if (it->isEnd()) {
                    AST_E(E_UNEXPECTED_EOF);
                    return pLeft;
                    
                } else {
                    AST_E(E_LOSE_RP);
                    return pLeft;
                }
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                return pLeft;
                
            } else {
                AST_E(E_ILLEGAL_TOKEN);
                return pLeft; // 报错，暂且返回
            }
            
        } else if (it->isPunc<'('>()) {
            ++it;
            ExprPtr pInner = buildExpression(it);
            if (it->isPunc<')'>()) {
                ++it;
                return pInner;
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                return pInner;
                
            } else {
                AST_E(E_LOSE_RP);
                return pInner; // 报错，暂且返回
            }
            
        } else if (it->isId()) {
            TokenValue tv;
            tv._str = it->getId();
            ExprPtr pId = Expression::newLeafScalar(ExprOp::LEAF_ID, tv, AST_ARG_LOCATION1);
            ++it;
            if (it->isPunc<'('>()) { // 调用表达式
                AST_REC_LOCATION1
                // 吸收逗号隔开的参数列表
                std::vector<ExprPtr> refArgList;
                _parseRealArgList(it, refArgList);
                
                // 完成表达式构建
                // 此处不可用 newExpr() 构造 OPT_CALL 表达式
                // return Expression::newCallExpr(ExprOp::OPT_CALL, ...);
                ExprPtr pCallExpr = Expression::newCallExpr(pId, refArgList, AST_ARG_LOCATION1);
                
                // 下标表达式
                if (it->isPunc<'['>()) {
                    return _buildExprIndexList(it, pCallExpr);
                    
                } else if (it->isEnd()) {
                    AST_E(E_UNEXPECTED_EOF); // 暂且返回
                    return pCallExpr;
                    
                } else {
                    return pCallExpr;
                }
                
            } else if (it->isPunc<'['>()) {
                return _buildExprIndexList(it, pId);
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF); // 报错，暂且将点表达式返回上层
                return pId;
                
            } else {
                return pId; // 直接当作点表达式，完成，返回上层
            }
            
        } else if (it->isType(TOKEN_LITERAL_CHARSEQ)) {
            ExprPtr pExpr = Expression::newLeafScalar(ExprOp::LEAF_CHAR_LITERAL, it->getVal(), AST_ARG_LOCATION1);
            ++it;
            return pExpr;
            
        } else if (it->isType(TOKEN_LITERAL_STRING)) {
            ExprPtr pExpr = Expression::newLeafScalar(ExprOp::LEAF_STRING_LITERAL, it->getVal(), AST_ARG_LOCATION1);
            ++it;
            return pExpr;
            
        } else if (it->isType(TOKEN_LITERAL_INT)) {
            ExprPtr pExpr = Expression::newLeafScalar(ExprOp::LEAF_INT_LITERAL, it->getVal(), AST_ARG_LOCATION1);
            ++it;
            return pExpr;
            
        } else if (it->isType(TOKEN_LITERAL_LONG)) {
            ExprPtr pExpr = Expression::newLeafScalar(ExprOp::LEAF_LONG_LITERAL, it->getVal(), AST_ARG_LOCATION1);
            ++it;
            return pExpr;
            
        } else if (it->isType(TOKEN_LITERAL_UNSIGNED_INT)) {
            ExprPtr pExpr = Expression::newLeafScalar(ExprOp::LEAF_INT_LITERAL, it->getVal(), AST_ARG_LOCATION1);
            ++it;
            return pExpr;
            
        } else if (it->isType(TOKEN_LITERAL_UNSIGNED_LONG)) {
            ExprPtr pExpr = Expression::newLeafScalar(ExprOp::LEAF_LONG_LITERAL, it->getVal(), AST_ARG_LOCATION1);
            ++it;
            return pExpr;
            
        } else if (it->isType(TOKEN_LITERAL_FLOAT)) {
            ExprPtr pExpr = Expression::newLeafScalar(ExprOp::LEAF_FLOAT_LITERAL, it->getVal(), AST_ARG_LOCATION1);
            ++it;
            return pExpr;
            
        } else if (it->isType(TOKEN_LITERAL_DOUBLE)) {
            ExprPtr pExpr = Expression::newLeafScalar(ExprOp::LEAF_DOUBLE_LITERAL, it->getVal(), AST_ARG_LOCATION1);
            ++it;
            return pExpr;
            
        } else if (it->isType(TOKEN_LITERAL_LONG_DOUBLE)) {
            ExprPtr pExpr = Expression::newLeafScalar(ExprOp::LEAF_LONG_DOUBLE_LITERAL, it->getVal(),
                    AST_ARG_LOCATION1);
            ++it;
            return pExpr;
            
        } else if (it->isKwTrue()) { // true
            ExprPtr pExpr = Expression::newLeafPure(ExprOp::LEAF_TRUE_LITERAL, AST_ARG_LOCATION1);
            ++it;
            return pExpr;
            
        } else if (it->isKwFalse()) { // false
            ExprPtr pExpr = Expression::newLeafPure(ExprOp::LEAF_FALSE_LITERAL, AST_ARG_LOCATION1);
            ++it;
            return pExpr;
            
        } else {
            AST_E(E_ILLEGAL_TOKEN); // 暂且报错
            return nullptr;
        }
    }
    
    /*
     * ExprUnary -> ExprSuffix ++
     *            | ExprSuffix --
     *            | e
     */
    ExprPtr _buildExprUnary(TokenIter & it) {
        ExprPtr pExpr = _buildExprSuffix(it);
        ExprOp op;
        AST_REC_LOCATION1
        if (it->isPunc<'++'>()) {
            op = ExprOp::OPT_BACK_PLUSPLUS;
        } else if (it->isPunc<'--'>()) {
            op = ExprOp::OPT_BACK_MINUSMINUS;
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pExpr;
            
        } else {
            return pExpr;
        }
        
        ++it;
        return Expression::newExpr(op, {pExpr}, AST_ARG_LOCATION1);
    }
    
    /*
     * ExprUnaryR2L -> ++ ExprUnaryR2L
     *               | -- ExprUnaryR2L
     *               | -  ExprUnaryR2L
     *               | +  ExprUnaryR2L
     *               | ~  ExprUnaryR2L
     *               | !  ExprUnaryR2L
     *               | ExprUnary
     */
    ExprPtr _buildExprUnaryR2L(TokenIter & it) {
        ExprOp op;
        AST_REC_LOCATION1
        if (it->isPunc<'++'>()) {
            op = ExprOp::OPT_FRONT_PLUSPLUS;
            
        } else if (it->isPunc<'--'>()) {
            op = ExprOp::OPT_FRONT_MINUSMINUS;
            
        } else if (it->isPunc<'-'>()) {
            op = ExprOp::OPT_NEGATIVE;
            
        } else if (it->isPunc<'+'>()) {
            op = ExprOp::OPT_POSITIVE;
            
        } else if (it->isPunc<'~'>()) {
            op = ExprOp::OPT_BIT_NOT;
            
        } else if (it->isPunc<'!'>()) {
            op = ExprOp::OPT_LOGIC_NOT;
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return nullptr;  // 这个表达式没有左侧 (一元)，暂且返回 nullptr
            
        } else {
            return _buildExprUnary(it);
        }
        
        ++it;
        ExprPtr pExpr = _buildExprUnaryR2L(it);
        return Expression::newExpr(op, {pExpr}, AST_ARG_LOCATION1);
    }
    
    /*
     * ExprMulDivMod -> ExprUnaryR2L ExprMulDivMod1
     * ExprMulDivMod1 -> * ExprUnaryR2L ExprMulDivMod1
     *                 | / ...
     *                 | % ...
     *                 | e
     */
    ExprPtr _buildExprMulDivMod1(TokenIter & it, ExprPtr pLeft) {
        ExprOp op;
        AST_REC_LOCATION1
        if (it->isPunc<'*'>()) {
            op = ExprOp::OPT_MUL;
            
        } else if (it->isPunc<'/'>()) {
            op = ExprOp::OPT_DIV;
            
        } else if (it->isPunc<'%'>()) {
            op = ExprOp::OPT_MOD;
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else {
            return pLeft;
        }
        
        ++it;
        ExprPtr pRight = _buildExprUnaryR2L(it);
        ExprPtr pNewLeft = Expression::newExpr(op, {pLeft, pRight}, AST_ARG_LOCATION1);
        return _buildExprMulDivMod1(it, pNewLeft);
    }
    
    ExprPtr _buildExprMulDivMod(TokenIter & it) {
        ExprPtr pLeft = _buildExprUnaryR2L(it);
        return _buildExprMulDivMod1(it, pLeft);
    }
    
    /*
     * ExprAddSub -> ExprMulDivMod ExprAddSub1
     * ExprAddSub1 -> + ExprMulDivMod ExprAddSub1
     *              | - ExprMulDivMod ExprAddSub1
     *              | e
     */
    ExprPtr _buildExprAddSub1(TokenIter & it, ExprPtr pLeft) {
        ExprOp op;
        AST_REC_LOCATION1
        if (it->isPunc<'+'>()) {
            op = ExprOp::OPT_PLUS;
            
        } else if (it->isPunc<'-'>()) {
            op = ExprOp::OPT_MINUS;
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else {
            return pLeft;
        }
        
        ++it;
        ExprPtr pRight = _buildExprMulDivMod(it);
        ExprPtr pNewLeft = Expression::newExpr(op, {pLeft, pRight}, AST_ARG_LOCATION1);
        return _buildExprAddSub1(it, pNewLeft);
    }
    
    ExprPtr _buildExprAddSub(TokenIter & it) {
        ExprPtr pLeft = _buildExprMulDivMod(it);
        return _buildExprAddSub1(it, pLeft);
    }
    
    /*
     * ExprShift -> ExprAddSub ExprShift1
     * ExprShift1 -> >> ExprAddSub ExprShift1
     *             | >>> ...
     *             | <<  ...
     *             | e
     */
    ExprPtr _buildExprShift1(TokenIter & it, ExprPtr pLeft) {
        ExprOp op;
        AST_REC_LOCATION1
        if (it->isPunc<'>>'>()) {
            op = ExprOp::OPT_RSHIFT;
        } else if (it->isPunc<'>>>'>()) {
            op = ExprOp::OPT_RSHIFT_ZERO;
        } else if (it->isPunc<'<<'>()) {
            op = ExprOp::OPT_LSHIFT;
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else {
            return pLeft;
        }
        
        ++it;
        ExprPtr pRight = _buildExprAddSub(it);
        ExprPtr pNewLeft = Expression::newExpr(op, {pLeft, pRight}, AST_ARG_LOCATION1);
        return _buildExprShift1(it, pNewLeft);
    }
    
    ExprPtr _buildExprShift(TokenIter & it) {
        ExprPtr pLeft = _buildExprAddSub(it);
        return _buildExprShift1(it, pLeft);
    }
    
    /*
     * ExprRelation -> ExprShift ExprRelation1
     * ExprRelation1 -> >  ExprShift ExprRelation1
     *                | >= ...
     *                | <  ...
     *                | >= ...
     *                | e
     */
    ExprPtr _buildExprRelation1(TokenIter & it, ExprPtr pLeft) {
        ExprOp op;
        AST_REC_LOCATION1
        if (it->isPunc<'>'>()) {
            op = ExprOp::OPT_GT;
        } else if (it->isPunc<'>='>()) {
            op = ExprOp::OPT_GE;
        } else if (it->isPunc<'<'>()) {
            op = ExprOp::OPT_LT;
        } else if (it->isPunc<'<='>()) {
            op = ExprOp::OPT_LE;
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else {
            return pLeft;
        }
        
        ++it;
        ExprPtr pRight = _buildExprShift(it);
        ExprPtr pNewLeft = Expression::newExpr(op, {pLeft, pRight}, AST_ARG_LOCATION1);
        return _buildExprRelation1(it, pNewLeft);
    }
    
    ExprPtr _buildExprRelation(TokenIter & it) {
        ExprPtr pLeft = _buildExprShift(it);
        return _buildExprRelation1(it, pLeft);
    }
    
    /*
     * ExprEqNe -> ExprRelation ExprEqNe1
     * ExprEqNe1 -> == ExprRelation ExprEqNe1
     *            | != ExprRelation ExprEqNe1
     *            | ε
     */
    ExprPtr _buildExprEqNe1(TokenIter & it, ExprPtr pLeft) {
        ExprOp op;
        AST_REC_LOCATION1
        if (it->isPunc<'=='>()) {
            op = ExprOp::OPT_EQ;
            
        } else if (it->isPunc<'!='>()) {
            op = ExprOp::OPT_NE;
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else {
            return pLeft;
        }
        
        ++it;
        ExprPtr pRight = _buildExprRelation(it);
        ExprPtr pNewLeft = Expression::newExpr(op, {pLeft, pRight}, AST_ARG_LOCATION1);
        return _buildExprEqNe1(it, pNewLeft);
    }
    
    ExprPtr _buildExprEqNe(TokenIter & it) {
        ExprPtr pLeft = _buildExprRelation(it);
        return _buildExprEqNe1(it, pLeft);
    }
    
    /*
     * ExprBitAnd -> ExprEqNe ExprBitAnd1
     * ExprBitAnd1 -> & ExprEqNe ExprBitAnd1
     *               | ε
     */
    ExprPtr _buildExprBitAnd1(TokenIter & it, ExprPtr pLeft) {
        AST_REC_LOCATION1
        if (it->isPunc<'&'>()) {
            ++it;
            ExprPtr pRight = _buildExprEqNe(it);
            ExprPtr pNewLeft =
                    Expression::newExpr(ExprOp::OPT_BIT_AND, {pLeft, pRight}, AST_ARG_LOCATION1);
            return _buildExprBitAnd1(it, pNewLeft);
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else {
            return pLeft;
        }
    }
    
    ExprPtr _buildExprBitAnd(TokenIter & it) {
        ExprPtr pLeft = _buildExprEqNe(it);
        return _buildExprBitAnd1(it, pLeft);
    }
    
    /*
     * ExprBitXor -> ExprBitAnd ExprBitXor1
     * ExprBitXor1 -> ^ ExprBitAnd ExprBitXor1
     *               | ε
     */
    ExprPtr _buildExprBitXor1(TokenIter & it, ExprPtr pLeft) {
        AST_REC_LOCATION1
        if (it->isPunc<'^'>()) {
            ++it;
            ExprPtr pRight = _buildExprBitAnd(it);
            ExprPtr pNewLeft =
                    Expression::newExpr(ExprOp::OPT_BIT_XOR, {pLeft, pRight}, AST_ARG_LOCATION1);
            return _buildExprBitXor1(it, pNewLeft);
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else {
            return pLeft;
        }
    }
    
    ExprPtr _buildExprBitXor(TokenIter & it) {
        ExprPtr pLeft = _buildExprBitAnd(it);
        return _buildExprBitXor1(it, pLeft);
    }
    
    /*
     * ExprBitOr -> ExprBitXor ExprBitOr1
     * ExprBitOr1 -> || ExprBitXor ExprBitOr1
     *               | ε
     */
    ExprPtr _buildExprBitOr1(TokenIter & it, ExprPtr pLeft) {
        AST_REC_LOCATION1
        if (it->isPunc<'|'>()) {
            ++it;
            ExprPtr pRight = _buildExprBitXor(it);
            ExprPtr pNewLeft =
                    Expression::newExpr(ExprOp::OPT_BIT_OR, {pLeft, pRight}, AST_ARG_LOCATION1);
            return _buildExprBitOr1(it, pNewLeft);
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else {
            return pLeft;
        }
    }
    
    ExprPtr _buildExprBitOr(TokenIter & it) {
        ExprPtr pLeft = _buildExprBitXor(it);
        return _buildExprBitOr1(it, pLeft);
    }
    
    /*
     * ExprLogicAnd -> ExprBitOr ExprLogicAnd1
     * ExprLogicAnd1 -> && ExprBitOr ExprLogicAnd1
     *               | ε
     */
    ExprPtr _buildExprLogicAnd1(TokenIter & it, ExprPtr pLeft) {
        AST_REC_LOCATION1
        if (it->isPunc<'&&'>()) {
            ++it;
            ExprPtr pRight = _buildExprBitOr(it);
            ExprPtr pNewLeft =
                    Expression::newExpr(ExprOp::OPT_LOGIC_AND, {pLeft, pRight}, AST_ARG_LOCATION1);
            return _buildExprLogicOr1(it, pNewLeft);
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else {
            return pLeft;
        }
    }
    
    ExprPtr _buildExprLogicAnd(TokenIter & it) {
        ExprPtr pLeft = _buildExprBitOr(it);
        return _buildExprLogicAnd1(it, pLeft);
    }
    
    /*
     * ExprLogicOr -> ExprLogicAnd ExprLogicOr1
     * ExprLogicOr1 -> || ExprLogicAnd ExprLogicOr1
     *               | ε
     */
    ExprPtr _buildExprLogicOr1(TokenIter & it, ExprPtr pLeft) {
        AST_REC_LOCATION1
        if (it->isPunc<'||'>()) {
            ++it;
            ExprPtr pRight = _buildExprLogicAnd(it);
            ExprPtr pNewLeft = Expression::newExpr(ExprOp::OPT_LOGIC_OR, {pLeft, pRight}, AST_ARG_LOCATION1);
            return _buildExprLogicOr1(it, pNewLeft);
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else {
            return pLeft;
        }
    }
    
    ExprPtr _buildExprLogicOr(TokenIter & it) {
        ExprPtr pLeft = _buildExprLogicAnd(it);
        return _buildExprLogicOr1(it, pLeft);
    }
    
    /*
     * ExprCond -> ExprLogicOr ? Expr : ExprCond
     *          | ExprLogicOr
     */
    ExprPtr _buildExprCond(TokenIter & it) {
        ExprPtr pLeft, pMid, pRight;
        pLeft = _buildExprLogicOr(it);
        AST_REC_LOCATION1
        if (it->isPunc<'?'>()) {
            ++it;
            pMid = buildExpression(it);
            if (it->isPunc<':'>()) {
                ++it;
                pRight = _buildExprCond(it);
                return Expression::newExpr(ExprOp::OPT_COND, {pLeft, pMid, pRight}, AST_ARG_LOCATION1);
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                return pLeft; // 此处表达式未正常闭合，暂且返回左侧表达式
                
            } else {
                AST_E(E_ILLEGAL_TOKEN); // 报错非法token，应该接受一个冒号 ':'
                return pLeft;
            }
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else {
            // 此处不管，直接返回左侧表达式
            return pLeft;
        }
    }
    
    /*
     * ExprAssign -> ExprCond ExprAssign1
     * ExprAssign1 -> { += | -= | *= | /= | %= | &= |
     * |= | ^= | <<= | >>= | >>>= | = } ExprAssign
     *          | ε
     *
     */
    ExprPtr _buildExprAssign1(TokenIter & it, ExprPtr pLeft) {
        ExprOp op;
        AST_REC_LOCATION1
        if (it->isPunc<'+='>()) {
            op = ExprOp::OPT_ASSIGN_WITH_ADD;
            
        } else if (it->isPunc<'-='>()) {
            op = ExprOp::OPT_ASSIGN_WITH_SUB;
            
        } else if (it->isPunc<'*='>()) {
            op = ExprOp::OPT_ASSIGN_WITH_MUL;
            
        } else if (it->isPunc<'/='>()) {
            op = ExprOp::OPT_ASSIGN_WITH_DIV;
            
        } else if (it->isPunc<'%='>()) {
            op = ExprOp::OPT_ASSIGN_WITH_MOD;
            
        } else if (it->isPunc<'&='>()) {
            op = ExprOp::OPT_ASSIGN_WITH_BIT_AND;
            
        } else if (it->isPunc<'|='>()) {
            op = ExprOp::OPT_ASSIGN_WITH_BIT_OR;
            
        } else if (it->isPunc<'^='>()) {
            op = ExprOp::OPT_ASSIGN_WITH_BIT_XOR;
            
        } else if (it->isPunc<'<<='>()) {
            op = ExprOp::OPT_ASSIGN_WITH_LSHIFT;
            
        } else if (it->isPunc<'>>='>()) {
            op = ExprOp::OPT_ASSIGN_WITH_RSHIFT;
            
        } else if (it->isPunc<'>>>='>()) {
            op = ExprOp::OPT_ASSIGN_WITH_RSHIFT_ZERO;
            
        } else if (it->isPunc<'='>()) {
            op = ExprOp::OPT_ASSIGN;
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else { // 遭遇其他token，对于本表达式来说没有右侧，直接返回本表达式的左侧，即接受ε
            return pLeft;
        }
        
        // if-elseif 处理代码的后置。满足各项赋值操作符都执行如下代码
        ++it;
        //ExprPtr pRight = _buildExprCond(it);
        ExprPtr pRight = _buildExprAssign(it); // 从右到左结合性
        ExprPtr pNewLeft = Expression::newExpr(op, {pLeft, pRight}, AST_ARG_LOCATION1);
        return _buildExprAssign1(it, pNewLeft);
    }
    
    ExprPtr _buildExprAssign(TokenIter & it) {
        ExprPtr pLeft = _buildExprCond(it);
        return _buildExprAssign1(it, pLeft);
    }
    
    /*
     * ExprComma -> ExprAssign ExprComma1
     * ExprComma1 -> , ExprAssign ExprComma1
     *              | ε
     */
    /*
    ExprPtr _buildExprComma1(TokenIter & it, ExprPtr pLeft) {
        AST_REC_LOCATION1
        if (it->isPunc<','>()) {
            ++ it;
            ExprPtr pRight = _buildExprAssign(it);
            ExprPtr pNewLeft = Expression::newExpr(ExprOp::OPT_COMMA, {pLeft, pRight}, AST_ARG_LOCATION1);
            return _buildExprComma1(it, pNewLeft);
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return pLeft;
            
        } else { // 遭遇其他token，对于本表达式来说没有右侧，直接返回本表达式的左侧，即接受ε
            return pLeft;
        }
    }
    */
    
    /*
     * CommaList ->   ExprAssign CommaList1
     *              | ExprAssign
     * CommaList1 ->  , ExprAssign CommaList1
     *              | e
     */
    void _parseCommaList1(TokenIter & it, std::vector<ExprPtr> & args) {
        if (it->isPunc<','>()) {
            ++it;
            if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else { // 进入自递归
                ExprPtr pExpr = _buildExprAssign(it);
                args.push_back(pExpr);
                _parseCommaList1(it, args);
            }
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            
        } else {
            return; // 不递进，暂且返回给上层
        }
    }
    
    ExprPtr _buildExprComma(TokenIter & it) {
        AST_REC_LOCATION1
        std::vector<ExprPtr> commaList;
        ExprPtr pExpr = _buildExprAssign(it);
        commaList.push_back(pExpr);
        _parseCommaList1(it, commaList);
        if (commaList.size() > 1) {
            return Expression::newCommaExpr(commaList, AST_ARG_LOCATION1);
        } else {
            return commaList[0];
        }
    }
    
    ExprPtr buildExpression(TokenIter & it) {
        if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return nullptr;
        } else {
            return _buildExprComma(it);
        }
    }
    
    /*
     * ScopeObjInitEntityList -> { ExprAssign ScopeObjInitEntityList1 }
     *                         | { }
     *                         | { ScopeObjInitEntityList ScopeObjInitEntityList1 }
     *
     * ScopeObjInitEntityList1 -> , ExprAssign ScopeObjInitEntityList1
     *                          | , ScopeObjInitEntityList ScopeObjInitEntityList1
     *                          | e
     */
    void parseScopeObjInitEntityList1(TokenIter & it, std::vector<LocalVar::InitEntityPtr> & entities) {
        if (it->isPunc<','>()) {
            ++it;
            if (it->isPunc<'{'>()) {
                // 说明元素是列表，进入递归
                std::vector<LocalVar::InitEntityPtr> subEntities;
                parseScopeObjInitEntityList(it, subEntities);
                entities.push_back(LocalVar::IInitEntity::newInitList(subEntities));
                
                parseScopeObjInitEntityList1(it, entities);
                
                // 吸收右括号
                if (it->isPunc<'}'>()) {
                    //++it; // 完美闭合
                } else if (it->isEnd()) {
                    AST_E(E_UNEXPECTED_EOF);
                    
                } else {
                    AST_E(E_LOSE_RBRACE); // 错误，可能丢失右括号
                }
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else { // 说明是单个表达式，进入后续递归
                ExprPtr pExpr = _buildExprAssign(it);
                entities.push_back(LocalVar::IInitEntity::newInitExpr(pExpr));
                
                parseScopeObjInitEntityList1(it, entities);
            }
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            
        } else {
            return; // 遇到非逗号的token，暂且返回
        }
    }
    
    void parseScopeObjInitEntityList(TokenIter & it, std::vector<LocalVar::InitEntityPtr> & entities) {
        assert(it->isPunc<'{'>());
        ++it;
        
        if (it->isPunc<'}'>()) {
            ++it; // 吃掉右花括号
            // 碰到 '}' 右花括号，说明此参数列表为空，直接返回
        } else if (it->isPunc<'{'>()) {
            // 说明元素是列表，进入递归
            std::vector<LocalVar::InitEntityPtr> subEntities;
            parseScopeObjInitEntityList(it, subEntities);
            entities.push_back(LocalVar::IInitEntity::newInitList(subEntities));
            
            parseScopeObjInitEntityList1(it, entities);
            
            // 吸收右括号
            if (it->isPunc<'}'>()) {
                ++it; // 完美闭合
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_LOSE_RBRACE); // 错误，可能丢失右括号
            }
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            return;
            
        } else { // 说明是单个表达式，进入后续递归
            ExprPtr pExpr = _buildExprAssign(it);
            entities.push_back(LocalVar::IInitEntity::newInitExpr(pExpr));
            
            parseScopeObjInitEntityList1(it, entities);
            
            // 吸收右括号
            if (it->isPunc<'}'>()) {
                ++it; // 完美闭合
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_LOSE_RBRACE); // 错误，可能丢失右括号
            }
        }
        
    }
    
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
    void parseScopeObjDefineStmt1(TokenIter & it, std::vector<LocalVar> & defScopeObjs,
            const LocalVar & modelScopeObj) {
        if (it->isPunc<','>()) {
            ++it;
            parseScopeObjDefineStmt(it, defScopeObjs, modelScopeObj); // 因为有相同的公共因子序列，故直接转调用
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            
        } else {
            // 认为语句闭合，直接返回上层
            return;
        }
    }
    
    void parseScopeObjDefineStmt(TokenIter & it, std::vector<LocalVar> & defScopeObjs,
            const LocalVar & modelScopeObj) {
        LocalVar so;
        LocalVar::copySameModifierAndType(modelScopeObj, so);
        
        if (it->isId()) {
            AST_REC_LOCATION1
            so.setName(LocatedUtfString::make(it->getId(), AST_ARG_LOCATION1));
            ++it;
            if (it->isPunc<'='>()) {
                ++it;
                // 此处判断是表达式列表还是单个表达式 (赋值优先级以及赋值以上的)
                if (it->isPunc<'{'>()) {
                    // 表达式列表
                    std::vector<LocalVar::InitEntityPtr> entities;
                    parseScopeObjInitEntityList(it, entities); // 注意列表中的元素也可能是一个列表
                    so.setInitList(entities);
                    
                } else {
                    // 单个表达式
                    ExprPtr pExpr = _buildExprAssign(it);
                    so.setInitExpr(pExpr);
                }
                // 加入定义列表
                defScopeObjs.push_back(so);
                parseScopeObjDefineStmt1(it, defScopeObjs, modelScopeObj);
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                // 此处说明定义了一个列表，先保存变量，然后直接进入递归解析
                defScopeObjs.push_back(so);
                parseScopeObjDefineStmt1(it, defScopeObjs, modelScopeObj);
            }
            
        } else if (it->isEnd()) {
            AST_E(E_UNEXPECTED_EOF);
            
        } else {
            AST_E(E_ILLEGAL_TOKEN); // 报错，不递进，交由上层处理
        }
    }
    
    /*
     * DefOrPureExprStmt ->
     *  | DefType ScopeObjDefineStmt
     *  | Expr
     */
    StmtPtr buildDefOrPureExprStmt(TokenIter & it) {
        std::vector<LocalVar> defScopeObjs;
        LocalVar modelScopeObj;
        
        // 此处需要区别对待表达式和变量定义式，
        // 在需要解析变量定义式时使用 parseScopeObjDefineStmt 函数
        auto itRbk = it;
        auto cnt = _aed.getErrCount();
        
        VarType objType;
        parseDefType(it, objType); // 先假设过一遍 DefType，看看能否成功解析
        modelScopeObj.setType(objType);
        
        auto errs = _aed.getErrCount() - cnt;
        if (errs == 0) {
            // 成功预判到 DefType，接着判断:
            // 如果是 id, 则认为是变量定义语句，如果是 ';'，则返回去认定为表达式 (暂且)
            if (it->isPunc<';'>()) {
                it = itRbk; // rollback
                ExprPtr pExpr = buildExpression(it);
                return IStmt::newPureExprStmt(pExpr);
                /*
                if (it->isPunc<';'>()) {
                    // ++it; // 表达式完美闭合，但是不递进
                    // 完成，生成语句 (纯表达式)，将表达式存入作用域
                    return IStmt::newPureExprStmt(pExpr);

                } else if (it->isEnd()) {
                    AST_E(E_UNEXPECTED_EOF);

                } else {
                    AST_E(E_LOSE_SEMICOLON); // 报错，此处可能缺少分号
                }
                */
                
            } else if (it->isId()) {
                // 认定为变量定义语句，解析语句
                parseScopeObjDefineStmt(it, defScopeObjs, modelScopeObj);
                
                // 完成，生成变量定义语句，存储语句
                return IStmt::newDefineStmt(defScopeObjs);
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_LOSE_SEMICOLON); // 报错，此处可能缺少分号
            }
            
        } else {
            // 不成功，先删除错误，然后认定并解析为表达式
            _aed.clearLastErr(errs); // 删除预判过程中出现的错误
            
            // 解析为表达式
            ExprPtr pExpr = buildExpression(it);
            return IStmt::newPureExprStmt(pExpr);
            /*
            if (it->isPunc<';'>()) {
                //++it; // 表达式完美闭合，但是不递进
                // 完成，生成语句 (纯表达式)，将表达式存入作用域
                return IStmt::newPureExprStmt(pExpr);

            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);

            } else {
                AST_E(E_LOSE_SEMICOLON); // 报错，此处可能缺少分号
            }*/
        }
        return nullptr;
    }
    
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
    StmtPtr buildStmt(TokenIter & it) {
        if (it->isPunc<'{'>()) { // 解析语句块
            ++it;
            StmtsPtr pStmts = buildStmts(it);
            if (it->isPunc<'}'>()) {
                ++it; // 成功闭合语句块
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_LOSE_RBRACE);
            }
            return pStmts; // 返回语句块指针（本质与语句指针一致）
            
        } else if (it->isPunc<';'>()) { // 解析空语句
            ++it;
            return IStmt::newEmptyStmt();
            
        } else if (it->isKwFor()) { // 解析 for 语句结构
            ++it;
            if (it->isPunc<'('>()) {
                ++it;
                // 此处需要预判是否是定义式，还是表达式，还是空语句
                StmtPtr pDefStmtOrInitExpr = nullptr;
                if (it->isPunc<';'>()) { // 空语句
                    ++it;
                    
                } else if (it->isEnd()) {
                    AST_E(E_UNEXPECTED_EOF);
                    
                } else {
                    pDefStmtOrInitExpr = buildDefOrPureExprStmt(it);
                    if (it->isPunc<';'>()) {
                        ++it; // 第一个语句闭合
                        
                    } else if (it->isEnd()) {
                        AST_E(E_UNEXPECTED_EOF);
                        
                    } else {
                        AST_E(E_LOSE_SEMICOLON);
                    }
                }
                
                // 接着解析第二个语句
                StmtPtr pCondStmt = nullptr;
                if (it->isPunc<';'>()) {
                    ++it;
                    pCondStmt = IStmt::newEmptyStmt();
                } else {
                    ExprPtr pCondExpr = buildExpression(it);
                    if (it->isPunc<';'>()) {
                        ++it; // 第二个语句闭合
                        pCondStmt = IStmt::newPureExprStmt(pCondExpr);
                        
                    } else if (it->isEnd()) {
                        AST_E(E_UNEXPECTED_EOF);
                        
                    } else {
                        AST_E(E_LOSE_SEMICOLON);
                    }
                }
                
                // 接着解析第三个语句
                StmtPtr pIncreStmt = nullptr;
                if (it->isPunc<')'>()) {
                    ++it;
                    pIncreStmt = IStmt::newEmptyStmt();
                } else {
                    ExprPtr pIncreExpr = buildExpression(it);
                    if (it->isPunc<')'>()) {
                        ++it; // 第三个语句闭合
                        pIncreStmt = IStmt::newPureExprStmt(pIncreExpr);
                        
                    } else if (it->isEnd()) {
                        AST_E(E_UNEXPECTED_EOF);
                        
                    } else {
                        AST_E(E_LOSE_RP);
                    }
                }
                
                if (it->isPunc<'{'>()) {
                    ++it;
                    StmtsPtr pStmts = buildStmts(it);
                    StmtPtr pForStmt = IStmt::newForStmt(pDefStmtOrInitExpr, pCondStmt, pIncreStmt, pStmts);
                    if (it->isPunc<'}'>()) {
                        ++it; // 成功闭合
                        
                    } else if (it->isEnd()) {
                        AST_E(E_UNEXPECTED_EOF);
                        
                    } else {
                        AST_E(E_ILLEGAL_TOKEN);
                    }
                    return pForStmt;
                    
                } else if (it->isEnd()) {
                    AST_E(E_UNEXPECTED_EOF);
                    
                } else {
                    AST_E(E_ILLEGAL_TOKEN);
                }
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_ILLEGAL_TOKEN);
            }
            return IStmt::newEmptyStmt();
            
        } else if (it->isKwWhile()) { // 解析 while 语句结构
            ++it;
            if (it->isPunc<'('>()) {
                ++it; // 吸收表达式
                ExprPtr pCondExpr = buildExpression(it);
                if (it->isPunc<')'>()) {
                    ++it;
                    
                    if (it->isPunc<'{'>()) {
                        ++it;
                        StmtsPtr pStmts = buildStmts(it);
                        StmtPtr pWhileStmt = IStmt::newWhileStmt(pCondExpr, pStmts);
                        if (it->isPunc<'}'>()) {
                            ++it; // 成功闭合
                            
                        } else if (it->isEnd()) {
                            AST_E(E_UNEXPECTED_EOF);
                            
                        } else {
                            AST_E(E_ILLEGAL_TOKEN);
                        }
                        return pWhileStmt;
                        
                    } else if (it->isEnd()) {
                        AST_E(E_UNEXPECTED_EOF);
                        
                    } else {
                        AST_E(E_ILLEGAL_TOKEN);
                    }
                    
                } else if (it->isEnd()) {
                    AST_E(E_UNEXPECTED_EOF);
                    
                } else {
                    AST_E(E_LOSE_RP);
                }
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_ILLEGAL_TOKEN);
            }
            return IStmt::newEmptyStmt();
            
        } else if (it->isKwDo()) { // 解析 do 语句结构
            ++it;
            if (it->isPunc<'{'>()) {
                ++it;
                StmtsPtr pStmts = buildStmts(it);
                if (it->isPunc<'}'>()) {
                    ++it; // 成功闭合
                    if (it->isKwWhile()) {
                        ++it;
                        if (it->isPunc<'('>()) {
                            ++it; // 吸收表达式
                            ExprPtr pCondExpr = buildExpression(it);
                            if (it->isPunc<')'>()) {
                                ++it;
                                
                                StmtPtr pDoWhileStmt = IStmt::newDoWhileStmt(pStmts, pCondExpr);
                                if (it->isPunc<';'>()) {
                                    ++it;
                                    
                                } else if (it->isEnd()) {
                                    AST_E(E_UNEXPECTED_EOF);
                                    
                                } else {
                                    AST_E(E_LOSE_SEMICOLON);
                                }
                                return pDoWhileStmt;
                                
                            } else if (it->isEnd()) {
                                AST_E(E_UNEXPECTED_EOF);
                                
                            } else {
                                AST_E(E_LOSE_RP);
                            }
                            return pStmts;
                            
                        } else if (it->isEnd()) {
                            AST_E(E_UNEXPECTED_EOF);
                            
                        } else {
                            AST_E(E_ILLEGAL_TOKEN);
                        }
                        return pStmts;
                        
                    } else if (it->isEnd()) {
                        AST_E(E_UNEXPECTED_EOF);
                        
                    } else {
                        AST_E(E_ILLEGAL_TOKEN);
                    }
                    return pStmts;
                    
                } else if (it->isEnd()) {
                    AST_E(E_UNEXPECTED_EOF);
                    
                } else {
                    AST_E(E_ILLEGAL_TOKEN);
                }
                return pStmts;
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_ILLEGAL_TOKEN);
            }
            return IStmt::newEmptyStmt();
            
        } else if (it->isKwIf()) { // 解析 if 语句结构
            ++it;
            std::vector<IfCondEntry> condEntries;
            if (it->isPunc<'('>()) {
                ++it; // 吸收表达式
                ExprPtr pCondExpr = buildExpression(it);
                if (it->isPunc<')'>()) {
                    ++it;
                    
                    if (it->isPunc<'{'>()) {
                        ++it;
                        StmtsPtr pStmts = buildStmts(it);
                        IfCondEntry ce(pCondExpr, pStmts);
                        condEntries.emplace_back(pCondExpr, pStmts);
                        
                        if (it->isPunc<'}'>()) {
                            ++it;
                        } else if (it->isEnd()) {
                            AST_E(E_UNEXPECTED_EOF);
                            
                        } else {
                            AST_E(E_LOSE_RBRACE);
                        }
                        
                        // 循环吸收后部的结构
                        while (it->isKwElse()) {
                            ++it;
                            // 此处判定是 else 还是 else if
                            bool isElse; // else 为 true, else if 为 false
                            if (it->isKwIf()) { // 说明是 else if
                                ++it;
                                isElse = false;
                                if (it->isPunc<'('>()) {
                                    ++it; // 吸收表达式
                                    pCondExpr = buildExpression(it);
                                    if (it->isPunc<')'>()) {
                                        ++it;
                                    } else if (it->isEnd()) {
                                        AST_E(E_UNEXPECTED_EOF);
                                        
                                    } else {
                                        AST_E(E_LOSE_RP);
                                    }
                                    
                                } else if (it->isEnd()) {
                                    AST_E(E_UNEXPECTED_EOF);
                                    
                                } else {
                                    AST_E(E_ILLEGAL_TOKEN);
                                }
                                
                            } else { // 说明只是 else, 不递进
                                AST_REC_LOCATION1
                                pCondExpr = Expression::newLeafPure(
                                        Expression::Operator::LEAF_TRUE_LITERAL, AST_ARG_LOCATION1);
                                isElse = true;
                            }
                            
                            if (it->isPunc<'{'>()) {
                                ++it;
                                StmtsPtr pStmts = buildStmts(it);
                                condEntries.emplace_back(pCondExpr, pStmts);
                                
                                if (it->isPunc<'}'>()) {
                                    ++it;
                                } else if (it->isEnd()) {
                                    AST_E(E_UNEXPECTED_EOF);
                                    
                                } else {
                                    AST_E(E_LOSE_RBRACE);
                                }
                                
                            } else if (it->isEnd()) {
                                AST_E(E_UNEXPECTED_EOF);
                                
                            } else {
                                AST_E(E_ILLEGAL_TOKEN);
                            }
                            if (isElse) break; // 如果是 else 就跳出循环，if-else 整体结构已经闭合
                        }
                        // 说明是 if-else 结构闭合
                        return IStmt::newIfStmt(condEntries);
                        
                    } else if (it->isEnd()) {
                        AST_E(E_UNEXPECTED_EOF);
                        
                    } else {
                        AST_E(E_ILLEGAL_TOKEN);
                    }
                    
                } else if (it->isEnd()) {
                    AST_E(E_UNEXPECTED_EOF);
                    
                } else {
                    AST_E(E_LOSE_RP);
                }
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_ILLEGAL_TOKEN);
            }
            return IStmt::newEmptyStmt();
            
        } else if (it->isKwSwitch()) { // 解析 switch 语句结构
            ++it;
            std::vector<StmtPtr> stmts;
            std::vector<SwitchIndexEntry> idxEntries;
            
            if (it->isPunc<'('>()) {
                ++it;
                ExprPtr pSrcExpr = buildExpression(it);
                if (it->isPunc<')'>()) {
                    ++it;
                    
                    if (it->isPunc<'{'>()) {
                        ++it;
                        
                        while (true) { // 大循环读取标记以及普通语句(块)
                            if (it->isKwCase()) {
                                ++it;
                                // 吸收比较表达式
                                ExprPtr pObjExpr = buildExpression(it);
                                
                                // 吸收冒号 ':'
                                if (it->isPunc<':'>()) {
                                    ++it;
                                    idxEntries.emplace_back(pSrcExpr, pObjExpr, stmts.size()); // 创建跳转标记 case
                                    
                                } else if (it->isEnd()) {
                                    AST_E(E_UNEXPECTED_EOF);
                                    
                                } else {
                                    AST_E(E_LOSE_COLON);
                                }
                                
                            } else if (it->isKwDefault()) {
                                ++it;
                                // 吸收冒号 ':'
                                if (it->isPunc<':'>()) {
                                    ++it;
                                    idxEntries.emplace_back(stmts.size()); // 创建跳转标记 default
                                    
                                } else if (it->isEnd()) {
                                    AST_E(E_UNEXPECTED_EOF);
                                    
                                } else {
                                    AST_E(E_LOSE_COLON);
                                }
                                
                            } else if (it->isPunc<'}'>()) { // 预判到 switch 闭合标记
                                break; // 不递进，直接跳出循环
                                
                            } else if (it->isEnd()) {
                                AST_E(E_UNEXPECTED_EOF);
                                
                            } else {
                                StmtPtr pStmt = buildStmt(it);
                                stmts.push_back(pStmt);
                            }
                        }
                        
                        if (it->isPunc<'}'>()) {
                            ++it;
                            
                        } else if (it->isEnd()) {
                            AST_E(E_UNEXPECTED_EOF);
                            
                        } else {
                            AST_E(E_LOSE_RP);
                        }
                        
                        std::sort(idxEntries.begin(), idxEntries.end(),
                                [](const SwitchIndexEntry & a, const SwitchIndexEntry & b) {
                                    return a._stmtIdx < b._stmtIdx;
                                });
                        return IStmt::newSwitchStmt(idxEntries, stmts);
                        
                    } else if (it->isEnd()) {
                        AST_E(E_UNEXPECTED_EOF);
                        
                    } else {
                        AST_E(E_ILLEGAL_TOKEN);
                    }
                    
                } else if (it->isEnd()) {
                    AST_E(E_UNEXPECTED_EOF);
                    
                } else {
                    AST_E(E_LOSE_RP);
                }
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_ILLEGAL_TOKEN);
            }
            return IStmt::newEmptyStmt();
            
        } else if (it->isKwReturn()) { // 解析 return 语句结构
            AST_REC_LOCATION1
            ++it;
            StmtPtr pStmt = nullptr;
            ExprPtr pDummyExpr = Expression::newLeafPure(ExprOp::LEAF_ID, AST_ARG_LOCATION1);
            
            if (it->isPunc<';'>()) {
                ++it; // 不带返回值的 return 语句闭合
                
                pStmt = IStmt::newReturnStmt(pDummyExpr);
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                ExprPtr pExpr = buildExpression(it);
                pStmt = IStmt::newReturnStmt(pExpr, pDummyExpr);
                if (it->isPunc<';'>()) {
                    ++it; // 带返回值的 return 语句闭合
                    
                } else if (it->isEnd()) {
                    AST_E(E_UNEXPECTED_EOF);
                    
                } else {
                    AST_E(E_LOSE_SEMICOLON);
                }
            }
            return pStmt;
            
        } else if (it->isKwBreak()) { // 解析 break 语句结构
            AST_REC_LOCATION1
            ++it;
            
            ExprPtr pDummyExpr = Expression::newLeafPure(ExprOp::LEAF_ID, AST_ARG_LOCATION1);
            StmtPtr pStmt = IStmt::newBreakStmt(pDummyExpr);
            if (it->isPunc<';'>()) {
                ++it; // 语句闭合
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_LOSE_SEMICOLON);
            }
            return pStmt;
            
        } else if (it->isKwContinue()) { // 解析 continue 语句
            AST_REC_LOCATION1
            ++it;
            
            ExprPtr pDummyExpr = Expression::newLeafPure(ExprOp::LEAF_ID, AST_ARG_LOCATION1);
            StmtPtr pStmt = IStmt::newContinueStmt(pDummyExpr);
            if (it->isPunc<';'>()) {
                ++it; // 语句闭合
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                AST_E(E_LOSE_SEMICOLON);
            }
            return pStmt;
            
        } else {
            StmtPtr pStmt = buildDefOrPureExprStmt(it);
            if (it->isPunc<';'>()) {
                ++it; // 语句闭合
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                ++it;
                AST_E(E_LOSE_SEMICOLON);
            }
            return pStmt;
        }
    }
    
    StmtsPtr buildStmts(TokenIter & it) {
        std::vector<StmtPtr> stmts;
        while (true) {
            if (it->isPunc<'}'>()) { // 不吸收右花括号
                return IStmt::newStmts(stmts);
                
            } else if (it->isEnd()) {
                AST_E(E_UNEXPECTED_EOF);
                
            } else {
                StmtPtr pStmt = buildStmt(it);
                stmts.push_back(pStmt);
            }
        }
    }
};

#endif
