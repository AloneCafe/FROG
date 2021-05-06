#ifndef __IL_GENERATOR_HEADER__
#define __IL_GENERATOR_HEADER__

#include <sstream>
#include "UTFSequence.hpp"

class ILGenerator {
protected:
    static constexpr const char * TAB = "\t";
    static constexpr const char * SPACE = " ";
    static constexpr const char * SPLIT = ",";
    static constexpr const char * LABEL_PREFIX = "#";
    static constexpr const char * LABEL_SUFFIX = ":";
    static constexpr const char * STMT_SUFFIX = ";";

protected:
    std::stringstream _context;

public: /* 指令参数皆是从右往左压栈 */
    ILGenerator() = default;
    ILGenerator(const ILGenerator &) = default;
    ILGenerator & operator=(const ILGenerator &) = default;
    
    std::stringstream & getContextRef() { return _context; }

public:
    
    void append_OFFSET();
    
    void begin_STATIC();
    
    void end_STATIC();
    
    void begin_FUNC(const std::string & fname);
    
    void end_FUNC();
    
    void append_LABEL(const std::string & label);
    
    void append_MKVEC_N_QW(int degree);
    
    void append_MKVEC_N_DW(int degree);
    
    void append_MKVEC_N_W(int degree);
    
    void append_MKVEC_N_B(int degree);
    
    void append_MKVEC_N_FLT(int degree);
    
    void append_MKVEC_N_DBL(int degree);
    
    void append_IPUSH_QW(int64_t qw);
    
    void append_IPUSH_DW(int32_t dw);
    
    void append_IPUSH_W(int16_t w);
    
    void append_IPUSH_B(int8_t b);
    
    void append_IPUSH_FLT(float flt);
    
    void append_IPUSH_DBL(double dbl);
    
    void append_DUP_QW();
    
    void append_DUP_DW();
    
    void append_DUP_W();
    
    void append_DUP_B();
    
    void append_DUP_FLT();
    
    void append_DUP_DBL();
    
    void append_PUSH_VAR_QW(const std::string & varName);
    
    void append_PUSH_VAR_DW(const std::string & varName);
    
    void append_PUSH_VAR_W(const std::string & varName);
    
    void append_PUSH_VAR_B(const std::string & varName);
    
    void append_PUSH_VAR_FLT(const std::string & varName);
    
    void append_PUSH_VAR_DBL(const std::string & varName);
    
    void append_POP_VAR_QW(const std::string & varName);
    
    void append_POP_VAR_DW(const std::string & varName);
    
    void append_POP_VAR_W(const std::string & varName);
    
    void append_POP_VAR_B(const std::string & varName);
    
    void append_POP_VAR_FLT(const std::string & varName);
    
    void append_POP_VAR_DBL(const std::string & varName);
    
    void append_TOP_VAR_QW(const std::string & varName);
    
    void append_TOP_VAR_DW(const std::string & varName);
    
    void append_TOP_VAR_W(const std::string & varName);
    
    void append_TOP_VAR_B(const std::string & varName);
    
    void append_TOP_VAR_FLT(const std::string & varName);
    
    void append_TOP_VAR_DBL(const std::string & varName);
    
    void append_DEF_VAR_QW(const std::string & varName);
    
    void append_DEF_VAR_DW(const std::string & varName);
    
    void append_DEF_VAR_W(const std::string & varName);
    
    void append_DEF_VAR_B(const std::string & varName);
    
    void append_DEF_VAR_FLT(const std::string & varName);
    
    void append_DEF_VAR_DBL(const std::string & varName);
    
    void append_HPUSH_QW();
    
    void append_HPUSH_DW();
    
    void append_HPUSH_W();
    
    void append_HPUSH_B();
    
    void append_HPUSH_FLT();
    
    void append_HPUSH_DBL();
    
    void append_HPOP_QW();
    
    void append_HPOP_DW();
    
    void append_HPOP_W();
    
    void append_HPOP_B();
    
    void append_HPOP_FLT();
    
    void append_HPOP_DBL();
    
    void append_ADD_QW();
    
