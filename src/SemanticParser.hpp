#ifndef __SEM_PARSER_HEADER__
#define __SEM_PARSER_HEADER__

#include <memory>

#include "GUIDGenerator.hpp"
#include "ASM.hpp"
#include "SemanticErrorDriver.hpp"
#include "LocalCodeBinder.hpp"
#include "GlobalFuncMap.hpp"

#define ASMMAKER_ENABLE(x) \
AsmMaker * x, temp; \
if (_CHK == OnlyChk) { \
x = &temp; \
} else { \
x = &_asmk; \
}

#define ASMMAKER_DISABLE(x)

#define TYPE_PRODUCT2DEMAND(choice) \
if (true) { \
if (_CHK == OnlyGen) { \
	this->gen4ITC<OnlyGen>(choice, demand); \
} else { \
		ExprTypeConstraint::ChkResult _cr_ = ExprTypeConstraint::check<_CHK>(this, demand, (choice)); \
		if (ExprTypeConstraint::ChkResult::CHK_INCMP == _cr_) { \
				if ((choice) == "byte")\
				{ SEM_E((E_INCMP_TYPE_BYTE), pExpr); }\
				else if ((choice) == "boolean")       \
				{ SEM_E((E_INCMP_TYPE_BOOLEAN), pExpr); }                                      \
				else if ((choice) == "char")       \
				{ SEM_E((E_INCMP_TYPE_CHAR), pExpr); }                                          \
				else if ((choice) == "short")       \
				{ SEM_E((E_INCMP_TYPE_SHORT), pExpr); }                                          \
				else if ((choice) == "int")       \
				{ SEM_E((E_INCMP_TYPE_DW), pExpr); }                                          \
				else if ((choice) == "long")       \
				{ SEM_E((E_INCMP_TYPE_QW), pExpr); }                                          \
				else if ((choice) == "float")           \
				{ SEM_E((E_INCMP_TYPE_FLT), pExpr); }                                          \
				else if ((choice) == "double")                       \
				{ SEM_E((E_INCMP_TYPE_DBL), pExpr); }      \
				else if (VarType(choice).getDegree() > 0)                                         \
				{ SEM_E((E_INCMP_TYPE_REF), pExpr); }                                          \
		} \
} \
}

class SemParser {
	friend class Parser;
    friend class UniParser;
private:
	AsmMaker _asmk;

public:
	class ExprTypeConstraint {
	public:
		enum class ChkResult : int {
			CHK_INCMP,   // 完全不兼容，至少需要用户显式附加强制类型转换
			CHK_NEED_IC, // 需要生成
			CHK_OK,      // OK
		};

		static ChkResult _check(const VarType & demand, const VarType & choice) {
			if (demand == choice) {
				return ChkResult::CHK_OK;
			} else {
				if (demand == "void") {
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
						return ChkResult::CHK_OK;
					}
				} else if (demand == "byte") {
					if (choice == "char") {
						return ChkResult::CHK_OK;
					}
				} else if (demand == "boolean") {
					if (choice == "boolean") {
						return ChkResult::CHK_OK;
					}
				}
			}
			return ChkResult::CHK_INCMP;
		}
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

