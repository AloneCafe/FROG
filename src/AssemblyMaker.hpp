#ifndef __ASM_HEADER__
#define __ASM_HEADER__

#include <sstream>
#include "UTFSequence.hpp"

class AsmMaker {
protected:
    static constexpr const char * TAB = "\t";
    static constexpr const char * SPACE = " ";
    static constexpr const char * SPLIT = ",";
    static constexpr const char * LABEL_PREFIX = "#";
    static constexpr const char * LABEL_SUFFIX = ":";
    static constexpr const char * STMT_SUFFIX = ";";

protected:
    std::stringstream _context;

public:
    AsmMaker() = default;
    AsmMaker(const AsmMaker &) = default;
    AsmMaker & operator=(const AsmMaker &) = default;
    
    std::stringstream & getContextRef() { return _context; }

public: /* 指令参数皆是从右往左压栈 */
    
    void append_INDEX() {
        _context << TAB << "INDEX" << STMT_SUFFIX << std::endl;
    }
    
    void begin_STATIC() {
        _context << ".STATIC" << STMT_SUFFIX << std::endl;
    }
    
    void end_STATIC() {
        _context << ".END" << STMT_SUFFIX << std::endl << std::endl;
    }
    
    void begin_FUNC(const std::string & fname) {
        _context << ".FUNC" << SPACE << fname << STMT_SUFFIX << std::endl;
    }
    
    void end_FUNC() {
        _context << ".END" << STMT_SUFFIX << std::endl << std::endl;
    }
    
    void append_LABEL(const std::string & label) {
        _context << LABEL_PREFIX << label << LABEL_SUFFIX << std::endl;
    }
    
    void append_MKLIST_N_QW(int degree) {
        _context << TAB << "MKLIST " << degree << " QW" << STMT_SUFFIX << std::endl;
    }
    
    void append_MKLIST_N_DW(int degree) {
        _context << TAB << "MKLIST " << degree << " DW" << STMT_SUFFIX << std::endl;
    }
    
    void append_MKLIST_N_W(int degree) {
        _context << TAB << "MKLIST " << degree << " W" << STMT_SUFFIX << std::endl;
    }
    
    void append_MKLIST_N_B(int degree) {
        _context << TAB << "MKLIST " << degree << " B" << STMT_SUFFIX << std::endl;
    }
    
    void append_MKLIST_N_FLT(int degree) {
        _context << TAB << "MKLIST " << degree << " FLT" << STMT_SUFFIX << std::endl;
    }
    
    void append_MKLIST_N_DBL(int degree) {
        _context << TAB << "MKLIST " << degree << " DBL" << STMT_SUFFIX << std::endl;
    }
    
    /* 立即数传送 */
    void append_IPUSH_QW(int64_t qw) {
        _context << TAB << "IPUSH QW" << SPACE << qw << STMT_SUFFIX << std::endl;
    }
    void append_IPUSH_DW(int32_t dw) {
        _context << TAB << "IPUSH DW" << SPACE << static_cast<int64_t>(dw) << STMT_SUFFIX << std::endl;
    }
    void append_IPUSH_W(int16_t w) {
        _context << TAB << "IPUSH W" << SPACE << static_cast<int64_t>(w) << STMT_SUFFIX << std::endl;
    }
    void append_IPUSH_B(int8_t b) {
        _context << TAB << "IPUSH B" << SPACE << static_cast<int64_t>(b) << STMT_SUFFIX << std::endl;
    }
    void append_IPUSH_FLT(float flt) {
        _context << TAB << "IPUSH FLT" << SPACE << flt << STMT_SUFFIX << std::endl;
    }
    void append_IPUSH_DBL(double dbl) {
        _context << TAB << "IPUSH DBL" << SPACE << dbl << STMT_SUFFIX << std::endl;
    }
    void append_VIPUSH(const std::vector<int8_t> l) {
        assert(!l.empty());
        _context << TAB << "VIPUSH" << SPACE << l[0];
        size_t sz = l.size();
        for (size_t i = 1; i < sz; ++i)
            _context << "," << SPACE << l[i];
        _context << STMT_SUFFIX << std::endl;
    }
    void append_VIPUSH(const std::string & s) {
        size_t sz = s.size();
        _context << TAB << "VIPUSH" << SPACE << '\'' << s[0] << '\'';
        for (size_t i = 1; i < sz; ++i)
            _context << "," << SPACE << '\'' << s[i] << '\'';
        _context << '0' << STMT_SUFFIX << std::endl;
    }
    
