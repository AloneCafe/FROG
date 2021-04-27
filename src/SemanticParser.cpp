#include "SemanticParser.hpp"


SemParser::ExprTypeConstraint::ChkResult
SemParser::ExprTypeConstraint::_check(const VarType & demand, const VarType & choice) {
    if (demand == choice) {
        return ChkResult::CHK_OK;
    } else {
        if (demand == "void") { // accept multi-degree
            if (choice == "double" || choice == "float" || choice == "long" || choice == "int" ||
                    choice == "short" || choice == "char" || choice == "byte") {
                return ChkResult::CHK_NEED_IC;
            }
            
        } else if (demand == "double") {
            if (choice == "float" || choice == "long" || choice == "int" ||
                    choice == "short" || choice == "char" || choice == "byte") {
                return ChkResult::CHK_NEED_IC;
            }
        } else if (demand == "float") {
            if (choice == "long" || choice == "int" ||
                    choice == "short" || choice == "char" || choice == "byte") {
                return ChkResult::CHK_NEED_IC;
            }
        } else if (demand == "long") {
            if (choice == "int" || choice == "short" || choice == "char" || choice == "byte") {
                return ChkResult::CHK_NEED_IC;
            }
        } else if (demand == "int") {
            if (choice == "short" || choice == "char" || choice == "byte") {
                return ChkResult::CHK_NEED_IC;
            }
        } else if (demand == "short") {
            if (choice == "byte" || choice == "char") {
                return ChkResult::CHK_NEED_IC;
            }
        } else if (demand == "char") {
            if (choice == "byte") {
                return ChkResult::CHK_NEED_IC;
            }
        } else if (demand == "byte") {
            if (choice == "char") {
                return ChkResult::CHK_NEED_IC;
            }
        } else if (demand == "boolean") {
            if (choice == "boolean") {
                return ChkResult::CHK_OK;
            }
        }
    }
    return ChkResult::CHK_INCMP;
}


const char *
SemParser::scalarVarType2VMStr(const VarType & choice) {
    if (choice == "long") {
        return "QW";
    } else if (choice == "int") {
        return "DW";
    } else if (choice == "short") {
        return "W";
    } else if (choice == "byte" || choice == "boolean" || choice == "char" ) {
        return "B";
    } else if (choice == "float") {
        return "FLT";
    } else if (choice == "double") {
        return "DBL";
    } else if (choice.getDegree() > 0) {
        return "DW";
    } else {
        //assert(0); // 一般不会到此
        //return nullptr;
        return "VOID";
    }
}

bool SemParser::checkMultiTypeMatching(
        const ExprPtr & pExpr,
        const std::vector<VarType> & orderedTypeMatchList,
        VarType & okChoice)
{
    size_t sizList = orderedTypeMatchList.size();
    for (size_t i = 0; i < sizList; ++i) {
        const auto & typeMatch = orderedTypeMatchList[i];
        SEM_E_COUNT_RECOND;
        VarType fromChoice = gen4expr<OnlyChk>(pExpr, typeMatch);
        if (SEM_E_COUNT_HAS_INCREASED) {
            if (i != sizList - 1) {
                SEM_E_COUNT_CLEAR_DELTA;
            }
            continue;
        } else {
            okChoice = typeMatch;
            return true;
        }
    }
    return false;
}

int SemParser::getScalarTypeSize(const VarType & choice) const {
    if (choice == "long") {
        return 8;
    } else if (choice == "int") {
        return 4;
    } else if (choice == "short") {
        return 2;
    } else if (choice == "byte" || choice == "boolean" || choice == "char") {
        return 1;
    } else if (choice == "float") {
        return 4;
    } else if (choice == "double") {
        return 8;
    } else if (choice.getDegree() > 0) {
        return 4;
    } else {
        assert(0); // 一般不会到此
        return 0;
    }
}

bool SemParser::check2ScalarTypeBigger(const VarType & choice1, const VarType & choice2, int & choiceIdx) {
    static const std::map<std::string, int> typeCompare = {
            std::make_pair("byte", 0),
            std::make_pair("boolean", 0),
            std::make_pair("char", 0),
            std::make_pair("short", 1),
            std::make_pair("int", 2),
            std::make_pair("long", 3),
            std::make_pair("float", 4),
            std::make_pair("double", 5),
            std::make_pair("void", 6)
    };
    
    std::map<std::string, int>::const_iterator it1, it2;
    
    if (((it1 = typeCompare.find(choice1.toString())) != typeCompare.cend())
            && ((it2 = typeCompare.find(choice2.toString())) != typeCompare.cend())) {
        int c1 = it1->second;
        int c2 = it2->second;
        if (c1 > c2) {
            choiceIdx = 0;
        } else if (c1 == c2) {
            choiceIdx = -1;
        } else {
            choiceIdx = 1;
        }
        return true;
    } else {
        return false;
    }
}

