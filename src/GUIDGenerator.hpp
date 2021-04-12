#ifndef __GUID_HEADER__
#define __GUID_HEADER__

#include <string>
#include <sstream>
#include <set>
#include <cstdlib>

class BasicGuidGen {
	static std::set<std::string> _set;
	static char _salts[26 + 26 + 10];
protected:
	static std::string apply(size_t bits = 8) {
		//srand(static_cast<unsigned int>(time(nullptr)));
		std::stringstream ss;
		for (size_t i = 0; i < bits; ++i) {
			ss << _salts[(rand() % sizeof(_salts))];
		}
		std::string s = ss.str();
		if (_set.find(s) != _set.cend()) {
			return apply(bits + 1);
		}
		_set.insert(s);
		return s;
	}

};

char BasicGuidGen::_salts[26 + 26 + 10] = {
		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'1','2','3','4','5','6','7','8','9','0'
};

std::set<std::string> BasicGuidGen::_set;

struct IBreakContinueAnchor {
	virtual const std::string * getBreak() const	{ return nullptr; }
	virtual const std::string * getContinue() const { return nullptr; }
};

struct LabelGroup {

	struct CondExpr {
		std::string _lbl_condexpr_true;
		std::string _lbl_condexpr_false;
		std::string _lbl_condexpr_end;
	};

	struct For : public IBreakContinueAnchor {
		std::string _lbl_for_begin; // 第一条语句插入点
		std::string _lbl_for_judge; // 第二条语句插入点
		std::string _lbl_for_continue; // 第三条语句插入点
		std::string _lbl_for_end_break; // for 循环跳出点

		virtual const std::string * getBreak() const 
			final override { return &_lbl_for_end_break; }
		virtual const std::string * getContinue() const 
			final override { return &_lbl_for_continue; }
	};

	struct While : public IBreakContinueAnchor {
		std::string _lbl_while_begin_judge_continue; 
		std::string _lbl_while_end_break;         

		virtual const std::string * getBreak() const
			final override { return &_lbl_while_end_break; }
		virtual const std::string * getContinue() const
			final override { return &_lbl_while_begin_judge_continue; }
	};

	struct DoWhile : public IBreakContinueAnchor {
		std::string _lbl_do_while_begin;		  
		std::string _lbl_do_while_judge_continue; 
		std::string _lbl_do_while_end_break; 

		virtual const std::string * getBreak() const
			final override { return &_lbl_do_while_end_break; }
		virtual const std::string * getContinue() const
			final override { return &_lbl_do_while_judge_continue; }
	};

	struct If {
		std::string _lbl_if_begin;
		std::vector<std::string> _lbl_if_casenodes;
		std::string _lbl_if_end;
	};

	struct Switch : public IBreakContinueAnchor {
		std::string _lbl_switch_begin;
		std::vector<std::string> _lbl_switch_casenodes;
		std::string _lbl_switch_end_break;

		virtual const std::string * getBreak() const
			final override { return &_lbl_switch_end_break; }
		virtual const std::string * getContinue() const
			final override { return nullptr; }
	};
};


class AdvGuidGen : public BasicGuidGen {
public:
	static LabelGroup::CondExpr applyLabelGroup4CondExpr() {
		LabelGroup::CondExpr ce;
		std::string s = apply();
		std::stringstream ss1, ss2, ss3;
		ss1 << "condexpr_" << s << "_true";
		ss2 << "condexpr_" << s << "_false";
		ss3 << "condexpr_" << s << "_end";
		ce._lbl_condexpr_true = ss1.str();
		ce._lbl_condexpr_false = ss2.str();
		ce._lbl_condexpr_end = ss3.str();
		return std::move(ce);
	}

	static LabelGroup::For applyLabelGroup4For() {
		LabelGroup::For f;
		std::string s = apply();
		std::stringstream ss1, ss2, ss3, ss4;
		ss1 << "for_" << s << "_begin";
		ss2 << "for_" << s << "_judge";
		ss3 << "for_" << s << "_continue";
		ss4 << "for_" << s << "_end_break";
		f._lbl_for_begin = ss1.str();
		f._lbl_for_judge = ss2.str();
		f._lbl_for_continue = ss3.str();
		f._lbl_for_end_break = ss4.str();
		return std::move(f);
	}

	static LabelGroup::While applyLabelGroup4While() {
		LabelGroup::While w;
		std::string s = apply();
		std::stringstream ss1, ss2;
		ss1 << "while_" << s << "_begin_judge_continue";
		ss2 << "while_" << s << "_end_break";
		w._lbl_while_begin_judge_continue = ss1.str();
		w._lbl_while_end_break = ss2.str();
		return std::move(w);
	}

	static LabelGroup::DoWhile applyLabelGroup4DoWhile() {
		LabelGroup::DoWhile dw;
		std::string s = apply();
		std::stringstream ss1, ss2, ss3;
		ss1 << "do_while_" << s << "_begin";
		ss2 << "do_while_" << s << "_judge_continue";
		ss3 << "do_while_" << s << "_end_break";
		dw._lbl_do_while_begin = ss1.str();
		dw._lbl_do_while_judge_continue = ss2.str();
		dw._lbl_do_while_end_break = ss3.str();
		return std::move(dw);
	}

	static LabelGroup::If applyLabelGroup4If(const std::vector<IfCondEntry> & condEntries) {
		LabelGroup::If lgIf;
		std::string s = apply();

		std::stringstream ss1, ss2;
		ss1 << "if_" << s << "_begin";
		lgIf._lbl_if_begin = ss1.str();

		size_t sz = condEntries.size();
		for (size_t i = 0; i < sz; ++i) {
			std::stringstream ss;
			ss << "if_" << s << "_entry_" << i;
			lgIf._lbl_if_casenodes.push_back(ss.str());
		}
		
		ss2 << "if_" << s << "_end";
		lgIf._lbl_if_end = ss2.str();

		return std::move(lgIf);
	}

	
	static LabelGroup::Switch applyLabelGroup4Switch(const std::vector<SwitchIndexEntry> & idxEntries) {
		LabelGroup::Switch lgSwitch;
		std::string s = apply();

		std::stringstream ss1, ss2;
		ss1 << "switch_" << s << "_begin";
		lgSwitch._lbl_switch_begin = ss1.str();

		size_t sz = idxEntries.size();
		for (size_t i = 0; i < sz; ++i) {
			std::stringstream ss;
			ss << "switch_" << s << "_entry_" << i;
			lgSwitch._lbl_switch_casenodes.push_back(ss.str());
		}

		ss2 << "switch_" << s << "_end_break";
		lgSwitch._lbl_switch_end_break = ss2.str();

		return std::move(lgSwitch);
	}
};

#endif