    void append_MKHEAP(/* int32_t sz */) {
        _context << TAB << "MKHEAP" << STMT_SUFFIX << std::endl;
    }
    
    /* 作用域栈 */
    void append_PUSH_VAR(const std::string & varName) {
        _context << TAB << "PUSH" << SPACE << varName << STMT_SUFFIX << std::endl;
    }
    
    void append_POP_VAR(const std::string & varName) {
        _context << TAB << "POP" << SPACE << varName << STMT_SUFFIX << std::endl;
    }
    
    void append_TOP_VAR(const std::string & varName) {
        _context << TAB << "TOP" << SPACE << varName << STMT_SUFFIX << std::endl;
    }
    
    /*
    void append_DEF_VAR(const std::string & varName) {
        // 此处必然保证变量唯一性
        _context << TAB << "DEF" << SPACE << varName << STMT_SUFFIX << std::endl;
    }*/
    
    void append_DEF_VAR_QW(const std::string & varName) {
        _context << TAB << "DEF QW" << SPACE << varName << STMT_SUFFIX << std::endl;
    }
    void append_DEF_VAR_DW(const std::string & varName) {
        _context << TAB << "DEF DW" << SPACE << varName << STMT_SUFFIX << std::endl;
    }
    void append_DEF_VAR_W(const std::string & varName) {
        _context << TAB << "DEF W" << SPACE << varName << STMT_SUFFIX << std::endl;
    }
    void append_DEF_VAR_B(const std::string & varName) {
        _context << TAB << "DEF B" << SPACE << varName << STMT_SUFFIX << std::endl;
    }
    void append_DEF_VAR_FLT(const std::string & varName) {
        _context << TAB << "DEF FLT" << SPACE << varName << STMT_SUFFIX << std::endl;
    }
    void append_DEF_VAR_DBL(const std::string & varName) {
        _context << TAB << "DEF DBL" << SPACE << varName << STMT_SUFFIX << std::endl;
    }
    
    /* 堆 */
    void append_HPUSH_QW(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH QW" << STMT_SUFFIX << std::endl;
    }
    void append_HPUSH_DW(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH DW" << STMT_SUFFIX << std::endl;
    }
    void append_HPUSH_W(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH W" << STMT_SUFFIX << std::endl;
    }
    void append_HPUSH_B(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH B" << STMT_SUFFIX << std::endl;
    }
    void append_HPUSH_FLT(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH FLT" << STMT_SUFFIX << std::endl;
    }
    void append_HPUSH_DBL(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH DBL" << STMT_SUFFIX << std::endl;
    }
    void append_HPUSH_STR(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH STR" << STMT_SUFFIX << std::endl;
    }
    void append_VHPUSH(/* int32_t off, int32_t sz, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "VHPUSH" << STMT_SUFFIX << std::endl;
    }
    
    void append_HPOP_QW(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP QW" << STMT_SUFFIX << std::endl;
    }
    void append_HPOP_DW(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP DW" << STMT_SUFFIX << std::endl;
    }
    void append_HPOP_W(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP W" << STMT_SUFFIX << std::endl;
    }
    void append_HPOP_B(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP B" << STMT_SUFFIX << std::endl;
    }
    void append_HPOP_FLT(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP FLT" << STMT_SUFFIX << std::endl;
    }
    void append_HPOP_DBL(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP DBL" << STMT_SUFFIX << std::endl;
    }
    void append_HPOP_STR(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP STR" << STMT_SUFFIX << std::endl;
    }
    void append_VHPOP(/* int32_t off, int32_t sz, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "VHPOP" << STMT_SUFFIX << std::endl;
    }
    
    void append_ADD_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "ADD QW" << STMT_SUFFIX << std::endl;
    }
    void append_ADD_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "ADD DW" << STMT_SUFFIX << std::endl;
    }
    void append_ADD_W(/* int16_t a, int16_t b */) {
        _context << TAB << "ADD W" << STMT_SUFFIX << std::endl;
    }
    void append_ADD_B(/* int8_t a, int8_t b */) {
        _context << TAB << "ADD B" << STMT_SUFFIX << std::endl;
    }
    void append_ADD_FLT(/* float a, float b */) {
        _context << TAB << "ADD FLT" << STMT_SUFFIX << std::endl;
    }
    void append_ADD_DBL(/* double a, double b */) {
        _context << TAB << "ADD DBL" << STMT_SUFFIX << std::endl;
    }
    