void SemParser::gen4VarDefine(const GlobalVar & gvar) {
    const VarType & varType = gvar.getType();
    const VarName & varName = gvar.getName();
    
    bool result = _saBinder.bindDef(varName, varType);
    if (!result) {
        SEM_E(E_VAR_REDEFINED, varName);
        return;
    }
    
    std::tuple<bool, VarName, VarType> tuple =
            _saBinder.getDef(gvar.getName());
    assert(std::get<0>(tuple));
    const std::string realNameStr = std::get<1>(tuple).toString();
    const LocatedUtfString & lowLvType = varType.getLowLvType();
    int degree = 0;
    
    if (varType == "long") {
        _asmk.append_DEF_VAR_QW(realNameStr);
    } else if (varType == "int") {
        _asmk.append_DEF_VAR_DW(realNameStr);
    } else if (varType == "short") {
        _asmk.append_DEF_VAR_W(realNameStr);
    } else if (varType == "byte" || varType == "boolean" || varType == "char") {
        _asmk.append_DEF_VAR_B(realNameStr);
    } else if (varType == "float") {
        _asmk.append_DEF_VAR_FLT(realNameStr);
    } else if (varType == "double") {
        _asmk.append_DEF_VAR_DBL(realNameStr);
    } else if ((degree = varType.getDegree()) > 0) {
        _asmk.append_DEF_VAR_DW(realNameStr);
        if (lowLvType == "long") {
            _asmk.append_MKVEC_N_QW(degree);
        } else if (lowLvType == "int") {
            _asmk.append_MKVEC_N_DW(degree);
        } else if (lowLvType == "short") {
            _asmk.append_MKVEC_N_W(degree);
        } else if (lowLvType == "byte" || lowLvType == "boolean" || lowLvType == "char") {
            _asmk.append_MKVEC_N_B(degree);
        } else if (lowLvType == "float") {
            _asmk.append_MKVEC_N_FLT(degree);
        } else if (lowLvType == "double") {
            _asmk.append_MKVEC_N_DBL(degree);
        } else {
            assert(0);
        } // 执行创建 list 的命令
        
        _asmk.append_POP_VAR_DW(realNameStr); // 赋予引用
        /*
        const VarName & varName = std::get<1>(tuple);
        const VarType & varType = std::get<2>(tuple);
        if (varType.getDegree() > 0) {
            _asmk.append_POP_VAR_DW(realNameStr);
        } else if (varType == "int") {
            _asmk.append_POP_VAR_DW(realNameStr);
        } else if (varType == "long") {
            _asmk.append_POP_VAR_QW(realNameStr);
        } else if (varType == "float") {
            _asmk.append_POP_VAR_FLT(realNameStr);
        } else if (varType == "double") {
            _asmk.append_POP_VAR_DBL(realNameStr);
        } else if (varType == "short") {
            _asmk.append_POP_VAR_W(realNameStr);
        } else if (varType == "char" || varType == "byte" || varType == "boolean") {
            _asmk.append_POP_VAR_B(realNameStr);
        } else {
            assert(0);
        }*/
        
    } else {
        assert(0);
    }
    
    // 检查是否具备初始化表达式，如果具备，就生成初始化代码
    if (!gvar.hasInit())
        return;
    
    if (degree > 0 && gvar.isListInit()) {
        // 暂不支持表达式列表
        //SEM_E(E_UNSUPPORTED_LIST_INIT, defVar.getName());
        std::vector<int> linearOffsets;
        
        // 取出列表
        degree -= 1;
        const std::vector<LocalVar::InitEntityPtr> & initList =
                gvar.getInitListNativePtr()->_entities;
        size_t sizInitList = initList.size();
        for (size_t i = 0; i < sizInitList; ++i) {
            const LocalVar::InitEntityPtr & pInitEntity = initList[i];
            LocalVar::InitEntityType iet = pInitEntity->getEntityType();
            if (iet == LocalVar::InitEntityType::Expr) {
                LocalVar::InitExpr * pNativeInitExpr =
                        static_cast<LocalVar::InitExpr *>(pInitEntity.get());
                if (degree > 0) {
                    // 此处判断是不是当前是1维char，然后接着判断是不是一个纯 NSTRING 字面量表达式
                    if (degree == 1 && lowLvType == "char" &&
                            pNativeInitExpr->_pExpr->getOp() == ExprOp::LEAF_STRING_LITERAL)
                    {
                        linearOffsets.push_back(i);
                        const std::string & lstr = pNativeInitExpr->_pExpr->getLeafScalar()._str;
                        std::vector<char> vstr;
                        std::for_each(lstr.cbegin(), lstr.cend(), [&](const char & ch){
                            vstr.push_back(ch);
                        });
                        vstr.push_back(0);
                        
                        int count = 0;
                        std::for_each(vstr.cbegin(), vstr.cend(), [&](const char & ch){
                            size_t sizLinearOffsets = linearOffsets.size();
                            //_asmk.append_PUSH_VAR(realNameStr);
                            if (varType == "long") {
                                _asmk.append_PUSH_VAR_QW(realNameStr);
                            } else if (varType == "int") {
                                _asmk.append_PUSH_VAR_DW(realNameStr);
                            } else if (varType == "short") {
                                _asmk.append_PUSH_VAR_W(realNameStr);
                            } else if (varType == "byte" || varType == "boolean" || varType == "char") {
                                _asmk.append_PUSH_VAR_B(realNameStr);
                            } else if (varType == "float") {
                                _asmk.append_PUSH_VAR_FLT(realNameStr);
                            } else if (varType == "double") {
                                _asmk.append_PUSH_VAR_DBL(realNameStr);
                            } else if (varType.getDegree() > 0) {
                                _asmk.append_PUSH_VAR_DW(realNameStr);
                            } else {
                                assert(0);
                            }
                            
                            _asmk.append_IPUSH_DW(linearOffsets[0]);
                            _asmk.append_OFFSET();
                            for (size_t i = 1; i < sizLinearOffsets; ++i) {
                                _asmk.append_HPUSH_DW();
                                _asmk.append_IPUSH_DW(linearOffsets[i]);
                                _asmk.append_OFFSET();
                            }
                            _asmk.append_HPUSH_DW();
                            _asmk.append_IPUSH_DW(count++);
                            _asmk.append_OFFSET();
                            _asmk.append_IPUSH_B(ch);
                            _asmk.append_HPOP_B();
                        });
                        linearOffsets.pop_back();
                        continue;
                        
                    } else {
                        SEM_E(E_ILLEGAL_INIT_STMT, gvar.getName());
                        return;
                    }
                }
                //_asmk.append_PUSH_VAR(realNameStr);
                if (varType == "long") {
                    _asmk.append_PUSH_VAR_QW(realNameStr);
                } else if (varType == "int") {
                    _asmk.append_PUSH_VAR_DW(realNameStr);
                } else if (varType == "short") {
                    _asmk.append_PUSH_VAR_W(realNameStr);
                } else if (varType == "byte" || varType == "boolean" || varType == "char") {
                    _asmk.append_PUSH_VAR_B(realNameStr);
                } else if (varType == "float") {
                    _asmk.append_PUSH_VAR_FLT(realNameStr);
                } else if (varType == "double") {
                    _asmk.append_PUSH_VAR_DBL(realNameStr);
                } else if (varType.getDegree() > 0) {
                    _asmk.append_PUSH_VAR_DW(realNameStr);
                } else {
                    assert(0);
                }
                
                _asmk.append_IPUSH_DW(i);
                _asmk.append_OFFSET();
                gen4expr<OnlyGen>(pNativeInitExpr->_pExpr, lowLvType.toString());
                if (lowLvType == "long") {
                    _asmk.append_HPOP_QW();
                } else if (lowLvType == "int") {
                    _asmk.append_HPOP_DW();
                } else if (lowLvType == "short") {
                    _asmk.append_HPOP_W();
                } else if (lowLvType == "byte" || lowLvType == "boolean" || lowLvType == "char") {
                    _asmk.append_HPOP_B();
                } else if (lowLvType == "float") {
                    _asmk.append_HPOP_FLT();
                } else if (lowLvType == "double") {
                    _asmk.append_HPOP_DBL();
                } else {
                    assert(0);
                }
                
            } else if (iet == LocalVar::InitEntityType::List) {
                if (degree == 0) {
                    SEM_E(E_ILLEGAL_INIT_STMT, gvar.getName());
                    return;
                }
                //_asmk.append_PUSH_VAR(realNameStr);
                //_asmk.append_IPUSH_DW(i);
                //_asmk.append_OFFSET();
                linearOffsets.push_back(i);
                
                LocalVar::InitList * pInitList =
                        static_cast<LocalVar::InitList *>(pInitEntity.get());
                
                static std::function<bool(const std::vector<LocalVar::InitEntityPtr> &, int)> fRecursion =
                        [&](const std::vector<LocalVar::InitEntityPtr> & initList, int deg) -> bool {
                            deg -= 1;
                            size_t sizInitList = initList.size();
                            for (size_t i = 0; i < sizInitList; ++i) {
                                const LocalVar::InitEntityPtr & pInitEntity = initList[i];
                                LocalVar::InitEntityType iet = pInitEntity->getEntityType();
                                if (iet == LocalVar::InitEntityType::Expr) {
                                    LocalVar::InitExpr * pNativeInitExpr =
                                            static_cast<LocalVar::InitExpr *>(pInitEntity.get());
                                    if (deg > 0) {
                                        // 此处判断是不是当前是1维char，然后接着判断是不是一个纯 NSTRING 字面量表达式
                                        if (deg == 1 && lowLvType == "char" &&
                                                pNativeInitExpr->_pExpr->getOp() == ExprOp::LEAF_STRING_LITERAL)
                                        {
                                            linearOffsets.push_back(i);
                                            const std::string & lstr = pNativeInitExpr->_pExpr->getLeafScalar()._str;
                                            std::vector<char> vstr;
                                            std::for_each(lstr.cbegin(), lstr.cend(), [&](const char & ch){
                                                vstr.push_back(ch);
                                            });
                                            vstr.push_back(0);
                                            
                                            int count = 0;
                                            std::for_each(vstr.cbegin(), vstr.cend(), [&](const char & ch){
                                                size_t sizLinearOffsets = linearOffsets.size();
                                                //_asmk.append_PUSH_VAR(realNameStr);
                                                if (varType == "long") {
                                                    _asmk.append_PUSH_VAR_QW(realNameStr);
                                                } else if (varType == "int") {
                                                    _asmk.append_PUSH_VAR_DW(realNameStr);
                                                } else if (varType == "short") {
                                                    _asmk.append_PUSH_VAR_W(realNameStr);
                                                } else if (varType == "byte" || varType == "boolean" || varType == "char") {
                                                    _asmk.append_PUSH_VAR_B(realNameStr);
                                                } else if (varType == "float") {
                                                    _asmk.append_PUSH_VAR_FLT(realNameStr);
                                                } else if (varType == "double") {
                                                    _asmk.append_PUSH_VAR_DBL(realNameStr);
                                                } else if (varType.getDegree() > 0) {
                                                    _asmk.append_PUSH_VAR_DW(realNameStr);
                                                } else {
                                                    assert(0);
                                                }
                                                
                                                _asmk.append_IPUSH_DW(linearOffsets[0]);
                                                _asmk.append_OFFSET();
                                                for (size_t i = 1; i < sizLinearOffsets; ++i) {
                                                    _asmk.append_HPUSH_DW();
                                                    _asmk.append_IPUSH_DW(linearOffsets[i]);
                                                    _asmk.append_OFFSET();
                                                }
                                                _asmk.append_HPUSH_DW();
                                                _asmk.append_IPUSH_DW(count++);
                                                _asmk.append_OFFSET();
                                                _asmk.append_IPUSH_B(ch);
                                                _asmk.append_HPOP_B();
                                            });
                                            linearOffsets.pop_back();
                                            continue;
                                            
                                        } else {
                                            
                                            size_t sizLinearOffsets = linearOffsets.size();
                                            _asmk.append_IPUSH_DW(linearOffsets[0]);
                                            _asmk.append_OFFSET();
                                            for (size_t i = 1; i < sizLinearOffsets; ++i) {
                                                _asmk.append_HPUSH_DW();
                                                _asmk.append_IPUSH_DW(linearOffsets[i]);
                                                _asmk.append_OFFSET();
                                            }
                                            _asmk.append_HPUSH_DW();
                                            gen4expr<OnlyGen>(pNativeInitExpr->_pExpr, varType);
                                            _asmk.append_HPOP_DW();
                                            return true;
                                            
                                            //SEM_E(E_ILLEGAL_INIT_STMT, gvar.getName());
                                            //return false;
                                        }
                                    }
                                    //_asmk.append_DEREF();
                                    //_asmk.append_IPUSH_DW(i);
                                    //_asmk.append_OFFSET();
                                    linearOffsets.push_back(i);
                                    
                                    size_t sizLinearOffsets = linearOffsets.size();
                                    //_asmk.append_PUSH_VAR(realNameStr);
                                    if (varType == "long") {
                                        _asmk.append_PUSH_VAR_QW(realNameStr);
                                    } else if (varType == "int") {
                                        _asmk.append_PUSH_VAR_DW(realNameStr);
                                    } else if (varType == "short") {
                                        _asmk.append_PUSH_VAR_W(realNameStr);
                                    } else if (varType == "byte" || varType == "boolean" || varType == "char") {
                                        _asmk.append_PUSH_VAR_B(realNameStr);
                                    } else if (varType == "float") {
                                        _asmk.append_PUSH_VAR_FLT(realNameStr);
                                    } else if (varType == "double") {
                                        _asmk.append_PUSH_VAR_DBL(realNameStr);
                                    } else if (varType.getDegree() > 0) {
                                        _asmk.append_PUSH_VAR_DW(realNameStr);
                                    } else {
                                        assert(0);
                                    }
                                    
                                    _asmk.append_IPUSH_DW(linearOffsets[0]);
                                    _asmk.append_OFFSET();
                                    for (size_t i = 1; i < sizLinearOffsets; ++i) {
                                        _asmk.append_HPUSH_DW();
                                        _asmk.append_IPUSH_DW(linearOffsets[i]);
                                        _asmk.append_OFFSET();
                                    }
                                    
                                    gen4expr<OnlyGen>(pNativeInitExpr->_pExpr, lowLvType.toString());
                                    if (lowLvType == "long") {
                                        _asmk.append_HPOP_QW();
                                    } else if (lowLvType == "int") {
                                        _asmk.append_HPOP_DW();
                                    } else if (lowLvType == "short") {
                                        _asmk.append_HPOP_W();
                                    } else if (lowLvType == "byte" || lowLvType == "boolean" || lowLvType == "char") {
                                        _asmk.append_HPOP_B();
                                    } else if (lowLvType == "float") {
                                        _asmk.append_HPOP_FLT();
                                    } else if (lowLvType == "double") {
                                        _asmk.append_HPOP_DBL();
                                    } else {
                                        assert(0);
                                    }
                                    
                                    linearOffsets.pop_back();
                                    
                                } else if (iet == LocalVar::InitEntityType::List) {
                                    LocalVar::InitList * pInitList =
                                            static_cast<LocalVar::InitList *>(pInitEntity.get());
                                    if (deg == 0) {
                                        SEM_E(E_ILLEGAL_INIT_STMT, gvar.getName());
                                        return false;
                                    }
                                    
                                    //_asmk.append_DEREF();
                                    //_asmk.append_IPUSH_DW(i);
                                    //_asmk.append_OFFSET();
                                    linearOffsets.push_back(i);
                                    fRecursion(pInitList->_entities, deg);
                                    linearOffsets.pop_back();
                                    
                                } else {
                                    assert(0);
                                }
                            }
                            return true;
                        };
                
                fRecursion(pInitList->_entities, degree);
                
                linearOffsets.pop_back();
                
            } else {
                assert(0);
            }
            
        }
        
    } else if (degree == 0) {
        // 单个表达式
        TokenValue tv;
        tv._str = gvar.getName().toString();
        ExprPtr pExprLeafId = Expr::newLeafScalar(ExprOp::LEAF_ID, tv,
                gvar.getName().lineno(), gvar.getName().colno());
        ExprPtr pNewAssignExpr =
                Expr::newExpr(ExprOp::OPT_ASSIGN, {pExprLeafId, gvar.getInitExprNativePtr()->_pExpr },
                        gvar.getName().lineno(), gvar.getName().colno());
        gen4expr<OnlyGen>(pNewAssignExpr, "void");
        
    } else {
        LocalVar::InitExpr * pNativeInitExpr = gvar.getInitExprNativePtr();
        if (degree == 1 && lowLvType == "char" &&
                pNativeInitExpr->_pExpr->getOp() == ExprOp::LEAF_STRING_LITERAL)
        {
            const std::string & lstr = pNativeInitExpr->_pExpr->getLeafScalar()._str;
            std::vector<char> vstr;
            std::for_each(lstr.cbegin(), lstr.cend(), [&](const char & ch){
                vstr.push_back(ch);
            });
            vstr.push_back(0);
            
            int count = 0;
            std::for_each(vstr.cbegin(), vstr.cend(), [&](const char & ch){
                //_asmk.append_PUSH_VAR(realNameStr);
                if (varType == "long") {
                    _asmk.append_PUSH_VAR_QW(realNameStr);
                } else if (varType == "int") {
                    _asmk.append_PUSH_VAR_DW(realNameStr);
                } else if (varType == "short") {
                    _asmk.append_PUSH_VAR_W(realNameStr);
                } else if (varType == "byte" || varType == "boolean" || varType == "char") {
                    _asmk.append_PUSH_VAR_B(realNameStr);
                } else if (varType == "float") {
                    _asmk.append_PUSH_VAR_FLT(realNameStr);
                } else if (varType == "double") {
                    _asmk.append_PUSH_VAR_DBL(realNameStr);
                } else if (varType.getDegree() > 0) {
                    _asmk.append_PUSH_VAR_DW(realNameStr);
                } else {
                    assert(0);
                }
                
                _asmk.append_IPUSH_DW(count++);
                _asmk.append_OFFSET();
                _asmk.append_IPUSH_B(ch);
                _asmk.append_HPOP_B();
            });
            return;
            
        } else {
            //SEM_E(E_ILLEGAL_INIT_STMT, gvar.getName());
            gen4expr<OnlyGen>(pNativeInitExpr->_pExpr, varType);
            _asmk.append_POP_VAR_DW(realNameStr);
            
            return;
        }
    }
    
}