    void append_ADD_DW();
    
    void append_ADD_W();
    
    void append_ADD_B();
    
    void append_ADD_FLT();
    
    void append_ADD_DBL();
    
    void append_SUB_QW();
    
    void append_SUB_DW();
    
    void append_SUB_W();
    
    void append_SUB_B();
    
    void append_SUB_FLT();
    
    void append_SUB_DBL();
    
    void append_MUL_QW();
    
    void append_MUL_DW();
    
    void append_MUL_W();
    
    void append_MUL_B();
    
    void append_MUL_FLT();
    
    void append_MUL_DBL();
    
    void append_DIV_QW();
    
    void append_DIV_DW();
    
    void append_DIV_W();
    
    void append_DIV_B();
    
    void append_DIV_FLT();
    
    void append_DIV_DBL();
    
    void append_MOD_QW();
    
    void append_MOD_DW();
    
    void append_MOD_W();
    
    void append_MOD_B();
    
    void append_MOD_FLT();
    
    void append_MOD_DBL();
    
    void append_LT_QW();
    
    void append_LT_DW();
    
    void append_LT_W();
    
    void append_LT_B();
    
    void append_LT_FLT();
    
    void append_LT_DBL();
    
    void append_LE_QW();
    
    void append_LE_DW();
    
    void append_LE_W();
    
    void append_LE_B();
    
    void append_LE_FLT();
    
    void append_LE_DBL();
    
    void append_EQ_QW();
    
    void append_EQ_DW();
    
    void append_EQ_W();
    
    void append_EQ_B();
    
    void append_EQ_FLT();
    
    void append_EQ_DBL();
    
    void append_NE_QW();
    
    void append_NE_DW();
    
    void append_NE_W();
    
    void append_NE_B();
    
    void append_NE_FLT();
    
    void append_NE_DBL();
    
    void append_GE_QW();
    
    void append_GE_DW();
    
    void append_GE_W();
    
    void append_GE_B();
    
    void append_GE_FLT();
    
    void append_GE_DBL();
    
    void append_GT_QW();
    
    void append_GT_DW();
    
    void append_GT_W();
    
    void append_GT_B();
    
    void append_GT_FLT();
    
    void append_GT_DBL();
    
    void append_LAND();
    
    void append_LOR();
    
    void append_LNOT();
    
    void append_BAND_QW();
    
    void append_BAND_DW();
    
    void append_BAND_W();
    
    void append_BAND_B();
    
    void append_BOR_QW();
    
    void append_BOR_DW();
    
    void append_BOR_W();
    
    void append_BOR_B();
    
    void append_BXOR_QW();
    
    void append_BXOR_DW();
    
    void append_BXOR_W();
    
    void append_BXOR_B();
    
    void append_BNOT_QW();
    
    void append_BNOT_DW();
    
    void append_BNOT_W();
    
    void append_BNOT_B();
    
    void append_SHL_QW();
    
    void append_SHL_DW();
    
    void append_SHL_W();
    
    void append_SHL_B();
    
    void append_SHR_QW();
    
    void append_SHR_DW();
    
    void append_SHR_W();
    
    void append_SHR_B();
    
    void append_SHRZ_QW();
    
    void append_SHRZ_DW();
    
    void append_SHRZ_W();
    
    void append_SHRZ_B();
    
    void append_NEG_QW();
    
    void append_NEG_DW();
    
    void append_NEG_W();
    
    void append_NEG_B();
    
    void append_NEG_FLT();
    
    void append_NEG_DBL();
    
    void append_RSZ(const char * from, const char * to );
    
    void append_HALT();
    
    void append_LEN();
    
    void append_J(const std::string & label);
    
    void append_JT(const std::string & label);
    
    void append_JF(const std::string & label);
    
    void append_CALL(const std::string & fn);
    
    void append_NRET();
    
    void append_RET_QW();
    
    void append_RET_DW();
    
    void append_RET_W();
    
    void append_RET_B();
    
    void append_RET_FLT();
    
    void append_RET_DBL();
    
    void append_NOP();
};

#endif
