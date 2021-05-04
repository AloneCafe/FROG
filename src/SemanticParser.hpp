#ifndef __SEM_PARSER_HEADER__
#define __SEM_PARSER_HEADER__

#include <memory>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include <functional>

#include "GUIDGenerator.hpp"
#include "ILGenerator.hpp"
#include "SemanticErrorDriver.hpp"
#include "LocalCodeBinder.hpp"
#include "GlobalFuncMap.hpp"

#define ASMMAKER_ENABLE(x) \
ILGenerator * x, temp; \
if (_CHK == OnlyChk) { \
x = &temp; \
} else { \
x = &_asmk; \
}

#define ASMMAKER_DISABLE(x)

#define TYPE_PRODUCT2DEMAND(choice) \
if (true) {                         \
VarType vtChoice{choice}, vtDemand{demand};                                    \
if (_CHK == OnlyGen) {              \
	if (vtChoice.getDegree() == 0 || demand == "void") this->gen4ITC<OnlyGen>(vtChoice, vtDemand);    \
	else if (vtChoice.getDegree() != vtDemand.getDegree() && demand.getLowLvType() != "void") SEM_E((E_INCMP_TYPE_REF), pExpr); \
} else { \
		ExprTypeConstraint::ChkResult _cr_ = ExprTypeConstraint::check<_CHK>(this, vtDemand, vtChoice, allowIC); \
		if (ExprTypeConstraint::ChkResult::CHK_INCMP == _cr_) { \
				if (vtChoice == "byte")\
				{ SEM_E((E_INCMP_TYPE_BYTE), pExpr); }\
				else if (vtChoice == "boolean")       \
				{ SEM_E((E_INCMP_TYPE_BOOLEAN), pExpr); }                                      \
				else if (vtChoice == "char")       \
				{ SEM_E((E_INCMP_TYPE_CHAR), pExpr); }                                          \
				else if (vtChoice == "short")       \
				{ SEM_E((E_INCMP_TYPE_SHORT), pExpr); }                                          \
				else if (vtChoice == "int")       \
				{ SEM_E((E_INCMP_TYPE_DW), pExpr); }                                          \
				else if (vtChoice == "long")       \
				{ SEM_E((E_INCMP_TYPE_QW), pExpr); }                                          \
				else if (vtChoice == "float")           \
				{ SEM_E((E_INCMP_TYPE_FLT), pExpr); }                                          \
				else if (vtChoice == "double")                       \
				{ SEM_E((E_INCMP_TYPE_DBL), pExpr); }      \
				else if (vtChoice.getDegree() > 0)                                         \
				{ SEM_E((E_INCMP_TYPE_REF), pExpr); }                                          \
		} \
} \
}

class SemParser {
	friend class Parser;
    friend class UniSemParser;
private:
	ILGenerator _asmk;

public:
	class ExprTypeConstraint {
	public:
		enum class ChkResult : int {
			CHK_INCMP,   // 完全不兼容，至少需要用户显式附加强制类型转换
			CHK_NEED_IC, // 需要生成
			CHK_OK,      // OK
		};
		
        template <int _CHK>
        static ChkResult check(SemParser * pThis, const VarType & demand, const VarType & choice, bool allowIC) {
            ChkResult cr = _check(demand, choice);
            if (cr == ChkResult::CHK_OK) {
            
            } else if (cr == ChkResult::CHK_NEED_IC) {
                if (allowIC)
                    pThis->gen4ITC<_CHK>(choice, demand); // 生成隐式类型转换代码
                else
                    return ChkResult::CHK_INCMP;
                
            } else if (cr == ChkResult::CHK_INCMP) {
            
            } else {
                assert(0);
            }
            return cr;
        }
        
		static ChkResult _check(const VarType & demand, const VarType & choice);
		
		/*
		template <int _CHK>
		static bool check(SemParser * pThis, const VarType & demand, const VarType & choice) {
			ChkResult cr = _check(demand, choice);
			if (cr == ChkResult::CHK_OK) {
				return true;
			} else if (cr == ChkResult::CHK_NEED_IC) {
				//pThis->gen4ITC<_CHK>(choice, demand); // 生成隐式类型转换代码
				return true;
			} else if (cr == ChkResult::CHK_INCMP) {
				return false;
			} else {
				assert(0);
			}
		}*/
	};

	static const char * scalarVarType2VMStr(const VarType & choice);

	bool checkMultiTypeMatching(
        const ExprPtr & pExpr,
		const std::vector<VarType> & orderedTypeMatchList,
		VarType & okChoice);

	int getScalarTypeSize(const VarType & choice) const;

	// choiceIdx == -1 表示两类型大小相等
	bool check2ScalarTypeBigger(const VarType & choice1, const VarType & choice2, int & choiceIdx);

	enum {
	    ChkAndGen = 0,
	    OnlyChk = 1,
	    OnlyGen = 2
	};

public:
	SemParser(const std::string & filename) : _filename(filename), _sed(filename) {}
    SemParser() : _filename("<stdin>"), _sed(_filename) {}

private:
    std::string _filename;
	SemErrorDriver<ErrorDriver> _sed;
	LocalCodeBinder _saBinder;

	// 隐式类型转换
	template <int _CHK>
	void gen4ITC(const VarType & from, const VarType & to) {
		ASMMAKER_ENABLE(pASM);

		if (from == to) { return; }
		const char * fromStr = scalarVarType2VMStr(from);
		const char * toStr = scalarVarType2VMStr(to);
		pASM->append_RSZ(fromStr, toStr);

		ASMMAKER_DISABLE(pASM);
	}

	template <int _CHK>
	VarType gen4ScopeId(const ExprPtr & pExpr, const VarType & demand) {
		ASMMAKER_ENABLE(pASM);

		std::tuple<bool, VarName, VarType> tuple;
		ExprOp op = pExpr->getOp();

		if (op == ExprOp::LEAF_ID) {
			tuple = _saBinder.getDef(LocatedUtfString::make(pExpr->getLeafScalar()._str,
				pExpr->lineno(), pExpr->colno()));
		} else {
			assert(0);
		}

		if (!std::get<0>(tuple)) {
			// ID 未定义 (至少在上文中)
			SEM_E(E_ID_UNDEFINED, pExpr);
			// 再判定是不是 this 成员变量 (TODO 先不实现)
			return "void";

		} else {
			const VarName & varName = std::get<1>(tuple);
			const VarType & varType = std::get<2>(tuple);

			std::string objTypeStr = varType.toString();
			// 先排除标量，哈哈，标量无需指定大小，所以直接将偏移压栈
			if (varType.getDegree() > 0) {
				pASM->append_PUSH_VAR_DW(varName.toString().c_str()); // int32 堆句柄 heap addr
				//TYPE_PRODUCT2DEMAND(objTypeStr);
				
			} else if (objTypeStr == "int") {
				pASM->append_PUSH_VAR_DW(varName.toString().c_str());
				//TYPE_PRODUCT2DEMAND(objTypeStr);
				
			} else if (objTypeStr == "long") {
				pASM->append_PUSH_VAR_QW(varName.toString().c_str());
				//TYPE_PRODUCT2DEMAND(objTypeStr);
				
			} else if (objTypeStr == "float") {
				pASM->append_PUSH_VAR_FLT(varName.toString().c_str());
				//TYPE_PRODUCT2DEMAND(objTypeStr);

			} else if (objTypeStr == "double") {
				pASM->append_PUSH_VAR_DBL(varName.toString().c_str());
				//TYPE_PRODUCT2DEMAND(objTypeStr);

			} else if (objTypeStr == "short") {
				pASM->append_PUSH_VAR_W(varName.toString().c_str());
				//TYPE_PRODUCT2DEMAND(objTypeStr);
			
			} else if (objTypeStr == "char" || objTypeStr == "byte" || objTypeStr == "boolean") {
				pASM->append_PUSH_VAR_B(varName.toString().c_str());
				//TYPE_PRODUCT2DEMAND(objTypeStr);
				
			} else {
				// 非法类型
				SEM_E(E_ILLEGAL_TYPE_FOR_OBJ_DEFINITION, pExpr);
			}

			return varType;
		}

		ASMMAKER_DISABLE(pASM);
	}