    void append_SUB_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "SUB QW" << STMT_SUFFIX << std::endl;
    }
    void append_SUB_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "SUB DW" << STMT_SUFFIX << std::endl;
    }
    void append_SUB_W(/* int16_t a, int16_t b */) {
        _context << TAB << "SUB W" << STMT_SUFFIX << std::endl;
    }
    void append_SUB_B(/* int8_t a, int8_t b */) {
        _context << TAB << "SUB B" << STMT_SUFFIX << std::endl;
    }
    void append_SUB_FLT(/* float a, float b */) {
        _context << TAB << "SUB FLT" << STMT_SUFFIX << std::endl;
    }
    void append_SUB_DBL(/* double a, double b */) {
        _context << TAB << "SUB DBL" << STMT_SUFFIX << std::endl;
    }
    
    void append_MUL_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "MUL QW" << STMT_SUFFIX << std::endl;
    }
    void append_MUL_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "MUL DW" << STMT_SUFFIX << std::endl;
    }
    void append_MUL_W(/* int16_t a, int16_t b */) {
        _context << TAB << "MUL W" << STMT_SUFFIX << std::endl;
    }
    void append_MUL_B(/* int8_t a, int8_t b */) {
        _context << TAB << "MUL B" << STMT_SUFFIX << std::endl;
    }
    void append_MUL_FLT(/* float a, float b */) {
        _context << TAB << "MUL FLT" << STMT_SUFFIX << std::endl;
    }
    void append_MUL_DBL(/* double a, double b */) {
        _context << TAB << "MUL DBL" << STMT_SUFFIX << std::endl;
    }
    
    void append_DIV_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "DIV QW" << STMT_SUFFIX << std::endl;
    }
    void append_DIV_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "DIV DW" << STMT_SUFFIX << std::endl;
    }
    void append_DIV_W(/* int16_t a, int16_t b */) {
        _context << TAB << "DIV W" << STMT_SUFFIX << std::endl;
    }
    void append_DIV_B(/* int8_t a, int8_t b */) {
        _context << TAB << "DIV B" << STMT_SUFFIX << std::endl;
    }
    void append_DIV_FLT(/* float a, float b */) {
        _context << TAB << "DIV FLT" << STMT_SUFFIX << std::endl;
    }
    void append_DIV_DBL(/* double a, double b */) {
        _context << TAB << "DIV DBL" << STMT_SUFFIX << std::endl;
    }
    
    void append_MOD_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "MOD QW" << STMT_SUFFIX << std::endl;
    }
    void append_MOD_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "MOD DW" << STMT_SUFFIX << std::endl;
    }
    void append_MOD_W(/* int16_t a, int16_t b */) {
        _context << TAB << "MOD W" << STMT_SUFFIX << std::endl;
    }
    void append_MOD_B(/* int8_t a, int8_t b */) {
        _context << TAB << "MOD B" << STMT_SUFFIX << std::endl;
    }
    void append_MOD_FLT(/* float a, float b */) {
        _context << TAB << "MOD FLT" << STMT_SUFFIX << std::endl;
    }
    void append_MOD_DBL(/* double a, double b */) {
        _context << TAB << "MOD DBL" << STMT_SUFFIX << std::endl;
    }
    
    void append_LT_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "LT QW" << STMT_SUFFIX << std::endl;
    }
    void append_LT_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "LT DW" << STMT_SUFFIX << std::endl;
    }
    void append_LT_W(/* int16_t a, int16_t b */) {
        _context << TAB << "LT W" << STMT_SUFFIX << std::endl;
    }
    void append_LT_B(/* int8_t a, int8_t b */) {
        _context << TAB << "LT B" << STMT_SUFFIX << std::endl;
    }
    void append_LT_FLT(/* float a, float b */) {
        _context << TAB << "LT FLT" << STMT_SUFFIX << std::endl;
    }
    void append_LT_DBL(/* double a, double b */) {
        _context << TAB << "LT DBL" << STMT_SUFFIX << std::endl;
    }
    
    void append_LE_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "LE QW" << STMT_SUFFIX << std::endl;
    }
    void append_LE_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "LE DW" << STMT_SUFFIX << std::endl;
    }
    void append_LE_W(/* int16_t a, int16_t b */) {
        _context << TAB << "LE W" << STMT_SUFFIX << std::endl;
    }
    void append_LE_B(/* int8_t a, int8_t b */) {
        _context << TAB << "LE B" << STMT_SUFFIX << std::endl;
    }
    void append_LE_FLT(/* float a, float b */) {
        _context << TAB << "LE FLT" << STMT_SUFFIX << std::endl;
    }
    void append_LE_DBL(/* double a, double b */) {
        _context << TAB << "LE DBL" << STMT_SUFFIX << std::endl;
    }
    
    void append_EQ_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "EQ QW" << STMT_SUFFIX << std::endl;
    }
    void append_EQ_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "EQ DW" << STMT_SUFFIX << std::endl;
    }
    void append_EQ_W(/* int16_t a, int16_t b */) {
        _context << TAB << "EQ W" << STMT_SUFFIX << std::endl;
    }
    void append_EQ_B(/* int8_t a, int8_t b */) {
        _context << TAB << "EQ B" << STMT_SUFFIX << std::endl;
    }
    void append_EQ_FLT(/* float a, float b */) {
        _context << TAB << "EQ FLT" << STMT_SUFFIX << std::endl;
    }
    void append_EQ_DBL(/* double a, double b */) {
        _context << TAB << "EQ DBL" << STMT_SUFFIX << std::endl;
    }
    
    void append_NE_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "NE QW" << STMT_SUFFIX << std::endl;
    }
    void append_NE_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "NE DW" << STMT_SUFFIX << std::endl;
    }
    void append_NE_W(/* int16_t a, int16_t b */) {
        _context << TAB << "NE W" << STMT_SUFFIX << std::endl;
    }
    void append_NE_B(/* int8_t a, int8_t b */) {
        _context << TAB << "NE B" << STMT_SUFFIX << std::endl;
    }
    void append_NE_FLT(/* float a, float b */) {
        _context << TAB << "NE FLT" << STMT_SUFFIX << std::endl;
    }
    void append_NE_DBL(/* double a, double b */) {
        _context << TAB << "NE DBL" << STMT_SUFFIX << std::endl;
    }
    
    void append_GE_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "GE QW" << STMT_SUFFIX << std::endl;
    }
    void append_GE_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "GE DW" << STMT_SUFFIX << std::endl;
    }
    void append_GE_W(/* int16_t a, int16_t b */) {
        _context << TAB << "GE W" << STMT_SUFFIX << std::endl;
    }
    void append_GE_B(/* int8_t a, int8_t b */) {
        _context << TAB << "GE B" << STMT_SUFFIX << std::endl;
    }
    void append_GE_FLT(/* float a, float b */) {
        _context << TAB << "GE FLT" << STMT_SUFFIX << std::endl;
    }
    void append_GE_DBL(/* double a, double b */) {
        _context << TAB << "GE DBL" << STMT_SUFFIX << std::endl;
    }
    
    void append_GT_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "GT QW" << STMT_SUFFIX << std::endl;
    }
    void append_GT_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "GT DW" << STMT_SUFFIX << std::endl;
    }
    void append_GT_W(/* int16_t a, int16_t b */) {
        _context << TAB << "GT W" << STMT_SUFFIX << std::endl;
    }
    void append_GT_B(/* int8_t a, int8_t b */) {
        _context << TAB << "GT B" << STMT_SUFFIX << std::endl;
    }
    void append_GT_FLT(/* float a, float b */) {
        _context << TAB << "GT FLT" << STMT_SUFFIX << std::endl;
    }
    void append_GT_DBL(/* double a, double b */) {
        _context << TAB << "GT DBL" << STMT_SUFFIX << std::endl;
    }
    
    void append_LAND(/* bool a, bool b */) {
        _context << TAB << "LAND" << STMT_SUFFIX << std::endl;
    }
    void append_LOR(/* bool a, bool b */) {
        _context << TAB << "LOR" << STMT_SUFFIX << std::endl;
    }
    void append_LNOT(/* bool a */) {
        _context << TAB << "LNOT" << STMT_SUFFIX << std::endl;
    }
    
    void append_BAND_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "BAND QW" << STMT_SUFFIX << std::endl;
    }
    void append_BAND_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "BAND DW" << STMT_SUFFIX << std::endl;
    }
    void append_BAND_W(/* int16_t a, int16_t b */) {
        _context << TAB << "BAND W" << STMT_SUFFIX << std::endl;
    }
    void append_BAND_B(/* int8_t a, int8_t b */) {
        _context << TAB << "BAND B" << STMT_SUFFIX << std::endl;
    }
    
    void append_BOR_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "BOR QW" << STMT_SUFFIX << std::endl;
    }
    void append_BOR_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "BOR DW" << STMT_SUFFIX << std::endl;
    }
    void append_BOR_W(/* int16_t a, int16_t b */) {
        _context << TAB << "BOR W" << STMT_SUFFIX << std::endl;
    }
    void append_BOR_B(/* int8_t a, int8_t b */) {
        _context << TAB << "BOR B" << STMT_SUFFIX << std::endl;
    }
    
    void append_BXOR_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "BXOR QW" << STMT_SUFFIX << std::endl;
    }
    void append_BXOR_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "BXOR DW" << STMT_SUFFIX << std::endl;
    }
    void append_BXOR_W(/* int16_t a, int16_t b */) {
        _context << TAB << "BXOR W" << STMT_SUFFIX << std::endl;
    }
    void append_BXOR_B(/* int8_t a, int8_t b */) {
        _context << TAB << "BXOR B" << STMT_SUFFIX << std::endl;
    }
    
    void append_BNOT_QW(/* int64_t a */) {
        _context << TAB << "BNOT QW" << STMT_SUFFIX << std::endl;
    }
    void append_BNOT_DW(/* int32_t a */) {
        _context << TAB << "BNOT DW" << STMT_SUFFIX << std::endl;
    }
    void append_BNOT_W(/* int16_t a */) {
        _context << TAB << "BNOT W" << STMT_SUFFIX << std::endl;
    }
    void append_BNOT_B(/* int8_t a */) {
        _context << TAB << "BNOT B" << STMT_SUFFIX << std::endl;
    }
    
    void append_SHL_QW(/* int64_t a, int8_t bits */) {
        _context << TAB << "SHL QW" << STMT_SUFFIX << std::endl;
    }
    void append_SHL_DW(/* int32_t a, int8_t bits */) {
        _context << TAB << "SHL DW" << STMT_SUFFIX << std::endl;
    }
    void append_SHL_W(/* int16_t a, int8_t bits */) {
        _context << TAB << "SHL W" << STMT_SUFFIX << std::endl;
    }
    void append_SHL_B(/* int8_t a, int8_t bits */) {
        _context << TAB << "SHL B" << STMT_SUFFIX << std::endl;
    }
    
    void append_SHR_QW(/* int64_t a, int8_t bits */) {
        _context << TAB << "SHR QW" << STMT_SUFFIX << std::endl;
    }
    void append_SHR_DW(/* int32_t a, int8_t bits */) {
        _context << TAB << "SHR DW" << STMT_SUFFIX << std::endl;
    }
    void append_SHR_W(/* int16_t a, int8_t bits */) {
        _context << TAB << "SHR W" << STMT_SUFFIX << std::endl;
    }
    void append_SHR_B(/* int8_t a, int8_t bits */) {
        _context << TAB << "SHR B" << STMT_SUFFIX << std::endl;
    }
    
    void append_SHRZ_QW(/* int64_t a, int8_t bits */) {
        _context << TAB << "SHRZ QW" << STMT_SUFFIX << std::endl;
    }
    void append_SHRZ_DW(/* int32_t a, int8_t bits */) {
        _context << TAB << "SHRZ DW" << STMT_SUFFIX << std::endl;
    }
    void append_SHRZ_W(/* int16_t a, int8_t bits */) {
        _context << TAB << "SHRZ W" << STMT_SUFFIX << std::endl;
    }
    void append_SHRZ_B(/* int8_t a, int8_t bits */) {
        _context << TAB << "SHRZ B" << STMT_SUFFIX << std::endl;
    }
    
    void append_NEG_QW(/* int64_t a */) {
        _context << TAB << "NEG QW" << STMT_SUFFIX << std::endl;
    }
    void append_NEG_DW(/* int32_t a */) {
        _context << TAB << "NEG DW" << STMT_SUFFIX << std::endl;
    }
    void append_NEG_W(/* int16_t a */) {
        _context << TAB << "NEG W" << STMT_SUFFIX << std::endl;
    }
    void append_NEG_B(/* int8_t a */) {
        _context << TAB << "NEG B" << STMT_SUFFIX << std::endl;
    }
    void append_NEG_FLT(/* float a */) {
        _context << TAB << "NEG FLT" << STMT_SUFFIX << std::endl;
    }
    void append_NEG_DBL(/* double a */) {
        _context << TAB << "NEG DBL" << STMT_SUFFIX << std::endl;
    }
    
    void append_RSZ(const char * from, const char * to /* a */) {
        _context << TAB << "RSZ" << SPACE << from << SPACE << to << STMT_SUFFIX << std::endl;
    }
    
    void append_HALT() {
        _context << TAB << "HALT" << STMT_SUFFIX << std::endl;
    }
    void append_BREAK() {
        _context << TAB << "BREAK" << STMT_SUFFIX << std::endl;
    }
    // localCodeAddr 汇编前是标号 label
    void append_J(/* int32_t localCodeAddr */ const std::string & label) {
        _context << TAB << "J" << SPACE << LABEL_PREFIX << label << STMT_SUFFIX << std::endl;
    }
    // localCodeAddr 汇编前是标号 label
    void append_JT(/* int32_t localCodeAddr, bool condVal */ const std::string & label) {
        _context << TAB << "JT" << SPACE << LABEL_PREFIX << label << STMT_SUFFIX << std::endl;
    }
    // localCodeAddr 汇编前是标号 label
    void append_JF(/* int32_t localCodeAddr, bool condVal */ const std::string & label) {
        _context << TAB << "JF" << SPACE << LABEL_PREFIX << label << STMT_SUFFIX << std::endl;
    }
    
    // localFuncCodeAddr 汇编前是函数全名 fn
    void append_CALL(/* int32_t localFuncCodeAddr, int32_t thisRef, ..., int32_t funcRetCodeAddr */
            const std::string & fn) {
        _context << TAB << "CALL" << SPACE << fn << STMT_SUFFIX << std::endl;
    }
    void append_NRET() {
        _context << TAB << "NRET" << STMT_SUFFIX << std::endl;
    }
    void append_RET_QW() {
        _context << TAB << "RET QW" << SPACE << STMT_SUFFIX << std::endl;
    }
    void append_RET_DW() {
        _context << TAB << "RET DW" << SPACE << STMT_SUFFIX << std::endl;
    }
    void append_RET_W() {
        _context << TAB << "RET W" << SPACE << STMT_SUFFIX << std::endl;
    }
    void append_RET_B() {
        _context << TAB << "RET B" << SPACE << STMT_SUFFIX << std::endl;
    }
    void append_RET_FLT() {
        _context << TAB << "RET FLT" << SPACE << STMT_SUFFIX << std::endl;
    }
    void append_RET_DBL() {
        _context << TAB << "RET DBL" << SPACE << STMT_SUFFIX << std::endl;
    }
    void append_NOP() {
        _context << TAB << "NOP" << STMT_SUFFIX << std::endl;
    }
    
    /* TODO 尚未实现
    void append_INVOKE_CALL() {

    }

    void append_INVOKE_VAR() {

    }

    void append_LASTERR() {

    }
    */
    
    void eraseLast() {
        std::string s;
    }
    
};

class AdvancedAsmMaker : public AsmMaker {

public:

};

#endif
