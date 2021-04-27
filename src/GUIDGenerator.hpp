#ifndef __GUID_HEADER__
#define __GUID_HEADER__

#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <cstdlib>

#include "Statements.hpp"

class BasicGuidGen {
	static std::set<std::string> _set;
	static char _salts[26 + 26 + 10];
protected:
	static std::string apply(size_t bits = 8);
};

struct IBreakContinueAnchor {
	virtual const std::string * getBreak() const;
	virtual const std::string * getContinue() const;
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
			final override;
		
		virtual const std::string * getContinue() const 
			final override;
	};

	struct While : public IBreakContinueAnchor {
		std::string _lbl_while_begin_judge_continue; 
		std::string _lbl_while_end_break;         

		virtual const std::string * getBreak() const
			final override;
		
		virtual const std::string * getContinue() const
			final override;
	};

	struct DoWhile : public IBreakContinueAnchor {
		std::string _lbl_do_while_begin;		  
		std::string _lbl_do_while_judge_continue; 
		std::string _lbl_do_while_end_break; 

		virtual const std::string * getBreak() const
			final override;
			
		virtual const std::string * getContinue() const
			final override;
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
			final override;
		
		virtual const std::string * getContinue() const
			final override;
	};
};


class AdvGuidGen : public BasicGuidGen {
public:
	static LabelGroup::CondExpr applyLabelGroup4CondExpr();

	static LabelGroup::For applyLabelGroup4For();

	static LabelGroup::While applyLabelGroup4While();

	static LabelGroup::DoWhile applyLabelGroup4DoWhile();

	static LabelGroup::If applyLabelGroup4If(const std::vector<IfCondEntry> & condEntries);
	
	static LabelGroup::Switch applyLabelGroup4Switch(const std::vector<SwitchIndexEntry> & idxEntries);
};

#endif