	template <int _CHK>
	VarType gen4expr(const ExprPtr & pExpr, const VarType & demand, bool allowIC = true) {
		ASMMAKER_ENABLE(pASM);

		ExprOp op = pExpr->getOp();
		switch (op) {
		case ExprOp::LEAF_ID:
		{
			// 无需 CONSTRAINT，底层已经做了
			VarType choice = gen4ScopeId<_CHK>(pExpr, demand);
			TYPE_PRODUCT2DEMAND(choice);
			return choice;
		}

		case ExprOp::LEAF_INT_LITERAL: { // int 字面量可以被赋值给更窄的数据类型
            /*
            if (demand == "byte") {
                pASM->append_IPUSH_B(pExpr->getLeafScalar().u._c);
                return VarType::buildFromStr("byte");
            } else if (demand == "char") {
                pASM->append_IPUSH_B(pExpr->getLeafScalar().u._c);
                return VarType::buildFromStr("char");
            } else if (demand == "short") {
                pASM->append_IPUSH_W(pExpr->getLeafScalar().u._s);
                return VarType::buildFromStr("short");
            } else if (demand == "int") {
                pASM->append_IPUSH_DW(pExpr->getLeafScalar().u._d);
                return VarType::buildFromStr("int");
            } else {
                pASM->append_IPUSH_DW(pExpr->getLeafScalar().u._d);
                TYPE_PRODUCT2DEMAND("int");
                return VarType::buildFromStr("int");
            }*/
            
            // 以大小形式进行判断
            int32_t d = pExpr->getLeafScalar().u._d;
            if (allowIC) {
                if (d < 0x100) {
                    pASM->append_IPUSH_B(d);
                    if (demand == "byte") {
                        return VarType::buildFromStr("byte");
                    } else if (demand == "char") {
                        return VarType::buildFromStr("char");
                    } else {
                        TYPE_PRODUCT2DEMAND("byte");
                        return VarType::buildFromStr("byte");
                    }
                    
                } else if (d < 0x10000) {
                    pASM->append_IPUSH_W(d);
                    if (demand == "short") {
                        return VarType::buildFromStr("short");
                    } else {
                        TYPE_PRODUCT2DEMAND("short");
                        return VarType::buildFromStr("short");
                    }
                    
                } else {
                    pASM->append_IPUSH_DW(d);
                    TYPE_PRODUCT2DEMAND("int");
                    return VarType::buildFromStr("int");
                }
                
            } else {
                pASM->append_IPUSH_DW(d);
                TYPE_PRODUCT2DEMAND("int");
                return VarType::buildFromStr("int");
            }
            
            /*
            pASM->append_IPUSH_DW(pExpr->getLeafScalar().u._d);
            if (allowIC) {
                if (demand == "byte") {
                    gen4ITC<_CHK>("int", "byte");
                    return VarType::buildFromStr("byte");
                } else if (demand == "char") {
                    gen4ITC<_CHK>("int", "char");
                    return VarType::buildFromStr("char");
                } else if (demand == "short") {
                    gen4ITC<_CHK>("int", "short");
                    return VarType::buildFromStr("short");
                } else if (demand == "int") {
                    return VarType::buildFromStr("int");
                } else {
                    TYPE_PRODUCT2DEMAND("int");
                    return VarType::buildFromStr("int");
                }
            } else {
                TYPE_PRODUCT2DEMAND("int");
                return VarType::buildFromStr("int");
            }*/
        }

		case ExprOp::LEAF_LONG_LITERAL:
			pASM->append_IPUSH_QW(pExpr->getLeafScalar().u._l);
			TYPE_PRODUCT2DEMAND("long");
			return VarType::buildFromStr("long");

		case ExprOp::LEAF_FLOAT_LITERAL:
			pASM->append_IPUSH_FLT(static_cast<float>(pExpr->getLeafScalar().u._lf));
			TYPE_PRODUCT2DEMAND("float");
			return VarType::buildFromStr("float");

		case ExprOp::LEAF_LONG_DOUBLE_LITERAL:
			// long double 暂未实现，当做 double 处理
		case ExprOp::LEAF_DOUBLE_LITERAL:
		    /*
			if (demand == "float") {
				pASM->append_IPUSH_FLT(static_cast<float>(pExpr->getLeafScalar().u._lf));
				TYPE_PRODUCT2DEMAND("float");
				return VarType::buildFromStr("float");
			} else {
				pASM->append_IPUSH_DBL(pExpr->getLeafScalar().u._lf);
				TYPE_PRODUCT2DEMAND("double");
				return VarType::buildFromStr("double");
			}*/
            pASM->append_IPUSH_DBL(pExpr->getLeafScalar().u._lf);
            if (allowIC) {
                if (demand == "float") {
                    gen4ITC<_CHK>("double", "float");
                    return VarType::buildFromStr("float");
                } else if (demand == "double") {
                    return VarType::buildFromStr("double");
                } else {
                    TYPE_PRODUCT2DEMAND("double");
                    return VarType::buildFromStr("double");
                }
            } else {
                TYPE_PRODUCT2DEMAND("double");
                return VarType::buildFromStr("double");
            }

		case ExprOp::LEAF_CHAR_LITERAL:
			pASM->append_IPUSH_B(pExpr->getLeafScalar().u._c);
			TYPE_PRODUCT2DEMAND("char");
			return VarType::buildFromStr("char");

		case ExprOp::LEAF_STRING_LITERAL: {
		    
            const std::string & s = pExpr->getLeafScalar()._str;
            
            pASM->append_MKVEC_N_B(1);
            for (auto ch : s) {
                pASM->append_DUP_DW(); // dup 句柄
            }
            pASM->append_DUP_DW(); // dup 句柄
            
            uint32_t i = 0;
            for (auto ch : s) {
                pASM->append_IPUSH_DW(i);
                pASM->append_OFFSET();
                pASM->append_IPUSH_B(ch);
                pASM->append_HPOP_B();
                ++i;
            }
            // handle '\0'
            pASM->append_IPUSH_DW(i);
            pASM->append_OFFSET();
            pASM->append_IPUSH_B(0);
            pASM->append_HPOP_B();
            
            VarType vt;
            vt.setDegree(1);
            vt.setLowLvType(LocatedUtfString("char", -1, -1));
            return vt;
        }
        
		case ExprOp::LEAF_TRUE_LITERAL:
			pASM->append_IPUSH_B(1);
			TYPE_PRODUCT2DEMAND("boolean");
			return VarType::buildFromStr("boolean");

		case ExprOp::LEAF_FALSE_LITERAL:
			pASM->append_IPUSH_B(0);
			TYPE_PRODUCT2DEMAND("boolean");
			return VarType::buildFromStr("boolean");

		case ExprOp::LEAF_TYPE: // 此处无需处理，类型转换应该交由上层处理
			break;

		case ExprOp::OPT_INDEX:
		{
			// 先对数组类型 demand 升阶，然后传递给 dot
			VarType newDemand = demand;
			newDemand.getDegreeRef()++;
			
			ExprOp opLeft = pExpr->getSubExprPtr(0)->getOp();
			VarType choice;
			if (opLeft == ExprOp::LEAF_ID) {
                choice = gen4ScopeId<_CHK>(pExpr->getSubExprPtr(0), newDemand);
            } else if (opLeft == ExprOp::OPT_INDEX) {
                choice = gen4expr<_CHK>(pExpr->getSubExprPtr(0), newDemand);
            } else {
                choice = gen4expr<_CHK>(pExpr->getSubExprPtr(0), newDemand);
			    //assert(0);
			}
			// 此时操作对象已经压栈

			// 解析下标
			VarType choiceIdx;
			bool result = checkMultiTypeMatching(pExpr->getSubExprPtr(1),
				{ "byte", "char", "short", "int" }, choiceIdx);
			if (result) {
				gen4expr<_CHK>(pExpr->getSubExprPtr(1), choiceIdx);
				
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
			}

			
			if (choiceIdx != VarType::buildFromStr("int")) {
				gen4ITC<_CHK>(choiceIdx, VarType::buildFromStr("int"));
			}
			// 汇编代码，取得数组中对应下标所指的底层对象 (引用句柄)
            pASM->append_OFFSET();
			
			choice.getDegreeRef()--;
            
            // 多种类型要考虑
            if (choice == "long") {
                pASM->append_HPUSH_QW();
            } else if (choice == "int") {
                pASM->append_HPUSH_DW();
            } else if (choice == "short") {
                pASM->append_HPUSH_W();
            } else if (choice == "byte" || choice == "boolean" || choice == "char") {
                pASM->append_HPUSH_B();
            } else if (choice == "float") {
                pASM->append_HPUSH_FLT();
            } else if (choice == "double") {
                pASM->append_HPUSH_DBL();
            } else if (choice.getDegree() > 0) {
                pASM->append_HPUSH_DW();
            } else {
                assert(0);
            }
			TYPE_PRODUCT2DEMAND(choice);
			return choice;
		}

		case ExprOp::OPT_LOGIC_NOT:
		{
			// 逻辑非表达式只能产生 Boolean, 若上级表达式需求不符合，就报错
			gen4expr<_CHK>(pExpr->getSubExprPtr(0), "boolean");
			pASM->append_LNOT();

			TYPE_PRODUCT2DEMAND("boolean");
			return VarType::buildFromStr("boolean");
		}

		case ExprOp::OPT_BIT_NOT:
		{
			VarType choice;
			bool result = checkMultiTypeMatching(pExpr->getSubExprPtr(0),
				{ "byte", "char", "short", "int", "long" },
				choice);
			if (result) {
				gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice);
				
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
			}

			if (choice == "byte" || choice == "char") {
				pASM->append_BNOT_B();
			} else if (choice == "short") {
				pASM->append_BNOT_W();
			} else if (choice == "int") {
				pASM->append_BNOT_DW();
			} else if (choice == "long") {
				pASM->append_BNOT_QW();
			} else {
				assert(0);
			}

			TYPE_PRODUCT2DEMAND(choice);
			return choice;
		}

		case ExprOp::OPT_FRONT_PLUSPLUS:
		{
			VarType choice;
			bool result = checkMultiTypeMatching(pExpr->getSubExprPtr(0),
				{ "byte", "char", "short", "int", "long", "float", "double" },
				choice);

			if (choice == "char" || choice == "byte") {
				pASM->append_IPUSH_B(1);
			} else if (choice == "short") {
				pASM->append_IPUSH_W(1);
			} else if (choice == "int") {
				pASM->append_IPUSH_DW(1);
			} else if (choice == "long") {
				pASM->append_IPUSH_QW(1);
			} else if (choice == "float") {
				pASM->append_IPUSH_FLT(1);
			} else if (choice == "double") {
				pASM->append_IPUSH_DBL(1);
			} else
				assert(0);

			if (result) {
				gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice);
				TYPE_PRODUCT2DEMAND(choice);
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
			}

			if (choice == "char" || choice == "byte") {
				pASM->append_ADD_B();
			} else if (choice == "short") {
				pASM->append_ADD_W();
			} else if (choice == "int") {
				pASM->append_ADD_DW();
			} else if (choice == "long") {
				pASM->append_ADD_QW();
			} else if (choice == "float") {
				pASM->append_ADD_FLT();
			} else if (choice == "double") {
				pASM->append_ADD_DBL();
			} else
				assert(0);

			TYPE_PRODUCT2DEMAND(choice);
			return choice;
		}