		template <int _CHK>
		static ChkResult check(SemParser * pThis, const VarType & demand, const VarType & choice) {
			ChkResult cr = _check(demand, choice);
			if (cr == ChkResult::CHK_OK) {
				
			} else if (cr == ChkResult::CHK_NEED_IC) {
				pThis->gen4ITC<_CHK>(choice, demand); // 生成隐式类型转换代码

			} else if (cr == ChkResult::CHK_INCMP) {

			} else {
				assert(0);
			}
			return cr;
		}

	};

	static const char * scalarVarType2VMStr(const VarType & choice) {
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
		} else {
			//assert(0); // 一般不会到此
			//return nullptr;
			return "VOID";
		}
	}

	bool checkMultiTypeMatching(const ExprPtr & pExpr,
		const std::vector<VarType> & orderedTypeMatchList, VarType & okChoice) {

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

		/*
		for (const auto & typeMatch : orderedTypeMatchList) {
			SEM_E_COUNT_RECOND;
			VarType fromChoice = gen4expr<OnlyChk>(pExpr, typeMatch);
			if (SEM_E_COUNT_HAS_INCREASED) {
				SEM_E_COUNT_CLEAR_DELTA;
				continue;
			} else {
				okChoice = typeMatch;
				return true;
			}
		}
		*/
		return false;
	}

	int getScalarTypeSize(const VarType & choice) const {
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

	// choiceIdx == -1 表示两类型大小相等
	bool check2ScalarTypeBigger(const VarType & choice1, const VarType & choice2, int & choiceIdx) {
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
	/*
	std::string compareScalarType(const VarType & choice1, const VarType & choice2) {
		int idx;
	}*/

	enum { ChkAndGen = 0, OnlyChk = 1, OnlyGen = 2 };

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
				pASM->append_PUSH_VAR(varName.toString().c_str()); // int32 堆句柄 heap addr
				//TYPE_PRODUCT2DEMAND(objTypeStr);
				
			} else if (objTypeStr == "int") {
				pASM->append_PUSH_VAR(varName.toString().c_str());
				//TYPE_PRODUCT2DEMAND(objTypeStr);
				
			} else if (objTypeStr == "long") {
				pASM->append_PUSH_VAR(varName.toString().c_str());
				//TYPE_PRODUCT2DEMAND(objTypeStr);
				
			} else if (objTypeStr == "float") {
				pASM->append_PUSH_VAR(varName.toString().c_str());
				//TYPE_PRODUCT2DEMAND(objTypeStr);

			} else if (objTypeStr == "double") {
				pASM->append_PUSH_VAR(varName.toString().c_str());
				//TYPE_PRODUCT2DEMAND(objTypeStr);

			} else if (objTypeStr == "short") {
				pASM->append_PUSH_VAR(varName.toString().c_str());
				//TYPE_PRODUCT2DEMAND(objTypeStr);
			
			} else if (objTypeStr == "char" || objTypeStr == "byte" || objTypeStr == "boolean") {
				pASM->append_PUSH_VAR(varName.toString().c_str());
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
	VarType gen4expr(const ExprPtr & pExpr, const VarType & demand) {
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

		case ExprOp::LEAF_INT_LITERAL: // int 字面量可以被赋值给更窄的数据类型
			/*
			if (demand == "byte") {
				pASM->append_IPUSH_B(pExpr->getLeaf().u._c);
				return VarType::buildFromStr("byte");
			} else if (demand == "char") {
				pASM->append_IPUSH_B(pExpr->getLeaf().u._c);
				return VarType::buildFromStr("char");
			} else if (demand == "short") {
				pASM->append_IPUSH_W(pExpr->getLeaf().u._s);
				return VarType::buildFromStr("short");
			} else {
				pASM->append_IPUSH_DW(pExpr->getLeaf().u._d);
				TYPE_PRODUCT2DEMAND("int");
				return VarType::buildFromStr("int");
			}*/
			pASM->append_IPUSH_DW(pExpr->getLeafScalar().u._d);
			TYPE_PRODUCT2DEMAND("int");
			return VarType::buildFromStr("int");

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
			if (demand == "float") {
				pASM->append_IPUSH_FLT(static_cast<float>(pExpr->getLeafScalar().u._lf));
				TYPE_PRODUCT2DEMAND("float");
				return VarType::buildFromStr("float");
			} else {
				pASM->append_IPUSH_DBL(pExpr->getLeafScalar().u._lf);
				TYPE_PRODUCT2DEMAND("double");
				return VarType::buildFromStr("double");
			}

		case ExprOp::LEAF_CHAR_LITERAL:
			pASM->append_IPUSH_B(pExpr->getLeafScalar().u._c);
			TYPE_PRODUCT2DEMAND("char");
			return VarType::buildFromStr("char");

		case ExprOp::LEAF_STRING_LITERAL:
			// string push 从 \0 开始倒序压栈
			pASM->append_VIPUSH(pExpr->getLeafScalar()._str);
			TYPE_PRODUCT2DEMAND("NSTRING");
			return VarType::buildFromStr("NSTRING");

		case ExprOp::LEAF_TRUE_LITERAL:
			pASM->append_IPUSH_B(1);
			TYPE_PRODUCT2DEMAND("boolean");
			return VarType::buildFromStr("boolean");

		case ExprOp::LEAF_FALSE_LITERAL:
			pASM->append_IPUSH_B(0);
			TYPE_PRODUCT2DEMAND("boolean");
			return VarType::buildFromStr("boolean");

		case ExprOp::LEAF_TYPE: // TODO 此处无需处理，类型转换交由上层处理
			break;

		case ExprOp::OPT_INDEX:
		{
			// 先对数组类型 demand 升阶，然后传递给 dot
			VarType newDemand = demand;
			newDemand.getDegreeRef()++;
			VarType choice = gen4ScopeId<_CHK>(pExpr->getSubExprPtr(0), newDemand);
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
			pASM->append_INDEX();

			choice.getDegreeRef()--;

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

			if (pLeftExpr->getOp() != ExprOp::LEAF_ID) {
				SEM_E(E_ASSIGN_LEFT_ID, pExpr);
				return demand;
			}

			VarType choice[2];
			
			choice[0] = gen4ScopeId<OnlyChk>(pLeftExpr, demand);
			bool result = checkMultiTypeMatching(pRightExpr,
									   { "boolean", "byte", "char", "short", "int", "long", "float", "double" },
									   choice[1]);
			if (result) {
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

			std::tuple<bool, VarName, VarType> tuple = 
				_saBinder.getDef(LocatedUtfString::make(pLeftExpr->getLeafScalar()._str,
					pLeftExpr->lineno(), pLeftExpr->colno()));

			if (std::get<0>(tuple))
				pASM->append_TOP_VAR(std::get<1>(tuple).toString().c_str());
			else
				SEM_E(E_ID_UNDEFINED, pLeftExpr);

			TYPE_PRODUCT2DEMAND(choice[0]);
			return choice[0];
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

			if (pLeftExpr->getOp() != ExprOp::LEAF_ID) {
				SEM_E(E_ASSIGN_LEFT_ID, pExpr);
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
			ExprPtr pNewRightExpr = Expression::newExpr(newOp,
				{ pLeftExpr, pRightExpr },
				pExpr->lineno(), pExpr->colno());

			ExprPtr pNewExpr = Expression::newExpr(ExprOp::OPT_ASSIGN,
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
			size_t cnt = pExpr->getExprCnt(), i;
			for (i = 0; i < cnt - 1; ++i) {
				gen4expr<_CHK == OnlyChk ? 
					OnlyChk : OnlyGen>(pExpr->getSubExprPtr(i), "void");
			}

			bool result = checkMultiTypeMatching(pExpr->getSubExprPtr(i),
				{ "boolean", "byte", "char", "short", "int", "long", "float", "double" },
				choice);
			if (result) {
				// 若需要检查，那么检查最后一个子表达式的类型是必须的
				gen4expr<_CHK>(pExpr->getSubExprPtr(i), demand);

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
			const std::string & idFunc = pFuncExprId->getLeafScalar()._str;
			Function * pFunc = GlobalFuncMap::getFunc(idFunc);
			if (!pFunc) {
				SEM_E(E_FUNC_UNDEFINED, pFuncExprId);
				return demand;

			} else {
				// 检查参数，并生成调用
				const auto & fal = pFunc->getFormalArgList();
				size_t argn = fal.size();
				if (pExpr->getExprCnt() - 1 != argn) {
					// 参数个数不匹配
					SEM_E(E_FUNC_ARG_NUM_MISMATCH, pFuncExprId);
					return demand;

				} else { 
					// 从右往左压栈
					for (size_t i = argn; i > 0; --i) {
						const VarType & argTypeDemand = fal[i - 1].getType();
						ExprPtr pArgExprId = pExpr->getSubExprPtr(i);
						gen4expr<_CHK>(pArgExprId, argTypeDemand);
					}

					pASM->append_CALL(idFunc);

					// 最终检查返回值类型约束
					const VarType & retType = pFunc->getRetType();
					TYPE_PRODUCT2DEMAND(retType);
					return retType;
				}
			}
		}

		default: {
			SEM_E(E_ILLEGAL_EXPR, pExpr);
			return demand;
		}

		// TODO 实现
		}

		ASMMAKER_DISABLE(pASM);
		return demand;
	}


    void gen4GlobalVar(const GlobalVar & gvar) {
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

		// 检查是否具备初始化表达式，如果具备，就生成初始化代码
		if (!gvar.hasInit())
			return;

		if (gvar.isListInit()) {
			// TODO 暂不支持表达式列表
			SEM_E(E_UNSUPPORTED_LIST_INIT, gvar.getName());
			return;

		} else {
			// 单个表达式
			TokenValue tv;
			tv._str = gvar.getName().toString();
			ExprPtr pExprLeafId = Expression::newLeafScalar(ExprOp::LEAF_ID, tv,
				gvar.getName().lineno(), gvar.getName().colno());
			ExprPtr pNewAssignExpr =
				Expression::newExpr(ExprOp::OPT_ASSIGN, { pExprLeafId, gvar.getInitExprNativePtr()->_pExpr },
					gvar.getName().lineno(), gvar.getName().colno());
			gen4expr<OnlyGen>(pNewAssignExpr, "void");
		}

    }

	void gen4Stmts(const Function & fun, std::vector<StmtPtr> & stmts) {
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
					bool result =
						_saBinder.bindDef(defVar.getName(), defVar.getType());
					if (!result) {
						SEM_E(E_VAR_REDEFINED, defVar.getName());
						continue;
					} else {


						std::tuple<bool, VarName, VarType> tuple =
							_saBinder.getDef(defVar.getName());
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

						// 检查是否具备初始化表达式，如果具备，就生成初始化代码
						if (!defVar.hasInit())
							continue;

						if (defVar.isListInit()) {
							// TODO 暂不支持表达式列表
							SEM_E(E_UNSUPPORTED_LIST_INIT, defVar.getName());
							continue;

						} else {
							// 单个表达式
							TokenValue tv;
							tv._str = defVar.getName().toString();
							ExprPtr pExprLeafId = Expression::newLeafScalar(ExprOp::LEAF_ID, tv,
								defVar.getName().lineno(), defVar.getName().colno());
							ExprPtr pNewAssignExpr =
								Expression::newExpr(ExprOp::OPT_ASSIGN, { pExprLeafId, defVar.getInitExprNativePtr()->_pExpr },
									defVar.getName().lineno(), defVar.getName().colno());
							gen4expr<OnlyGen>(pNewAssignExpr, "void");
						}
					}
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

			case StmtType::Empty: {
				break;
			}

			default: {
				assert(0);
			}
			}
		}
	}

	void gen4Function(const Function & fun) {
		_asmk.begin_FUNC(fun.getName().toString());

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
			_asmk.append_POP_VAR(realNameStr);
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
	
	void updateCurFileName(const std::string & curFileName) {
	    _filename = curFileName;
	    _sed.setFileName(curFileName);
	}

public:
    /*
    void parse(FileStructurePtr pFileStu) {
		
        // 解析全局变量
		_saBinder.makeScope();
		_asmk.begin_STATIC();
        for (const GlobalVar & gvar : pFileStu->_gvars) {
            gen4GlobalVar(gvar);
        }
		_asmk.end_STATIC();

        // 解析函数
        for (const Function & fun : pFileStu->_funs) {
			_saBinder.makeScope();
            gen4Function(fun);
			_saBinder.clearScope();
        }
		_saBinder.clearScope();
    }
    */
    
    void parse(const std::vector<FileStructurePtr> & fileStus) {
        // 解析全局变量
        _saBinder.makeScope();
        _asmk.begin_STATIC();
        for (const FileStructurePtr & pFileStu : fileStus) {
            updateCurFileName(pFileStu->_filename);
            for (const GlobalVar & gvar : pFileStu->_gvars) {
                gen4GlobalVar(gvar);
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

    const std::vector<Error> & getErrList() { return _sed.getErrList(); }
    bool hasErr() const { return _sed.hasErr(); }
};

#endif