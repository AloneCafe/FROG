#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <cstdlib>

#include "Statements.hpp"
#include "GUIDGenerator.hpp"

std::set<std::string> BasicGuidGen::_set;

std::string BasicGuidGen::apply(uint32_t bits) {
    //srand(static_cast<unsigned int>(time(nullptr)));
    std::stringstream ss;
    for (uint32_t i = 0; i < bits; ++i) {
        ss << _salts[(rand() % sizeof(_salts))];
    }
    std::string s = ss.str();
    if (_set.find(s) != _set.cend()) {
        return apply(bits + 1);
    }
    _set.insert(s);
    return s;
}

char BasicGuidGen::_salts[26 + 26 + 10] = {
        'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
        'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
        '1','2','3','4','5','6','7','8','9','0'
};

const std::string * IBreakContinueAnchor::getBreak() const {
    return nullptr;
}

const std::string * IBreakContinueAnchor::getContinue() const {
    return nullptr;
}




LabelGroup::CondExpr AdvGuidGen::applyLabelGroup4CondExpr() {
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

LabelGroup::For AdvGuidGen::applyLabelGroup4For() {
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

LabelGroup::While AdvGuidGen::applyLabelGroup4While() {
    LabelGroup::While w;
    std::string s = apply();
    std::stringstream ss1, ss2;
    ss1 << "while_" << s << "_begin_judge_continue";
    ss2 << "while_" << s << "_end_break";
    w._lbl_while_begin_judge_continue = ss1.str();
    w._lbl_while_end_break = ss2.str();
    return std::move(w);
}

LabelGroup::DoWhile AdvGuidGen::applyLabelGroup4DoWhile() {
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

LabelGroup::If AdvGuidGen::applyLabelGroup4If(const std::vector<IfCondEntry> & condEntries) {
    LabelGroup::If lgIf;
    std::string s = apply();
    
    std::stringstream ss1, ss2;
    ss1 << "if_" << s << "_begin";
    lgIf._lbl_if_begin = ss1.str();
    
    uint32_t sz = condEntries.size();
    for (uint32_t i = 0; i < sz; ++i) {
        std::stringstream ss;
        ss << "if_" << s << "_entry_" << i;
        lgIf._lbl_if_casenodes.push_back(ss.str());
    }
    
    ss2 << "if_" << s << "_end";
    lgIf._lbl_if_end = ss2.str();
    
    return std::move(lgIf);
}


LabelGroup::Switch AdvGuidGen::applyLabelGroup4Switch(const std::vector<SwitchIndexEntry> & idxEntries) {
    LabelGroup::Switch lgSwitch;
    std::string s = apply();
    
    std::stringstream ss1, ss2;
    ss1 << "switch_" << s << "_begin";
    lgSwitch._lbl_switch_begin = ss1.str();
    
    uint32_t sz = idxEntries.size();
    for (uint32_t i = 0; i < sz; ++i) {
        std::stringstream ss;
        ss << "switch_" << s << "_entry_" << i;
        lgSwitch._lbl_switch_casenodes.push_back(ss.str());
    }
    
    ss2 << "switch_" << s << "_end_break";
    lgSwitch._lbl_switch_end_break = ss2.str();
    
    return std::move(lgSwitch);
}
const std::string * LabelGroup::For::getBreak() const {
    return &_lbl_for_end_break;
}
const std::string * LabelGroup::For::getContinue() const {
    return &_lbl_for_continue;
}
const std::string * LabelGroup::While::getBreak() const {
    return &_lbl_while_end_break;
}
const std::string * LabelGroup::While::getContinue() const {
    return &_lbl_while_begin_judge_continue;
}
const std::string * LabelGroup::DoWhile::getBreak() const {
    return &_lbl_do_while_end_break;
}
const std::string * LabelGroup::DoWhile::getContinue() const {
    return &_lbl_do_while_judge_continue;
}
const std::string * LabelGroup::Switch::getBreak() const {
    return &_lbl_switch_end_break;
}
const std::string * LabelGroup::Switch::getContinue() const {
    return nullptr;
}