		case ExprOp::OPT_FRONT_MINUSMINUS:
		{
			VarType choice;
			bool result = checkMultiTypeMatching(pExpr->getSubExprPtr(0),
				{ "byte", "char", "short", "int", "long", "float", "double" },
				choice);

			if (choice == "char" || choice == "byte") {
				pASM->append_IPUSH_B(1);
			} else if (choice == "short") {
				pASM->append_IPUSH_W(1);
			} else if (choice == "int") {
				pASM->append_IPUSH_DW(1);
			} else if (choice == "long") {
				pASM->append_IPUSH_QW(1);
			} else if (choice == "float") {
				pASM->append_IPUSH_FLT(1);
			} else if (choice == "double") {
				pASM->append_IPUSH_DBL(1);
			} else
				assert(0);

			if (result) {
				gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice);
				
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
			}

			if (choice == "char" || choice == "byte") {
				pASM->append_SUB_B();
			} else if (choice == "short") {
				pASM->append_SUB_W();
			} else if (choice == "int") {
				pASM->append_SUB_DW();
			} else if (choice == "long") {
				pASM->append_SUB_QW();
			} else if (choice == "float") {
				pASM->append_SUB_FLT();
			} else if (choice == "double") {
				pASM->append_SUB_DBL();
			} else
				assert(0);