void SemParser::gen4Stmts(const Function & fun, std::vector<StmtPtr> & stmts) {
    for (const auto & pStmt : stmts) {
        // 分发、解析每个语句(块)
        switch (pStmt->getStmtType()) {
        case StmtType::PureExpr: {
            PureExprStmt * pNativePureExprStmt =
                    static_cast<PureExprStmt *>(pStmt.get());
            gen4expr<OnlyGen>(pNativePureExprStmt->_pExpr, "void");
            break;
        }
        
        case StmtType::Define: {
            DefineStmt * pNativeDefineStmt =
                    static_cast<DefineStmt *>(pStmt.get());
            for (const LocalVar & defVar :
                    pNativeDefineStmt->getDefScopeObjs()) {
                gen4VarDefine(defVar);
            }
            
            break;
        }
        
        case StmtType::For: {
            ForStmt * pNativeForStmt =
                    static_cast<ForStmt *>(pStmt.get());
            LabelGroup::For lgFor = AdvGuidGen::applyLabelGroup4For();
            
            _saBinder.makeScope(&lgFor);
            
            StmtPtr pFirstStmt = pNativeForStmt->getFirstStmt();   // 第一个语句不是赋值表达式就是定义语句
            StmtPtr pSecondStmt = pNativeForStmt->getSecondStmt(); // 第二个语句一定是纯表达式
            StmtPtr pThirdStmt = pNativeForStmt->getThirdStmt();   // 第三个语句一定是纯表达式
            StmtPtr pBodyStmts = pNativeForStmt->_pStmts;
            
            StmtType firstStmtType = pFirstStmt->getStmtType(),
                    secondStmtType = pSecondStmt->getStmtType(),
                    thirdStmtType = pThirdStmt->getStmtType(),
                    bodyStmtsType = pBodyStmts->getStmtType();
            
            // init
            _asmk.append_LABEL(lgFor._lbl_for_begin);
            if (firstStmtType == StmtType::Define || firstStmtType == StmtType::PureExpr) {
                std::vector<StmtPtr> fakeStmts = { pFirstStmt };
                gen4Stmts(fun, fakeStmts);
            } else {
                assert(0);
            }
            
            // judge
            _asmk.append_LABEL(lgFor._lbl_for_judge);
            if (secondStmtType == StmtType::PureExpr) {
                PureExprStmt * pNativeSecondStmt =
                        static_cast<PureExprStmt *>(pSecondStmt.get());
                gen4expr<OnlyGen>(pNativeSecondStmt->_pExpr, "boolean");
            } else {
                assert(0);
            }
            _asmk.append_JF(lgFor._lbl_for_end_break);
            
            // body
            if (bodyStmtsType == StmtType::Stmts) {
                Stmts * pNativeBodyStmts =
                        static_cast<Stmts *>(pBodyStmts.get());
                gen4Stmts(fun, pNativeBodyStmts->_stmts);
            } else {
                assert(0);
            }
            
            // continue
            _asmk.append_LABEL(lgFor._lbl_for_continue);
            if (thirdStmtType == StmtType::PureExpr) {
                std::vector<StmtPtr> fakeStmts = { pThirdStmt };
                gen4Stmts(fun, fakeStmts);
            } else {
                assert(0);
            }
            _asmk.append_J(lgFor._lbl_for_judge);
            
            _asmk.append_LABEL(lgFor._lbl_for_end_break);
            _saBinder.clearScope();
            break;
        }
        
        case StmtType::Return: {
            ReturnStmt * pNativeReturnStmt =
                    static_cast<ReturnStmt *>(pStmt.get());
            const VarType & retType = fun.getRetType();
            if (retType == "void") {
                if (pNativeReturnStmt->_pRetExpr) {
                    SEM_E(E_FUNC_HAS_NO_RET_VALUE, pNativeReturnStmt->_pRetExpr);
                    
                } else {
                    _asmk.append_NRET();
                }
            } else {
                if (!pNativeReturnStmt->_pRetExpr) {
                    SEM_E(E_FUNC_NEED_RET_VALUE, pNativeReturnStmt->getDummyExpr());
                    
                } else {
                    
                    gen4expr<OnlyGen>(pNativeReturnStmt->_pRetExpr, retType);
                    if (retType == "long") {
                        _asmk.append_RET_QW();
                    } else if (retType == "int") {
                        _asmk.append_RET_DW();
                    } else if (retType == "short") {
                        _asmk.append_RET_W();
                    } else if (retType == "byte" || retType == "boolean" || retType == "char") {
                        _asmk.append_RET_B();
                    } else if (retType == "float") {
                        _asmk.append_RET_FLT();
                    } else if (retType == "double") {
                        _asmk.append_RET_DBL();
                    } else if (retType.getDegree() > 0) {
                        _asmk.append_RET_DW();
                    } else {
                        SEM_E(E_ILLEGAL_RET_TYPE, pNativeReturnStmt->_pRetExpr);
                    }
                }
            }
            break;
        }
        
        case StmtType::While: {
            WhileStmt * pNativeWhileStmt =
                    static_cast<WhileStmt *>(pStmt.get());
            LabelGroup::While lgWhile = AdvGuidGen::applyLabelGroup4While();
            
            _saBinder.makeScope(&lgWhile);
            
            _asmk.append_LABEL(lgWhile._lbl_while_begin_judge_continue);
            gen4expr<OnlyGen>(pNativeWhileStmt->_pCondExpr, "boolean");
            _asmk.append_JF(lgWhile._lbl_while_end_break);
            
            StmtsPtr pBodyStmts = pNativeWhileStmt->_pStmts;
            StmtType bodyStmtsType = pBodyStmts->getStmtType();
            // body
            if (bodyStmtsType == StmtType::Stmts) {
                Stmts * pNativeBodyStmts =
                        static_cast<Stmts *>(pBodyStmts.get());
                gen4Stmts(fun, pNativeBodyStmts->_stmts);
            } else {
                assert(0);
            }
            
            _asmk.append_J(lgWhile._lbl_while_begin_judge_continue);
            
            _asmk.append_LABEL(lgWhile._lbl_while_end_break);
            _saBinder.clearScope();
            break;
        }
        
        case StmtType::DoWhile: {
            DoWhileStmt * pNativeDoWhileStmt =
                    static_cast<DoWhileStmt *>(pStmt.get());
            LabelGroup::DoWhile lgDoWhile = AdvGuidGen::applyLabelGroup4DoWhile();
            
            _saBinder.makeScope(&lgDoWhile);
            
            _asmk.append_LABEL(lgDoWhile._lbl_do_while_begin);
            
            StmtsPtr pBodyStmts = pNativeDoWhileStmt->_pStmts;
            StmtType bodyStmtsType = pBodyStmts->getStmtType();
            // body
            if (bodyStmtsType == StmtType::Stmts) {
                Stmts * pNativeBodyStmts =
                        static_cast<Stmts *>(pBodyStmts.get());
                gen4Stmts(fun, pNativeBodyStmts->_stmts);
            } else {
                assert(0);
            }
            
            _asmk.append_LABEL(lgDoWhile._lbl_do_while_judge_continue);
            gen4expr<OnlyGen>(pNativeDoWhileStmt->_pCondExpr, "boolean");
            _asmk.append_JF(lgDoWhile._lbl_do_while_end_break);
            
            _asmk.append_J(lgDoWhile._lbl_do_while_begin);
            
            _asmk.append_LABEL(lgDoWhile._lbl_do_while_end_break);
            _saBinder.clearScope();
            break;
        }
        
        case StmtType::If: {
            IfStmt * pNativeIfStmt =
                    static_cast<IfStmt *>(pStmt.get());
            LabelGroup::If lgIf =
                    AdvGuidGen::applyLabelGroup4If(pNativeIfStmt->_condEntries);
            _asmk.append_LABEL(lgIf._lbl_if_begin);
            
            size_t i = 0, sz = pNativeIfStmt->_condEntries.size();
            for (const IfCondEntry & ice : pNativeIfStmt->_condEntries) {
                _asmk.append_LABEL(lgIf._lbl_if_casenodes[i]);
                
                gen4expr<OnlyGen>(ice._pCondExpr, "boolean");
                if (i + 1 < sz)
                    _asmk.append_JF(lgIf._lbl_if_casenodes[i + 1]);
                else
                    _asmk.append_JF(lgIf._lbl_if_end);
                
                StmtsPtr pBodyStmts = ice._pStmts;
                StmtType bodyStmtsType = pBodyStmts->getStmtType();
                // body
                if (bodyStmtsType == StmtType::Stmts) {
                    Stmts * pNativeBodyStmts =
                            static_cast<Stmts *>(pBodyStmts.get());
                    _saBinder.makeScope();
                    gen4Stmts(fun, pNativeBodyStmts->_stmts);
                    _saBinder.clearScope();
                } else {
                    assert(0);
                }
                
                _asmk.append_J(lgIf._lbl_if_end);
                ++i;
            }
            
            _asmk.append_LABEL(lgIf._lbl_if_end);
            break;
        }
        
        case StmtType::Switch: {
            SwitchStmt *pNativeSwitchStmt =
                    static_cast<SwitchStmt *>(pStmt.get());
            assert(pNativeSwitchStmt->_pStmts->getStmtType() == StmtType::Stmts);
            const std::vector<StmtPtr> & stmts =
                    static_cast<Stmts *>(pNativeSwitchStmt->_pStmts.get())->_stmts;
            std::vector<SwitchIndexEntry> & idxEntries =
                    pNativeSwitchStmt->_idxEntries;
            
            LabelGroup::Switch lgSwitch = AdvGuidGen::applyLabelGroup4Switch(idxEntries);
            _asmk.append_LABEL(lgSwitch._lbl_switch_begin);
            
            bool flagHasDefault = false;
            size_t iDefault;
            size_t sizIdxEntries = idxEntries.size();
            for (size_t i = 0; i < sizIdxEntries; ++i) {
                const SwitchIndexEntry & sie = idxEntries[i];
                if (sie._type == SwitchIndexEntry::MarkType::Case) {
                    gen4expr<OnlyGen>(sie._pCondExpr, "boolean");
                    _asmk.append_JT(lgSwitch._lbl_switch_casenodes[i]);
                } else {
                    // save
                    flagHasDefault = true;
                    iDefault = i;
                }
            }
            if (flagHasDefault) {
                _asmk.append_J(lgSwitch._lbl_switch_casenodes[iDefault]);
            } else {
                _asmk.append_J(lgSwitch._lbl_switch_end_break);
            }
            
            _saBinder.makeScope(&lgSwitch);
            size_t sizStmts = stmts.size(); // stmts
            size_t ie = 0; // idxEntries
            for (size_t is = 0; is < sizStmts; ++is) {
                // 可能存在空 case, 所以此处用 while
                while (ie < sizIdxEntries && is == idxEntries[ie]._stmtIdx) {
                    _asmk.append_LABEL(lgSwitch._lbl_switch_casenodes[ie]);
                    ++ie;
                }
                std::vector<StmtPtr> fakeStmts = { stmts[is] };
                gen4Stmts(fun, fakeStmts); // fake
            }
            _saBinder.clearScope();
            
            _asmk.append_LABEL(lgSwitch._lbl_switch_end_break);
            break;
        }
        
        
        case StmtType::Break: {
            const std::string * pLabel = _saBinder.getNearestBreak();
            if (pLabel) {
                _asmk.append_J(*pLabel);
            } else {
                SEM_E(E_IILEGAL_BREAK, pStmt->getDummyExpr());
            }
            break;
        }
        
        case StmtType::Continue: {
            const std::string * pLabel = _saBinder.getNearestBreak();
            if (pLabel) {
                _asmk.append_J(*pLabel);
            } else {
                SEM_E(E_IILEGAL_CONTINUE, pStmt->getDummyExpr());
            }
            break;
        }
        
        case StmtType::Stmts: {
            Stmts * pNativeStmts =
                    static_cast<Stmts *>(pStmt.get());
            _saBinder.makeScope();
            gen4Stmts(fun, pNativeStmts->_stmts);
            _saBinder.clearScope();
            break;
        }
        
        case StmtType::InlineASM: {
            InlineASM * pNativeInlineASM =
                    static_cast<InlineASM *>(pStmt.get());
            
            const std::string & asmstr = pNativeInlineASM->_asmcodes;
            size_t sizAsmstr = asmstr.size();
            size_t beg = 0, end = 0;
            int beginResign = 0;
            std::string resignVarName;
            for (size_t i = 0; i < sizAsmstr; ++i) {
                if (beginResign == 1) {
                    if (isblank(asmstr[i])) {
                    
                    } else {
                        beginResign = 2;
                        resignVarName += asmstr[i];
                    }
                    
                } else if (beginResign == 2) {
                    if (isalpha(asmstr[i]) || isdigit(asmstr[i]) || asmstr[i] == '_') {
                        resignVarName += asmstr[i];
                        
                    } else {
                        beginResign = 0;
                        
                        std::tuple<bool, VarName, VarType> tuple =
                                _saBinder.getDef(LocatedUtfString::make(resignVarName, -1, -1));
                        if (!std::get<0>(tuple)) {
                            SEM_E(E_ID_UNDEFINED, pNativeInlineASM->_pDummyExpr);
                            break;
                        }
                        
                        _asmk.getContextRef() << std::get<1>(tuple).toString();
                        _asmk.getContextRef() << asmstr[i];
                        resignVarName.clear();
                    }
                    
                } else if (beginResign == 0) {
                    if (asmstr[i] == '@') {
                        beginResign = 1;
                        
                    } else {
                        _asmk.getContextRef() << asmstr[i];
                    }
                }
            }
            
            
            
            //_asmk.getContextRef() << pNativeInlineASM->_asmcodes;
        }
        
        case StmtType::Empty: {
            break;
        }
        
        default: {
            assert(0);
        }
        }
    }
}

