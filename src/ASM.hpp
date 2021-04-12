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

protected:
    std::stringstream _context;

public:
    AsmMaker() = default;
    AsmMaker(const AsmMaker &) = default;
    AsmMaker & operator=(const AsmMaker &) = default;

    std::stringstream & getContextRef() { return _context; }

public: /* 指令参数皆是从右往左压栈 */

    void append_INDEX() {
        _context << TAB << "INDEX" << std::endl;
    }

    void begin_STATIC() {
        _context << ".STATIC" << std::endl;
    }

    void end_STATIC() {
        _context << ".END STATIC" << std::endl << std::endl;
    }

	void begin_FUNC(const std::string & fname) {
		_context << ".FUNC" << SPACE << fname << std::endl;
	}

	void end_FUNC() {
		_context << ".END FUNC" << std::endl << std::endl;
	}

    void append_LABEL(const std::string & label) {
        _context << LABEL_PREFIX << label << LABEL_SUFFIX << std::endl;
    }

    /* 立即数传送 */
    void append_IPUSH_QW(int64_t qw) {
        _context << TAB << "IPUSH QW" << SPACE << qw << std::endl;
    }
    void append_IPUSH_DW(int32_t dw) {
        _context << TAB << "IPUSH DW" << SPACE << static_cast<int64_t>(dw) << std::endl;
    }
    void append_IPUSH_W(int16_t w) {
        _context << TAB << "IPUSH W" << SPACE << static_cast<int64_t>(w) << std::endl;
    }
    void append_IPUSH_B(int8_t b) {
        _context << TAB << "IPUSH B" << SPACE << static_cast<int64_t>(b) << std::endl;
    }
    void append_IPUSH_FLT(float flt) {
        _context << TAB << "IPUSH FLT" << SPACE << flt << std::endl;
    }
    void append_IPUSH_DBL(double dbl) {
        _context << TAB << "IPUSH DBL" << SPACE << dbl << std::endl;
    }
    void append_VIPUSH(const std::vector<int8_t> l) {
        assert(!l.empty());
        _context << TAB << "VIPUSH" << SPACE << l[0];
        size_t sz = l.size();
        for (size_t i = 1; i < sz; ++i)
            _context << "," << SPACE << l[i];
        _context << std::endl;
    }
    void append_VIPUSH(const std::string & s) {
        size_t sz = s.size();
        _context << TAB << "VIPUSH" << SPACE << '\'' << s[0] << '\'';
        for (size_t i = 1; i < sz; ++i)
            _context << "," << SPACE << '\'' << s[i] << '\'';
        _context << '0' << std::endl;
    }

    void append_MKHEAP(/* int32_t sz */) {
        _context << TAB << "MKHEAP" << std::endl;
    }

    /* 作用域栈 */
    void append_PUSH_VAR(const std::string & varName) {
        _context << TAB << "PUSH" << SPACE << varName << std::endl;
    }

    void append_POP_VAR(const std::string & varName) {
        _context << TAB << "POP" << SPACE << varName << std::endl;
    }

    void append_TOP_VAR(const std::string & varName) {
		_context << TAB << "TOP" << SPACE << varName << std::endl;
	}

    /*
    void append_DEF_VAR(const std::string & varName) {
        // 此处必然保证变量唯一性
        _context << TAB << "DEF" << SPACE << varName << std::endl;
    }*/

	void append_DEF_VAR_QW(const std::string & varName) {
        _context << TAB << "DEF QW" << SPACE << varName << std::endl;
	}
	void append_DEF_VAR_DW(const std::string & varName) {
        _context << TAB << "DEF DW" << SPACE << varName << std::endl;
	}
	void append_DEF_VAR_W(const std::string & varName) {
        _context << TAB << "DEF W" << SPACE << varName << std::endl;
	}
	void append_DEF_VAR_B(const std::string & varName) {
        _context << TAB << "DEF B" << SPACE << varName << std::endl;
	}
	void append_DEF_VAR_FLT(const std::string & varName) {
        _context << TAB << "DEF FLT" << SPACE << varName << std::endl;
	}
	void append_DEF_VAR_DBL(const std::string & varName) {
        _context << TAB << "DEF DBL" << SPACE << varName << std::endl;
	}

    /* 堆 */
    void append_HPUSH_QW(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH QW" << std::endl;
    }
    void append_HPUSH_DW(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH DW" << std::endl;
    }
    void append_HPUSH_W(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH W" << std::endl;
    }
    void append_HPUSH_B(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH B" << std::endl;
    }
    void append_HPUSH_FLT(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH FLT" << std::endl;
    }
    void append_HPUSH_DBL(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH DBL" << std::endl;
    }
    void append_HPUSH_STR(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPUSH STR" << std::endl;
    }
    void append_VHPUSH(/* int32_t off, int32_t sz, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "VHPUSH" << std::endl;
    }

    void append_HPOP_QW(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP QW" << std::endl;
    }
    void append_HPOP_DW(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP DW" << std::endl;
    }
    void append_HPOP_W(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP W" << std::endl;
    }
    void append_HPOP_B(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP B" << std::endl;
    }
    void append_HPOP_FLT(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP FLT" << std::endl;
    }
    void append_HPOP_DBL(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP DBL" << std::endl;
    }
    void append_HPOP_STR(/* int32_t off, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "HPOP STR" << std::endl;
    }
    void append_VHPOP(/* int32_t off, int32_t sz, int32_t heapaddr, NSTRING type */) {
        _context << TAB << "VHPOP" << std::endl;
    }

    void append_ADD_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "ADD QW" << std::endl;
    }
    void append_ADD_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "ADD DW" << std::endl;
    }
    void append_ADD_W(/* int16_t a, int16_t b */) {
        _context << TAB << "ADD W" << std::endl;
    }
    void append_ADD_B(/* int8_t a, int8_t b */) {
        _context << TAB << "ADD B" << std::endl;
    }
    void append_ADD_FLT(/* float a, float b */) {
        _context << TAB << "ADD FLT" << std::endl;
    }
    void append_ADD_DBL(/* double a, double b */) {
        _context << TAB << "ADD DBL" << std::endl;
    }

    void append_SUB_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "SUB QW" << std::endl;
    }
    void append_SUB_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "SUB DW" << std::endl;
    }
    void append_SUB_W(/* int16_t a, int16_t b */) {
        _context << TAB << "SUB W" << std::endl;
    }
    void append_SUB_B(/* int8_t a, int8_t b */) {
        _context << TAB << "SUB B" << std::endl;
    }
    void append_SUB_FLT(/* float a, float b */) {
        _context << TAB << "SUB FLT" << std::endl;
    }
    void append_SUB_DBL(/* double a, double b */) {
        _context << TAB << "SUB DBL" << std::endl;
    }

    void append_MUL_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "MUL QW" << std::endl;
    }
    void append_MUL_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "MUL DW" << std::endl;
    }
    void append_MUL_W(/* int16_t a, int16_t b */) {
        _context << TAB << "MUL W" << std::endl;
    }
    void append_MUL_B(/* int8_t a, int8_t b */) {
        _context << TAB << "MUL B" << std::endl;
    }
    void append_MUL_FLT(/* float a, float b */) {
        _context << TAB << "MUL FLT" << std::endl;
    }
    void append_MUL_DBL(/* double a, double b */) {
        _context << TAB << "MUL DBL" << std::endl;
    }

    void append_DIV_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "DIV QW" << std::endl;
    }
    void append_DIV_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "DIV DW" << std::endl;
    }
    void append_DIV_W(/* int16_t a, int16_t b */) {
        _context << TAB << "DIV W" << std::endl;
    }
    void append_DIV_B(/* int8_t a, int8_t b */) {
        _context << TAB << "DIV B" << std::endl;
    }
    void append_DIV_FLT(/* float a, float b */) {
        _context << TAB << "DIV FLT" << std::endl;
    }
    void append_DIV_DBL(/* double a, double b */) {
        _context << TAB << "DIV DBL" << std::endl;
    }

    void append_MOD_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "MOD QW" << std::endl;
    }
    void append_MOD_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "MOD DW" << std::endl;
    }
    void append_MOD_W(/* int16_t a, int16_t b */) {
        _context << TAB << "MOD W" << std::endl;
    }
    void append_MOD_B(/* int8_t a, int8_t b */) {
        _context << TAB << "MOD B" << std::endl;
    }
    void append_MOD_FLT(/* float a, float b */) {
        _context << TAB << "MOD FLT" << std::endl;
    }
    void append_MOD_DBL(/* double a, double b */) {
        _context << TAB << "MOD DBL" << std::endl;
    }

    void append_LT_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "LT QW" << std::endl;
    }
    void append_LT_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "LT DW" << std::endl;
    }
    void append_LT_W(/* int16_t a, int16_t b */) {
        _context << TAB << "LT W" << std::endl;
    }
    void append_LT_B(/* int8_t a, int8_t b */) {
        _context << TAB << "LT B" << std::endl;
    }
    void append_LT_FLT(/* float a, float b */) {
        _context << TAB << "LT FLT" << std::endl;
    }
    void append_LT_DBL(/* double a, double b */) {
        _context << TAB << "LT DBL" << std::endl;
    }

    void append_LE_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "LE QW" << std::endl;
    }
    void append_LE_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "LE DW" << std::endl;
    }
    void append_LE_W(/* int16_t a, int16_t b */) {
        _context << TAB << "LE W" << std::endl;
    }
    void append_LE_B(/* int8_t a, int8_t b */) {
        _context << TAB << "LE B" << std::endl;
    }
    void append_LE_FLT(/* float a, float b */) {
        _context << TAB << "LE FLT" << std::endl;
    }
    void append_LE_DBL(/* double a, double b */) {
        _context << TAB << "LE DBL" << std::endl;
    }

    void append_EQ_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "EQ QW" << std::endl;
    }
    void append_EQ_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "EQ DW" << std::endl;
    }
    void append_EQ_W(/* int16_t a, int16_t b */) {
        _context << TAB << "EQ W" << std::endl;
    }
    void append_EQ_B(/* int8_t a, int8_t b */) {
        _context << TAB << "EQ B" << std::endl;
    }
    void append_EQ_FLT(/* float a, float b */) {
        _context << TAB << "EQ FLT" << std::endl;
    }
    void append_EQ_DBL(/* double a, double b */) {
        _context << TAB << "EQ DBL" << std::endl;
    }

    void append_NE_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "NE QW" << std::endl;
    }
    void append_NE_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "NE DW" << std::endl;
    }
    void append_NE_W(/* int16_t a, int16_t b */) {
        _context << TAB << "NE W" << std::endl;
    }
    void append_NE_B(/* int8_t a, int8_t b */) {
        _context << TAB << "NE B" << std::endl;
    }
    void append_NE_FLT(/* float a, float b */) {
        _context << TAB << "NE FLT" << std::endl;
    }
    void append_NE_DBL(/* double a, double b */) {
        _context << TAB << "NE DBL" << std::endl;
    }

    void append_GE_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "GE QW" << std::endl;
    }
    void append_GE_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "GE DW" << std::endl;
    }
    void append_GE_W(/* int16_t a, int16_t b */) {
        _context << TAB << "GE W" << std::endl;
    }
    void append_GE_B(/* int8_t a, int8_t b */) {
        _context << TAB << "GE B" << std::endl;
    }
    void append_GE_FLT(/* float a, float b */) {
        _context << TAB << "GE FLT" << std::endl;
    }
    void append_GE_DBL(/* double a, double b */) {
        _context << TAB << "GE DBL" << std::endl;
    }

    void append_GT_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "GT QW" << std::endl;
    }
    void append_GT_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "GT DW" << std::endl;
    }
    void append_GT_W(/* int16_t a, int16_t b */) {
        _context << TAB << "GT W" << std::endl;
    }
    void append_GT_B(/* int8_t a, int8_t b */) {
        _context << TAB << "GT B" << std::endl;
    }
    void append_GT_FLT(/* float a, float b */) {
        _context << TAB << "GT FLT" << std::endl;
    }
    void append_GT_DBL(/* double a, double b */) {
        _context << TAB << "GT DBL" << std::endl;
    }

    void append_LAND(/* bool a, bool b */) {
        _context << TAB << "LAND" << std::endl;
    }
    void append_LOR(/* bool a, bool b */) {
        _context << TAB << "LOR" << std::endl;
    }
    void append_LNOT(/* bool a */) {
        _context << TAB << "LNOT" << std::endl;
    }

    void append_BAND_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "BAND QW" << std::endl;
    }
    void append_BAND_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "BAND DW" << std::endl;
    }
    void append_BAND_W(/* int16_t a, int16_t b */) {
        _context << TAB << "BAND W" << std::endl;
    }
    void append_BAND_B(/* int8_t a, int8_t b */) {
        _context << TAB << "BAND B" << std::endl;
    }

    void append_BOR_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "BOR QW" << std::endl;
    }
    void append_BOR_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "BOR DW" << std::endl;
    }
    void append_BOR_W(/* int16_t a, int16_t b */) {
        _context << TAB << "BOR W" << std::endl;
    }
    void append_BOR_B(/* int8_t a, int8_t b */) {
        _context << TAB << "BOR B" << std::endl;
    }

    void append_BXOR_QW(/* int64_t a, int64_t b */) {
        _context << TAB << "BXOR QW" << std::endl;
    }
    void append_BXOR_DW(/* int32_t a, int32_t b */) {
        _context << TAB << "BXOR DW" << std::endl;
    }
    void append_BXOR_W(/* int16_t a, int16_t b */) {
        _context << TAB << "BXOR W" << std::endl;
    }
    void append_BXOR_B(/* int8_t a, int8_t b */) {
        _context << TAB << "BXOR B" << std::endl;
    }

    void append_BNOT_QW(/* int64_t a */) {
        _context << TAB << "BNOT QW" << std::endl;
    }
    void append_BNOT_DW(/* int32_t a */) {
        _context << TAB << "BNOT DW" << std::endl;
    }
    void append_BNOT_W(/* int16_t a */) {
        _context << TAB << "BNOT W" << std::endl;
    }
    void append_BNOT_B(/* int8_t a */) {
        _context << TAB << "BNOT B" << std::endl;
    }

    void append_SHL_QW(/* int64_t a, int8_t bits */) {
        _context << TAB << "SHL QW" << std::endl;
    }
    void append_SHL_DW(/* int32_t a, int8_t bits */) {
        _context << TAB << "SHL DW" << std::endl;
    }
    void append_SHL_W(/* int16_t a, int8_t bits */) {
        _context << TAB << "SHL W" << std::endl;
    }
    void append_SHL_B(/* int8_t a, int8_t bits */) {
        _context << TAB << "SHL B" << std::endl;
    }

    void append_SHR_QW(/* int64_t a, int8_t bits */) {
        _context << TAB << "SHR QW" << std::endl;
    }
    void append_SHR_DW(/* int32_t a, int8_t bits */) {
        _context << TAB << "SHR DW" << std::endl;
    }
    void append_SHR_W(/* int16_t a, int8_t bits */) {
        _context << TAB << "SHR W" << std::endl;
    }
    void append_SHR_B(/* int8_t a, int8_t bits */) {
        _context << TAB << "SHR B" << std::endl;
    }

    void append_SHRZ_QW(/* int64_t a, int8_t bits */) {
        _context << TAB << "SHRZ QW" << std::endl;
    }
    void append_SHRZ_DW(/* int32_t a, int8_t bits */) {
        _context << TAB << "SHRZ DW" << std::endl;
    }
    void append_SHRZ_W(/* int16_t a, int8_t bits */) {
        _context << TAB << "SHRZ W" << std::endl;
    }
    void append_SHRZ_B(/* int8_t a, int8_t bits */) {
        _context << TAB << "SHRZ B" << std::endl;
    }

    void append_NEG_QW(/* int64_t a */) {
        _context << TAB << "NEG QW" << std::endl;
    }
    void append_NEG_DW(/* int32_t a */) {
        _context << TAB << "NEG DW" << std::endl;
    }
    void append_NEG_W(/* int16_t a */) {
        _context << TAB << "NEG W" << std::endl;
    }
    void append_NEG_B(/* int8_t a */) {
        _context << TAB << "NEG B" << std::endl;
    }
    void append_NEG_FLT(/* float a */) {
        _context << TAB << "NEG FLT" << std::endl;
    }
    void append_NEG_DBL(/* double a */) {
        _context << TAB << "NEG DBL" << std::endl;
    }

    void append_RSZ(const char * from, const char * to /* a */) {
        _context << TAB << "RSZ" << SPACE << from << SPACE << to << std::endl;
    }

    void append_HALT() {
        _context << TAB << "HALT" << std::endl;
    }
    void append_BREAK() {
        _context << TAB << "BREAK" << std::endl;
    }
    // localCodeAddr 汇编前是标号 label
    void append_J(/* int32_t localCodeAddr */ const std::string & label) {
        _context << TAB << "J" << SPACE << LABEL_PREFIX << label << std::endl;
    }
    // localCodeAddr 汇编前是标号 label
    void append_JT(/* int32_t localCodeAddr, bool condVal */ const std::string & label) {
        _context << TAB << "JT" << SPACE << LABEL_PREFIX << label << std::endl;
    }
	// localCodeAddr 汇编前是标号 label
	void append_JF(/* int32_t localCodeAddr, bool condVal */ const std::string & label) {
		_context << TAB << "JF" << SPACE << LABEL_PREFIX << label << std::endl;
	}

    // localFuncCodeAddr 汇编前是函数全名 fn
    void append_CALL(/* int32_t localFuncCodeAddr, int32_t thisRef, ..., int32_t funcRetCodeAddr */ const std::string & fn) {
        _context << TAB << "CALL" << SPACE << fn << std::endl;
    }
    void append_NRET() {
        _context << TAB << "NRET" << std::endl;
    }
	void append_RET_QW() {
		_context << TAB << "RET QW" << SPACE << std::endl;
	}
	void append_RET_DW() {
		_context << TAB << "RET DW" << SPACE << std::endl;
	}
	void append_RET_W() {
		_context << TAB << "RET W" << SPACE << std::endl;
	}
	void append_RET_B() {
		_context << TAB << "RET B" << SPACE << std::endl;
	}
	void append_RET_FLT() {
		_context << TAB << "RET FLT" << SPACE << std::endl;
	}
	void append_RET_DBL() {
		_context << TAB << "RET DBL" << SPACE << std::endl;
	}
    void append_NOP() {
        _context << TAB << "NOP" << std::endl;
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