			TYPE_PRODUCT2DEMAND(choice);
			return choice;
		}

		case ExprOp::OPT_LT:
		case ExprOp::OPT_LE:
		case ExprOp::OPT_GT:
		case ExprOp::OPT_GE:
		{
			VarType choice[2];
			bool result[2] = {
				checkMultiTypeMatching(pExpr->getSubExprPtr(0),
									   {"byte", "char", "short", "int", "long", "float", "double"},
									   choice[0]),
				checkMultiTypeMatching(pExpr->getSubExprPtr(1),
									   {"byte", "char", "short", "int", "long", "float", "double"},
									   choice[1]) };

			int idx;
			if (result[0] && result[1]) {
				bool checkFlag = check2ScalarTypeBigger(choice[0], choice[1], idx);
				assert(checkFlag);

				if (idx == 0) {
					// 可隐式类型转换，左侧数据类型大于等于右侧数据类型
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
					gen4ITC<_CHK>(choice[1], choice[0]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);

				} else if (idx == 1) {
					// 可隐式类型转换，右侧数据类型大于等于左侧数据类型
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);
					gen4ITC<_CHK>(choice[0], choice[1]);

				} else if (idx == -1) {
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);
					idx = 0;

				} else {
					// 不可转换，右侧数据类型过大
					SEM_E(E_INCMP_TYPE_CONVERT, pExpr);
					return demand;
				}
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
				return demand;
			}

			/*
			int idx;
			if (result[0] && result[1]) {

				bool r = check2ScalarTypeBigger(choice[0], choice[1], idx);
				if (r) {                                     // 谁大以谁为准，根据规律将自动生成 ITC
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[idx]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[idx]);
					
				} else {
					assert(0);
				}
			} else {
				gen4expr<_CHK>(pExpr->getSubExprPtr(1), "int");
				gen4expr<_CHK>(pExpr->getSubExprPtr(0), "int");

			}*/

			switch (op) {
			case ExprOp::OPT_LT:
				if (choice[idx] == "char" || choice[idx] == "byte") {
					pASM->append_LT_B();
				} else if (choice[idx] == "short") {
					pASM->append_LT_W();
				} else if (choice[idx] == "int") {
					pASM->append_LT_DW();
				} else if (choice[idx] == "long") {
					pASM->append_LT_QW();
				} else if (choice[idx] == "float") {
					pASM->append_LT_FLT();
				} else if (choice[idx] == "double") {
					pASM->append_LT_DBL();
				} else
					assert(0);
				break;

			case ExprOp::OPT_LE:
				if (choice[idx] == "char" || choice[idx] == "byte") {
					pASM->append_LE_B();
				} else if (choice[idx] == "short") {
					pASM->append_LE_W();
				} else if (choice[idx] == "int") {
					pASM->append_LE_DW();
				} else if (choice[idx] == "long") {
					pASM->append_LE_QW();
				} else if (choice[idx] == "float") {
					pASM->append_LE_FLT();
				} else if (choice[idx] == "double") {
					pASM->append_LE_DBL();
				} else
					assert(0);
				break;

			case ExprOp::OPT_GT:
				if (choice[idx] == "char" || choice[idx] == "byte") {
					pASM->append_GT_B();
				} else if (choice[idx] == "short") {
					pASM->append_GT_W();
				} else if (choice[idx] == "int") {
					pASM->append_GT_DW();
				} else if (choice[idx] == "long") {
					pASM->append_GT_QW();
				} else if (choice[idx] == "float") {
					pASM->append_GT_FLT();
				} else if (choice[idx] == "double") {
					pASM->append_GT_DBL();
				} else
					assert(0);
				break;

			case ExprOp::OPT_GE:
				if (choice[idx] == "char" || choice[idx] == "byte") {
					pASM->append_GE_B();
				} else if (choice[idx] == "short") {
					pASM->append_GE_W();
				} else if (choice[idx] == "int") {
					pASM->append_GE_DW();
				} else if (choice[idx] == "long") {
					pASM->append_GE_QW();
				} else if (choice[idx] == "float") {
					pASM->append_GE_FLT();
				} else if (choice[idx] == "double") {
					pASM->append_GE_DBL();
				} else
					assert(0);
				break;
			}

			TYPE_PRODUCT2DEMAND("boolean"); // 只生成 boolean
			return VarType::buildFromStr("boolean");
		}

		case ExprOp::OPT_EQ:
		case ExprOp::OPT_NE:
		{
			VarType choice[2];
			bool result[2] = {
				checkMultiTypeMatching(pExpr->getSubExprPtr(0),
									   {"boolean", "byte", "char", "short", "int", "long", "float", "double"},
									   choice[0]),
				checkMultiTypeMatching(pExpr->getSubExprPtr(1),
									   {"boolean", "byte", "char", "short", "int", "long", "float", "double"},
									   choice[1]) };

			int idx;
			if (result[0] && result[1]) {
				bool checkFlag = check2ScalarTypeBigger(choice[0], choice[1], idx);
				assert(checkFlag);

				if (idx == 0) {
					// 可隐式类型转换，左侧数据类型大于等于右侧数据类型
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
					gen4ITC<_CHK>(choice[1], choice[0]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);

				} else if (idx == 1) {
					// 可隐式类型转换，右侧数据类型大于等于左侧数据类型
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);
					gen4ITC<_CHK>(choice[0], choice[1]);

				} else if (idx == -1) {
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);
					idx = 0;

				} else {
					// 不可转换，右侧数据类型过大
					SEM_E(E_INCMP_TYPE_CONVERT, pExpr);
					return demand;
				}
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
				return demand;
			}

			/*
			int idx;
			if (result[0] && result[1]) {

				bool r = check2ScalarTypeBigger(choice[0], choice[1], idx);
				if (r) {                                     // 谁大以谁为准，根据规律将自动生成 ITC
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[idx]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[idx]);
				} else {
					assert(0);
				}
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
			}*/

			if (op == ExprOp::OPT_EQ) {
				if (choice[idx] == "char" || choice[idx] == "byte" || choice[idx] == "boolean") {
					pASM->append_EQ_B();
				} else if (choice[idx] == "short") {
					pASM->append_EQ_W();
				} else if (choice[idx] == "int") {
					pASM->append_EQ_DW();
				} else if (choice[idx] == "long") {
					pASM->append_EQ_QW();
				} else if (choice[idx] == "float") {
					pASM->append_EQ_FLT();
				} else if (choice[idx] == "double") {
					pASM->append_EQ_DBL();
				} else
					assert(0);
			} else if (op == ExprOp::OPT_NE) {
				if (choice[idx] == "char" || choice[idx] == "byte" || choice[idx] == "boolean") {
					pASM->append_NE_B();
				} else if (choice[idx] == "short") {
					pASM->append_NE_W();
				} else if (choice[idx] == "int") {
					pASM->append_NE_DW();
				} else if (choice[idx] == "long") {
					pASM->append_NE_QW();
				} else if (choice[idx] == "float") {
					pASM->append_NE_FLT();
				} else if (choice[idx] == "double") {
					pASM->append_NE_DBL();
				} else
					assert(0);
			} else
				assert(0);

			TYPE_PRODUCT2DEMAND("boolean"); // 只生成 boolean
			return VarType::buildFromStr("boolean");
		}

		case ExprOp::OPT_LOGIC_AND:
		case ExprOp::OPT_LOGIC_OR:
		{ // TODO 需要重构
			gen4expr<_CHK>(pExpr->getSubExprPtr(1), "boolean");
			gen4expr<_CHK>(pExpr->getSubExprPtr(0), "boolean");
			if (op == ExprOp::OPT_LOGIC_AND) {
				pASM->append_LAND();
			} else if (op == ExprOp::OPT_LOGIC_OR) {
				pASM->append_LOR();
			} else {
				assert(0);
			}
			// 逻辑非表达式只能产生 Boolean, 若上级表达式需求不符合，就报错
			TYPE_PRODUCT2DEMAND("boolean");
			return VarType::buildFromStr("boolean");
		}

		case ExprOp::OPT_BIT_AND:
		case ExprOp::OPT_BIT_OR:
		case ExprOp::OPT_BIT_XOR:
		{
			VarType choice[2];
			bool result[2] = {
				checkMultiTypeMatching(pExpr->getSubExprPtr(0),
									   {"byte", "char", "short", "int", "long"},
									   choice[0]),
				checkMultiTypeMatching(pExpr->getSubExprPtr(1),
									   {"byte", "char", "short", "int", "long"},
									   choice[1]) };

			int idx;
			if (result[0] && result[1]) {
				bool checkFlag = check2ScalarTypeBigger(choice[0], choice[1], idx);
				assert(checkFlag);

				if (idx == 0) {
					// 可隐式类型转换，左侧数据类型大于等于右侧数据类型
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
					gen4ITC<_CHK>(choice[1], choice[0]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);

				} else if (idx == 1) {
					// 可隐式类型转换，右侧数据类型大于等于左侧数据类型
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);
					gen4ITC<_CHK>(choice[0], choice[1]);

				} else if (idx == -1) {
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);
					idx = 0;

				} else {
					// 不可转换，右侧数据类型过大
					SEM_E(E_INCMP_TYPE_CONVERT, pExpr);
					return demand;
				}
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
				return demand;
			}

			/*
			int idx;
			if (result[0] && result[1]) {

				bool r = check2ScalarTypeBigger(choice[0], choice[1], idx);
				if (r) {                                       // 谁大以谁为准，根据规律将自动生成 ITC
					
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[idx]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[idx]);
				} else
					assert(0);
			} else {
				gen4expr<_CHK>(pExpr->getSubExprPtr(1), "int");
				gen4expr<_CHK>(pExpr->getSubExprPtr(0), "int");
			}*/

			if (op == ExprOp::OPT_BIT_AND) {
				if (choice[idx] == "char" || choice[idx] == "byte") {
					pASM->append_BAND_B();
				} else if (choice[idx] == "short") {
					pASM->append_BAND_W();
				} else if (choice[idx] == "int") {
					pASM->append_BAND_DW();
				} else if (choice[idx] == "long") {
					pASM->append_BAND_QW();
				} else
					assert(0);
			} else if (op == ExprOp::OPT_BIT_OR) {
				if (choice[idx] == "char" || choice[idx] == "byte") {
					pASM->append_BOR_B();
				} else if (choice[idx] == "short") {
					pASM->append_BOR_W();
				} else if (choice[idx] == "int") {
					pASM->append_BOR_DW();
				} else if (choice[idx] == "long") {
					pASM->append_BOR_QW();
				} else
					assert(0);
			} else if (op == ExprOp::OPT_BIT_XOR) {
				if (choice[idx] == "char" || choice[idx] == "byte") {
					pASM->append_BXOR_B();
				} else if (choice[idx] == "short") {
					pASM->append_BXOR_W();
				} else if (choice[idx] == "int") {
					pASM->append_BXOR_DW();
				} else if (choice[idx] == "long") {
					pASM->append_BXOR_QW();
				} else
					assert(0);
			} else {
				assert(0);
			}

			if (result[0] && result[1]) {
				TYPE_PRODUCT2DEMAND(choice[idx]); // 生成较大的类型
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
			}

			return choice[idx];
		}

		case ExprOp::OPT_PLUS:
		case ExprOp::OPT_MINUS:
		case ExprOp::OPT_MUL:
		case ExprOp::OPT_DIV:
		case ExprOp::OPT_MOD:
		{
			VarType choice[2];
			bool result[2] = {
				checkMultiTypeMatching(pExpr->getSubExprPtr(0),
									   {"byte", "char", "short", "int", "long", "float", "double" },
									   choice[0]),
				checkMultiTypeMatching(pExpr->getSubExprPtr(1),
									   {"byte", "char", "short", "int", "long", "float", "double" },
									   choice[1]) };

			int idx;
			if (result[0] && result[1]) {
				bool checkFlag = check2ScalarTypeBigger(choice[0], choice[1], idx);
				assert(checkFlag);

				if (idx == 0) {
					// 可隐式类型转换，左侧数据类型大于等于右侧数据类型
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
					gen4ITC<_CHK>(choice[1], choice[0]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);

				} else if (idx == 1) {
					// 可隐式类型转换，右侧数据类型大于等于左侧数据类型
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);
					gen4ITC<_CHK>(choice[0], choice[1]);

				} else if (idx == -1) {
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);
					idx = 0;

				} else {
					// 不可转换，右侧数据类型过大
					SEM_E(E_INCMP_TYPE_CONVERT, pExpr);
					return demand;
				}
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
				return demand;
			}

			/*
			int idx;
			if (result[0] && result[1]) {

				bool r = check2ScalarTypeBigger(choice[0], choice[1], idx);
				if (r) {                                       // 谁大以谁为准，根据规律将自动生成 ITC
					gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[idx]);
					gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[idx]);
				} else {
					assert(0);
				}
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
			}*/

			switch (op) {
			case ExprOp::OPT_PLUS:
				if (choice[idx] == "char" || choice[idx] == "byte") {
					pASM->append_ADD_B();
				} else if (choice[idx] == "short") {
					pASM->append_ADD_W();
				} else if (choice[idx] == "int") {
					pASM->append_ADD_DW();
				} else if (choice[idx] == "long") {
					pASM->append_ADD_QW();
				} else if (choice[idx] == "float") {
					pASM->append_ADD_FLT();
				} else if (choice[idx] == "double") {
					pASM->append_ADD_DBL();
				} else
					assert(0);
				break;

			case ExprOp::OPT_MINUS:
				if (choice[idx] == "char" || choice[idx] == "byte") {
					pASM->append_SUB_B();
				} else if (choice[idx] == "short") {
					pASM->append_SUB_W();
				} else if (choice[idx] == "int") {
					pASM->append_SUB_DW();
				} else if (choice[idx] == "long") {
					pASM->append_SUB_QW();
				} else if (choice[idx] == "float") {
					pASM->append_SUB_FLT();
				} else if (choice[idx] == "double") {
					pASM->append_SUB_DBL();
				} else
					assert(0);
				break;

			case ExprOp::OPT_MUL:
				if (choice[idx] == "char" || choice[idx] == "byte") {
					pASM->append_MUL_B();
				} else if (choice[idx] == "short") {
					pASM->append_MUL_W();
				} else if (choice[idx] == "int") {
					pASM->append_MUL_DW();
				} else if (choice[idx] == "long") {
					pASM->append_MUL_QW();
				} else if (choice[idx] == "float") {
					pASM->append_MUL_FLT();
				} else if (choice[idx] == "double") {
					pASM->append_MUL_DBL();
				} else
					assert(0);
				break;

			case ExprOp::OPT_DIV:
				if (choice[idx] == "char" || choice[idx] == "byte") {
					pASM->append_DIV_B();
				} else if (choice[idx] == "short") {
					pASM->append_DIV_W();
				} else if (choice[idx] == "int") {
					pASM->append_DIV_DW();
				} else if (choice[idx] == "long") {
					pASM->append_DIV_QW();
				} else if (choice[idx] == "float") {
					pASM->append_DIV_FLT();
				} else if (choice[idx] == "double") {
					pASM->append_DIV_DBL();
				} else
					assert(0);
				break;

			case ExprOp::OPT_MOD:
				if (choice[idx] == "char" || choice[idx] == "byte") {
					pASM->append_MOD_B();
				} else if (choice[idx] == "short") {
					pASM->append_MOD_W();
				} else if (choice[idx] == "int") {
					pASM->append_MOD_DW();
				} else if (choice[idx] == "long") {
					pASM->append_MOD_QW();
				} else if (choice[idx] == "float") {
					pASM->append_MOD_FLT();
				} else if (choice[idx] == "double") {
					pASM->append_MOD_DBL();
				} else
					assert(0);
				break;
			}

			if (result[0] && result[1]) {
				TYPE_PRODUCT2DEMAND(choice[idx]); // 生成较大的类型
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
			}
			return choice[idx];
		}

		case ExprOp::OPT_LSHIFT:
		case ExprOp::OPT_RSHIFT:
		case ExprOp::OPT_RSHIFT_ZERO:
		{
			VarType choice[2];
			bool result[2] = {
				checkMultiTypeMatching(pExpr->getSubExprPtr(0),
									   {"byte", "char", "short", "int", "long"},
									   choice[0]),
				checkMultiTypeMatching(pExpr->getSubExprPtr(1),
									   {"byte"},
									   choice[1]) };

			if (result[0] && result[1]) {
				gen4expr<_CHK>(pExpr->getSubExprPtr(1), choice[1]);
				gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice[0]);
			} else {
				gen4expr<_CHK>(pExpr->getSubExprPtr(1), "byte");
				gen4expr<_CHK>(pExpr->getSubExprPtr(0), "int");
			}

			switch (op) {
			case ExprOp::OPT_LSHIFT:
				if (choice[0] == "char" || choice[0] == "byte") {
					pASM->append_SHL_B();
				} else if (choice[0] == "short") {
					pASM->append_SHL_W();
				} else if (choice[0] == "int") {
					pASM->append_SHL_DW();
				} else if (choice[0] == "long") {
					pASM->append_SHL_QW();
				} else
					assert(0);
				break;

			case ExprOp::OPT_RSHIFT:
				if (choice[0] == "char" || choice[0] == "byte") {
					pASM->append_SHR_B();
				} else if (choice[0] == "short") {
					pASM->append_SHR_W();
				} else if (choice[0] == "int") {
					pASM->append_SHR_DW();
				} else if (choice[0] == "long") {
					pASM->append_SHR_QW();
				} else
					assert(0);
				break;

			case ExprOp::OPT_RSHIFT_ZERO:
				if (choice[0] == "char" || choice[0] == "byte") {
					pASM->append_SHRZ_B();
				} else if (choice[0] == "short") {
					pASM->append_SHRZ_W();
				} else if (choice[0] == "int") {
					pASM->append_SHRZ_DW();
				} else if (choice[0] == "long") {
					pASM->append_SHRZ_QW();
				} else
					assert(0);
				break;
			}

			if (result[0] && result[1]) {
				TYPE_PRODUCT2DEMAND(choice[0]); // 生成较大的类型
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
			}
			return choice[0];
		}

		case ExprOp::OPT_ASSIGN:
		{
		 
		 
			ExprPtr pLeftExpr = pExpr->getSubExprPtr(0), 
				pRightExpr = pExpr->getSubExprPtr(1);
			
            ExprOp leftOp = pLeftExpr->getOp();
			if (leftOp != ExprOp::LEAF_ID && leftOp != ExprOp::OPT_INDEX) {
				SEM_E(E_ASSIGN_LEFT_VAL, pExpr);
				return demand;
			}
			
			if (leftOp == ExprOp::LEAF_ID) { // 左值 id
                VarType choice[2];
                choice[0] = gen4ScopeId<OnlyChk>(pLeftExpr, demand);
                
                bool result;
                
                if (choice[0].getDegree() > 0) {
                    result = checkMultiTypeMatching(pRightExpr,
                            { choice[0] },
                            choice[1]);
                    gen4expr<_CHK>(pRightExpr, choice[1]);
                    
                } else { // choice[0].getDegree() == 0
                    result = checkMultiTypeMatching(pRightExpr,
                            { "boolean", "byte", "char", "short", "int", "long", "float", "double" },
                            choice[1]);
                    int choiceIdx;
                    bool checkFlag = check2ScalarTypeBigger(choice[0], choice[1], choiceIdx);
                    assert(checkFlag);
    
                    if (choiceIdx == 0) {
                        // 可隐式类型转换，左侧数据类型大于等于右侧数据类型
                        gen4expr<_CHK>(pRightExpr, choice[1]);
                        gen4ITC<_CHK>(choice[1], choice[0]);
        
                    } else if (choiceIdx == -1) {
                        gen4expr<_CHK>(pRightExpr, choice[1]);
        
                    } else {
                        // 不可转换，右侧数据类型过大
                        SEM_E(E_INCMP_TYPE_CONVERT, pExpr);
                        return demand;
                    }
                }
                
                if (!result)  {
                    SEM_E(E_INCMP_TYPE, pExpr);
                    return demand;
                }
                
                
                std::tuple<bool, VarName, VarType> tuple =
                        _saBinder.getDef(LocatedUtfString::make(pLeftExpr->getLeafScalar()._str,
                                pLeftExpr->lineno(), pLeftExpr->colno()));
                
                if (std::get<0>(tuple)) {
                    const VarName & varName = std::get<1>(tuple);
                    const VarType & varType = std::get<2>(tuple);
                    if (varType.getDegree() > 0) {
                        pASM->append_TOP_VAR_DW(varName.toString().c_str());
                    } else if (varType == "int") {
                        pASM->append_TOP_VAR_DW(varName.toString().c_str());
                    } else if (varType == "long") {
                        pASM->append_TOP_VAR_QW(varName.toString().c_str());
                    } else if (varType == "float") {
                        pASM->append_TOP_VAR_FLT(varName.toString().c_str());
                    } else if (varType == "double") {
                        pASM->append_TOP_VAR_DBL(varName.toString().c_str());
                    } else if (varType == "short") {
                        pASM->append_TOP_VAR_W(varName.toString().c_str());
                    } else if (varType == "char" || varType == "byte" || varType == "boolean") {
                        pASM->append_TOP_VAR_B(varName.toString().c_str());
                    } else {
                        assert(0);
                    }
                }
                else
                    SEM_E(E_ID_UNDEFINED, pLeftExpr);
                
                TYPE_PRODUCT2DEMAND(choice[0]);
                return choice[0];
			
			} else if (leftOp == ExprOp::OPT_INDEX) { // 左值 index 表达式
			    
                VarType choice[2];
                
                VarType newDemand = demand;
                newDemand.getDegreeRef()++;
                
                choice[0] = gen4expr<_CHK>(pLeftExpr->getSubExprPtr(0), newDemand);
                
                // 解析下标
                VarType choiceIdx;
                bool result = checkMultiTypeMatching(pLeftExpr->getSubExprPtr(1),
                        { "byte", "char", "short", "int" }, choiceIdx);
                if (result) {
                    gen4expr<_CHK>(pLeftExpr->getSubExprPtr(1), choiceIdx);
                    
                } else {
                    SEM_E(E_INCMP_TYPE, pLeftExpr);
                }
                
                if (choiceIdx != VarType::buildFromStr("int")) {
                    gen4ITC<_CHK>(choiceIdx, VarType::buildFromStr("int"));
                }
                
                // 汇编代码，取得数组中对应下标所指的底层对象 (引用句柄)
                pASM->append_OFFSET();
                
                choice[0].getDegreeRef()--;
                
                

#if 0
                result = checkMultiTypeMatching(pRightExpr,
                        { "boolean", "byte", "char", "short", "int", "long", "float", "double" },
                        choice[1]);
                if (result && choice[0].getDegree() == 0) {
                    int choiceIdx;
                    bool checkFlag = check2ScalarTypeBigger(choice[0], choice[1], choiceIdx);
                    assert(checkFlag);
                    
                    if (choiceIdx == 0) {
                        // 可隐式类型转换，左侧数据类型大于等于右侧数据类型
                        gen4expr<_CHK>(pRightExpr, choice[1]);
                        gen4ITC<_CHK>(choice[1], choice[0]);
                        
                    } else if (choiceIdx == -1) {
                        gen4expr<_CHK>(pRightExpr, choice[1]);
                        
                    } else {
                        // 不可转换，右侧数据类型过大
                        SEM_E(E_INCMP_TYPE_CONVERT, pExpr);
                        return demand;
                    }
                    
                } else {
                    SEM_E(E_INCMP_TYPE, pExpr);
                    return demand;
                }
#else
                if (choice[0].getDegree() > 0) {
                    result = checkMultiTypeMatching(pRightExpr,
                            { choice[0] },
                            choice[1]);
                    gen4expr<_CHK>(pRightExpr, choice[1]);
                    
                } else { // choice[0].getDegree() == 0
                    result = checkMultiTypeMatching(pRightExpr,
                            { "boolean", "byte", "char", "short", "int", "long", "float", "double" },
                            choice[1]);
                    int choiceIdx;
                    bool checkFlag = check2ScalarTypeBigger(choice[0], choice[1], choiceIdx);
                    assert(checkFlag);
                    
                    if (choiceIdx == 0) {
                        // 可隐式类型转换，左侧数据类型大于等于右侧数据类型
                        gen4expr<_CHK>(pRightExpr, choice[1]);
                        gen4ITC<_CHK>(choice[1], choice[0]);
                        
                    } else if (choiceIdx == -1) {
                        gen4expr<_CHK>(pRightExpr, choice[1]);
                        
                    } else {
                        // 不可转换，右侧数据类型过大
                        SEM_E(E_INCMP_TYPE_CONVERT, pExpr);
                        return demand;
                    }
                }
                
                if (!result)  {
                    SEM_E(E_INCMP_TYPE, pExpr);
                    return demand;
                }
#endif
                
                // 多种类型要考虑
                if (choice[0] == "long") {
                    pASM->append_HPOP_QW();
                } else if (choice[0] == "int") {
                    pASM->append_HPOP_DW();
                } else if (choice[0] == "short") {
                    pASM->append_HPOP_W();
                } else if (choice[0] == "byte" || choice[0] == "boolean" || choice[0] == "char") {
                    pASM->append_HPOP_B();
                } else if (choice[0] == "float") {
                    pASM->append_HPOP_FLT();
                } else if (choice[0] == "double") {
                    pASM->append_HPOP_DBL();
                } else if (choice[0].getDegree() > 0) {
                    pASM->append_HPOP_DW();
                } else {
                    assert(0);
                }
                
                /*
                std::tuple<bool, VarName, VarType> tuple =
                        _saBinder.getDef(LocatedUtfString::make(pLeftExpr->getLeafScalar()._str,
                                pLeftExpr->lineno(), pLeftExpr->colno()));
                
                if (std::get<0>(tuple))
                    pASM->append_TOP_VAR(std::get<1>(tuple).toDebugString().c_str());
                else
                    SEM_E(E_ID_UNDEFINED, pLeftExpr);
                */
                
                //TYPE_PRODUCT2DEMAND(choice[0]);
                return choice[0];
			
			} else {
			    assert(0);
			}
		}

		case ExprOp::OPT_ASSIGN_WITH_ADD: 
		case ExprOp::OPT_ASSIGN_WITH_SUB:
		case ExprOp::OPT_ASSIGN_WITH_MUL:
		case ExprOp::OPT_ASSIGN_WITH_DIV:
		case ExprOp::OPT_ASSIGN_WITH_MOD:
		case ExprOp::OPT_ASSIGN_WITH_BIT_AND:
		case ExprOp::OPT_ASSIGN_WITH_BIT_OR:
		case ExprOp::OPT_ASSIGN_WITH_BIT_XOR:
		case ExprOp::OPT_ASSIGN_WITH_LSHIFT:
		case ExprOp::OPT_ASSIGN_WITH_RSHIFT:
		case ExprOp::OPT_ASSIGN_WITH_RSHIFT_ZERO: { // 分散瓦解，转调用
			ExprPtr pLeftExpr = pExpr->getSubExprPtr(0),
				pRightExpr = pExpr->getSubExprPtr(1);
            ExprOp leftOp = pLeftExpr->getOp();
			if (leftOp != ExprOp::LEAF_ID && leftOp != ExprOp::OPT_INDEX) {
				SEM_E(E_ASSIGN_LEFT_VAL, pExpr);
				return demand;
			}

			ExprOp newOp;
			switch (op) {
			case ExprOp::OPT_ASSIGN_WITH_ADD:
				newOp = ExprOp::OPT_PLUS; break;
			case ExprOp::OPT_ASSIGN_WITH_SUB:
				newOp = ExprOp::OPT_MINUS; break;
			case ExprOp::OPT_ASSIGN_WITH_MUL:
				newOp = ExprOp::OPT_MUL; break;
			case ExprOp::OPT_ASSIGN_WITH_DIV:
				newOp = ExprOp::OPT_DIV; break;
			case ExprOp::OPT_ASSIGN_WITH_MOD:
				newOp = ExprOp::OPT_MOD; break;
			case ExprOp::OPT_ASSIGN_WITH_BIT_AND:
				newOp = ExprOp::OPT_BIT_AND; break;
			case ExprOp::OPT_ASSIGN_WITH_BIT_OR:
				newOp = ExprOp::OPT_BIT_OR; break;
			case ExprOp::OPT_ASSIGN_WITH_BIT_XOR:
				newOp = ExprOp::OPT_BIT_XOR; break;
			case ExprOp::OPT_ASSIGN_WITH_LSHIFT:
				newOp = ExprOp::OPT_LSHIFT; break;
			case ExprOp::OPT_ASSIGN_WITH_RSHIFT:
				newOp = ExprOp::OPT_RSHIFT; break;
			case ExprOp::OPT_ASSIGN_WITH_RSHIFT_ZERO:
				newOp = ExprOp::OPT_RSHIFT_ZERO; break;
			default: assert(0);
			}
			ExprPtr pNewRightExpr = Expr::newExpr(newOp,
				{ pLeftExpr, pRightExpr },
				pExpr->lineno(), pExpr->colno());

			ExprPtr pNewExpr = Expr::newExpr(ExprOp::OPT_ASSIGN,
				{ pLeftExpr, pNewRightExpr },
				pExpr->lineno(), pExpr->colno());
			return gen4expr<_CHK>(pNewExpr, demand);
		}

		case ExprOp::OPT_COND: {
			VarType choice;
			bool result = checkMultiTypeMatching(pExpr->getSubExprPtr(0),
									   {"boolean"}, choice);
			gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice);
			LabelGroup::CondExpr lg = AdvGuidGen::applyLabelGroup4CondExpr();
			pASM->append_JF(lg._lbl_condexpr_false);
			pASM->append_LABEL(lg._lbl_condexpr_true);
			  gen4expr<_CHK>(pExpr->getSubExprPtr(1), demand);
			pASM->append_J(lg._lbl_condexpr_end);
			pASM->append_LABEL(lg._lbl_condexpr_false);
			  gen4expr<_CHK>(pExpr->getSubExprPtr(2), demand);
			pASM->append_LABEL(lg._lbl_condexpr_end);
			// 下层貌似会处理 demand, 所以此处无需处理
			return demand;
		}

		case ExprOp::OPT_COMMA: {
			VarType choice;
			uint32_t cnt = pExpr->getExprCnt(), i;
			for (i = 0; i < cnt - 1; ++i) {
				gen4expr<_CHK == OnlyChk ? 
					OnlyChk : OnlyGen>(pExpr->getSubExprPtr(i), "void");
			}

			bool result = checkMultiTypeMatching(pExpr->getSubExprPtr(i),
				{ "boolean", "byte", "char", "short", "int", "long", "float", "double" },
				choice);
			
			if (result) {
				// 若需要检查，那么检查最后一个子表达式的类型是必须的
				choice = gen4expr<_CHK>(pExpr->getSubExprPtr(i), demand);

			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
			}
			
			TYPE_PRODUCT2DEMAND(choice);
			return choice;
		}

		case ExprOp::OPT_NEGATIVE: {
			VarType choice;
			bool result = checkMultiTypeMatching(pExpr->getSubExprPtr(0),
				{ "byte", "char", "short", "int", "long", "float", "double" },
				choice);
			if (result) {
				gen4expr<_CHK>(pExpr->getSubExprPtr(0), choice);
				
			} else {
				SEM_E(E_INCMP_TYPE, pExpr);
			}

			if (choice == "byte" || choice == "char") {
				pASM->append_NEG_B();
			} else if (choice == "short") {
				pASM->append_NEG_W();
			} else if (choice == "int") {
				pASM->append_NEG_DW();
			} else if (choice == "long") {
                pASM->append_NEG_QW();
            } else if (choice == "float") {
                pASM->append_NEG_FLT();
            } else if (choice == "double") {
                pASM->append_NEG_DBL();
            } else {
				assert(0);
			}

			TYPE_PRODUCT2DEMAND(choice);
			return choice;
		}

		case ExprOp::OPT_POSITIVE: {
			return gen4expr<_CHK>(pExpr->getSubExprPtr(0), demand);
		}

		case ExprOp::OPT_TYPE_CONVERT: {
			if (pExpr->getSubExprPtr(0)->getOp() != ExprOp::LEAF_TYPE) {
				SEM_E(E_ILLEGAL_TYPE_CONVERT_TYPE, pExpr);
				return demand;
			}
			const VarType &toVarType = pExpr->getSubExprPtr(0)->getLeafType();
			const VarType fromVarType = gen4expr<_CHK>(pExpr->getSubExprPtr(1), "void");
			if (toVarType.getDegree() > 0) {
				gen4ITC<_CHK>("void", "int");
			} else {
				gen4ITC<_CHK>("void", toVarType);
			}

			TYPE_PRODUCT2DEMAND(toVarType);
			return toVarType;
		}

		case ExprOp::OPT_CALL: {
			VarType choice;

			ExprPtr pFuncExprId = pExpr->getSubExprPtr(0);
			if (pFuncExprId->getOp() != ExprOp::LEAF_ID) {
				SEM_E(E_ILLEGAL_FUNCTION_TO_CALL, pExpr);
				return demand;
			}
			std::vector<Function> perhapsFuncs =
			        GlobalFuncMap::getFuncsNoMangling(pFuncExprId->getLeafScalar()._str);
			if (perhapsFuncs.empty()) {
				SEM_E(E_FUNC_UNDEFINED, pFuncExprId);
				return demand;

			} else {
			    // 循环检查可能匹配的函数
			    for (const auto & func : perhapsFuncs) {
                    // 检查参数，并生成调用
                    const auto & fal = func.getFormalArgList();
                    uint32_t argn = fal.size();
                    if (pExpr->getExprCnt() - 1 != argn) {
                        // 参数个数不匹配
                        //SEM_E(E_FUNC_ARG_NUM_MISMATCH, pFuncExprId);
                        continue; // 下一个
                    
                    } else {
                        // 预先检查
                        for (uint32_t i = argn; i > 0; --i) {
                            const VarType & argTypeDemand = fal[i - 1].getType();
                            ExprPtr pArgExprId = pExpr->getSubExprPtr(i);
                            SEM_E_COUNT_RECOND;
                            gen4expr<OnlyChk>(pArgExprId, argTypeDemand, false);
                            if (SEM_E_COUNT_HAS_INCREASED) {
                                SEM_E_COUNT_CLEAR_DELTA;
                                goto lbl_next_func0; // 此函数不匹配，下一个函数
                            }
                        }
                        
                        // 从右往左压栈
                        for (uint32_t i = argn; i > 0; --i) {
                            const VarType & argTypeDemand = fal[i - 1].getType();
                            ExprPtr pArgExprId = pExpr->getSubExprPtr(i);
                            gen4expr<_CHK>(pArgExprId, argTypeDemand);
                        }
                    
                        pASM->append_CALL(GlobalFuncMap::nameMangling(func.getName().toString(), func.getFormalArgList()));
                    
                        // 最终检查返回值类型约束
                        const VarType & retType = func.getRetType();
                        TYPE_PRODUCT2DEMAND(retType);
                        return retType;
                    }
                    
                    lbl_next_func0: continue;
			    }
			    
			    // 此处再进行允许隐式类型转换的函数调用
                // 循环检查可能匹配的函数
                for (const auto & func : perhapsFuncs) {
                    // 检查参数，并生成调用
                    const auto & fal = func.getFormalArgList();
                    uint32_t argn = fal.size();
                    if (pExpr->getExprCnt() - 1 != argn) {
                        // 参数个数不匹配
                        //SEM_E(E_FUNC_ARG_NUM_MISMATCH, pFuncExprId);
                        continue; // 下一个
                        
                    } else {
                        // 预先检查
                        for (uint32_t i = argn; i > 0; --i) {
                            const VarType & argTypeDemand = fal[i - 1].getType();
                            ExprPtr pArgExprId = pExpr->getSubExprPtr(i);
                            SEM_E_COUNT_RECOND;
                            gen4expr<OnlyChk>(pArgExprId, argTypeDemand, true);
                            if (SEM_E_COUNT_HAS_INCREASED) {
                                SEM_E_COUNT_CLEAR_DELTA;
                                goto lbl_next_func1; // 此函数不匹配，下一个函数
                            }
                        }
                        
                        // 从右往左压栈
                        for (uint32_t i = argn; i > 0; --i) {
                            const VarType & argTypeDemand = fal[i - 1].getType();
                            ExprPtr pArgExprId = pExpr->getSubExprPtr(i);
                            gen4expr<_CHK>(pArgExprId, argTypeDemand);
                        }
                        
                        pASM->append_CALL(GlobalFuncMap::nameMangling(func.getName().toString(), func.getFormalArgList()));
                        
                        // 最终检查返回值类型约束
                        const VarType & retType = func.getRetType();
                        TYPE_PRODUCT2DEMAND(retType);
                        return retType;
                    }
                    
                    lbl_next_func1: continue;
                }
			    
			    SEM_E(E_MISMATCHING_FUNC, pFuncExprId);
                return demand;
				
			}
		}

		default: {
			SEM_E(E_ILLEGAL_EXPR, pExpr);
			return demand;
		}
		}
		
		ASMMAKER_DISABLE(pASM);
		return demand;
	}


    void gen4VarDefine(const GlobalVar & gvar);

	void gen4Stmts(const Function & fun, std::vector<StmtPtr> & stmts);

	void gen4Function(const Function & fun);
	
	void updateCurFileName(const std::string & curFileName);

public:
    void parse(const std::vector<FileStructurePtr> & fileStus);

    const std::vector<Error> & getErrList();
    
    bool hasErr() const;
};

#endif