void SemParser::gen4Function(const Function & fun) {
    std::string funcName = fun.getName().toString();
    const auto & argList = fun.getFormalArgList();
    Function * pFunc = GlobalFuncMap::getFuncByMangling(funcName, argList);
    if (!pFunc) {
        SEM_E(E_FUNC_UNDEFINED, fun.getName());
        return;
    }
    
    _asmk.begin_FUNC(GlobalFuncMap::nameMangling(funcName, argList));
    
    const FunctionStructure & fs = fun.getFuncStu();
    
    StmtsPtr pStmts = fs.getStmtsPtr();
    assert(pStmts->getStmtType() == StmtType::Stmts);
    Stmts * pNativeStmts = static_cast<Stmts *>(pStmts.get());
    
    // 此处还需处理 形式参数(局部变量)
    FormalArgList formalArgList = fun.getFormalArgList();
    for (const FormalArg & formalArg : formalArgList) {
        bool result =
                _saBinder.bindDef(formalArg.getName(), formalArg.getType());
        if (!result) {
            SEM_E(E_VAR_REDEFINED, formalArg.getName());
            continue;
        }
        
        std::tuple<bool, VarName, VarType> tuple =
                _saBinder.getDef(formalArg.getName());
        assert(std::get<0>(tuple));
        const std::string realNameStr = std::get<1>(tuple).toString();
        const VarType & varType = std::get<2>(tuple);
        
        if (varType == "long") {
            _asmk.append_DEF_VAR_QW(realNameStr);
        } else if (varType == "int") {
            _asmk.append_DEF_VAR_DW(realNameStr);
        } else if (varType == "short") {
            _asmk.append_DEF_VAR_W(realNameStr);
        } else if (varType == "byte" || varType == "boolean" || varType == "char") {
            _asmk.append_DEF_VAR_B(realNameStr);
        } else if (varType == "float") {
            _asmk.append_DEF_VAR_FLT(realNameStr);
        } else if (varType == "double") {
            _asmk.append_DEF_VAR_DBL(realNameStr);
        } else if (varType.getDegree() > 0) {
            _asmk.append_DEF_VAR_DW(realNameStr);
        } else {
            assert(0);
        }
        
        //_asmk.append_POP_VAR(realNameStr);
        if (varType == "long") {
            _asmk.append_POP_VAR_QW(realNameStr);
        } else if (varType == "int") {
            _asmk.append_POP_VAR_DW(realNameStr);
        } else if (varType == "short") {
            _asmk.append_POP_VAR_W(realNameStr);
        } else if (varType == "byte" || varType == "boolean" || varType == "char") {
            _asmk.append_POP_VAR_B(realNameStr);
        } else if (varType == "float") {
            _asmk.append_POP_VAR_FLT(realNameStr);
        } else if (varType == "double") {
            _asmk.append_POP_VAR_DBL(realNameStr);
        } else if (varType.getDegree() > 0) {
            _asmk.append_PUSH_VAR_DW(realNameStr);
        } else {
            assert(0);
        }
        
    }
    
    gen4Stmts(fun, pNativeStmts->_stmts);
    
    const VarType & retType = fun._retType;
    // 无论如何，在函数结束前，也得生成函数返回代码
    if (retType == "long") {
        _asmk.append_IPUSH_QW(0);
        _asmk.append_RET_QW();
    } else if (retType == "int") {
        _asmk.append_IPUSH_DW(0);
        _asmk.append_RET_DW();
    } else if (retType == "short") {
        _asmk.append_IPUSH_W(0);
        _asmk.append_RET_W();
    } else if (retType == "byte" || retType == "boolean" || retType == "char") {
        _asmk.append_IPUSH_B(0);
        _asmk.append_RET_B();
    } else if (retType == "float") {
        _asmk.append_IPUSH_FLT(0);
        _asmk.append_RET_FLT();
    } else if (retType == "double") {
        _asmk.append_IPUSH_DBL(0);
        _asmk.append_RET_DBL();
    } else if (retType.getDegree() > 0) {
        _asmk.append_IPUSH_DW(0);
        _asmk.append_RET_DW();
    } else if (retType == "void") {
        _asmk.append_NRET();
    } else {
        SEM_E(E_ILLEGAL_RET_TYPE, fun._name);
    }
    
    _asmk.end_FUNC();
}

void SemParser::updateCurFileName(const std::string & curFileName) {
    _filename = curFileName;
    _sed.setFileName(curFileName);
}

void SemParser::parse(const std::vector<FileStructurePtr> & fileStus) {
    // 解析全局变量
    _saBinder.makeScope();
    _asmk.begin_STATIC();
    for (const FileStructurePtr & pFileStu : fileStus) {
        updateCurFileName(pFileStu->_filename);
        for (const GlobalVar & gvar : pFileStu->_gvars) {
            gen4VarDefine(gvar);
        }
    }
    _asmk.end_STATIC();
    
    // 解析函数
    for (const FileStructurePtr & pFileStu : fileStus) {
        updateCurFileName(pFileStu->_filename);
        for (const Function & fun : pFileStu->_funs) {
            _saBinder.makeScope();
            gen4Function(fun);
            _saBinder.clearScope();
        }
    }
    _saBinder.clearScope();
}

const std::vector<Error> &
SemParser::getErrList() {
    return _sed.getErrList();
}

bool SemParser::hasErr() const {
    return _sed.hasErr();
}