#include "ILGenerator.hpp"

void ILGenerator::append_OFFSET() {
    _context << TAB << "OFFSET" << STMT_SUFFIX << std::endl;
}

void ILGenerator::begin_STATIC() {
    _context << ".STATIC" << STMT_SUFFIX << std::endl;
}

void ILGenerator::end_STATIC() {
    _context << ".END" << STMT_SUFFIX << std::endl << std::endl;
}

void ILGenerator::begin_FUNC(const std::string & fname) {
    _context << ".FUNC" << SPACE << fname << STMT_SUFFIX << std::endl;
}

void ILGenerator::end_FUNC() {
    _context << ".END" << STMT_SUFFIX << std::endl << std::endl;
}

void ILGenerator::append_LABEL(const std::string & label) {
    _context << LABEL_PREFIX << label << LABEL_SUFFIX << std::endl;
}

void ILGenerator::append_MKVEC_N_QW(int degree) {
    _context << TAB << "MKVEC" << SPACE << degree << SPACE << "QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MKVEC_N_DW(int degree) {
    _context << TAB << "MKVEC" << SPACE << degree << SPACE << "DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MKVEC_N_W(int degree) {
    _context << TAB << "MKVEC" << SPACE << degree << SPACE << "W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MKVEC_N_B(int degree) {
    _context << TAB << "MKVEC" << SPACE << degree << SPACE << "B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MKVEC_N_FLT(int degree) {
    _context << TAB << "MKVEC" << SPACE << degree << SPACE << "FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MKVEC_N_DBL(int degree) {
    _context << TAB << "MKVEC" << SPACE << degree << SPACE << "DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_IPUSH_QW(int64_t qw) {
    _context << TAB << "IPUSH QW" << SPACE << qw << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_IPUSH_DW(int32_t dw) {
    _context << TAB << "IPUSH DW" << SPACE << static_cast<int64_t>(dw) << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_IPUSH_W(int16_t w) {
    _context << TAB << "IPUSH W" << SPACE << static_cast<int64_t>(w) << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_IPUSH_B(int8_t b) {
    _context << TAB << "IPUSH B" << SPACE << static_cast<int64_t>(b) << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_IPUSH_FLT(float flt) {
    _context << TAB << "IPUSH FLT" << SPACE << flt << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_IPUSH_DBL(double dbl) {
    _context << TAB << "IPUSH DBL" << SPACE << dbl << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_PUSH_VAR_QW(const std::string & varName) {
    _context << TAB << "PUSH QW" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_PUSH_VAR_DW(const std::string & varName) {
    _context << TAB << "PUSH DW" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_PUSH_VAR_W(const std::string & varName) {
    _context << TAB << "PUSH W" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_PUSH_VAR_B(const std::string & varName) {
    _context << TAB << "PUSH B" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_PUSH_VAR_FLT(const std::string & varName) {
    _context << TAB << "PUSH FLT" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_PUSH_VAR_DBL(const std::string & varName) {
    _context << TAB << "PUSH DBL" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_POP_VAR_QW(const std::string & varName) {
    _context << TAB << "POP QW" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_POP_VAR_DW(const std::string & varName) {
    _context << TAB << "POP DW" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_POP_VAR_W(const std::string & varName) {
    _context << TAB << "POP W" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_POP_VAR_B(const std::string & varName) {
    _context << TAB << "POP B" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_POP_VAR_FLT(const std::string & varName) {
    _context << TAB << "POP FLT" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_POP_VAR_DBL(const std::string & varName) {
    _context << TAB << "POP DBL" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_TOP_VAR_QW(const std::string & varName) {
    _context << TAB << "TOP QW" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_TOP_VAR_DW(const std::string & varName) {
    _context << TAB << "TOP DW" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_TOP_VAR_W(const std::string & varName) {
    _context << TAB << "TOP W" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_TOP_VAR_B(const std::string & varName) {
    _context << TAB << "TOP B" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_TOP_VAR_FLT(const std::string & varName) {
    _context << TAB << "TOP FLT" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_TOP_VAR_DBL(const std::string & varName) {
    _context << TAB << "TOP DBL" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_DEF_VAR_QW(const std::string & varName) {
    _context << TAB << "DEF QW" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_DEF_VAR_DW(const std::string & varName) {
    _context << TAB << "DEF DW" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_DEF_VAR_W(const std::string & varName) {
    _context << TAB << "DEF W" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_DEF_VAR_B(const std::string & varName) {
    _context << TAB << "DEF B" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_DEF_VAR_FLT(const std::string & varName) {
    _context << TAB << "DEF FLT" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_DEF_VAR_DBL(const std::string & varName) {
    _context << TAB << "DEF DBL" << SPACE << varName << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_HPUSH_QW() {
    _context << TAB << "HPUSH QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_HPUSH_DW() {
    _context << TAB << "HPUSH DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_HPUSH_W() {
    _context << TAB << "HPUSH W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_HPUSH_B() {
    _context << TAB << "HPUSH B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_HPUSH_FLT() {
    _context << TAB << "HPUSH FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_HPUSH_DBL() {
    _context << TAB << "HPUSH DBL" << STMT_SUFFIX << std::endl;
}


void ILGenerator::append_HPOP_QW() {
    _context << TAB << "HPOP QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_HPOP_DW() {
    _context << TAB << "HPOP DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_HPOP_W() {
    _context << TAB << "HPOP W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_HPOP_B() {
    _context << TAB << "HPOP B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_HPOP_FLT() {
    _context << TAB << "HPOP FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_HPOP_DBL() {
    _context << TAB << "HPOP DBL" << STMT_SUFFIX << std::endl;
}


void ILGenerator::append_ADD_QW() {
    _context << TAB << "ADD QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_ADD_DW() {
    _context << TAB << "ADD DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_ADD_W() {
    _context << TAB << "ADD W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_ADD_B() {
    _context << TAB << "ADD B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_ADD_FLT() {
    _context << TAB << "ADD FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_ADD_DBL() {
    _context << TAB << "ADD DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SUB_QW() {
    _context << TAB << "SUB QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SUB_DW() {
    _context << TAB << "SUB DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SUB_W() {
    _context << TAB << "SUB W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SUB_B() {
    _context << TAB << "SUB B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SUB_FLT() {
    _context << TAB << "SUB FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SUB_DBL() {
    _context << TAB << "SUB DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MUL_QW() {
    _context << TAB << "MUL QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MUL_DW() {
    _context << TAB << "MUL DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MUL_W() {
    _context << TAB << "MUL W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MUL_B() {
    _context << TAB << "MUL B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MUL_FLT() {
    _context << TAB << "MUL FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MUL_DBL() {
    _context << TAB << "MUL DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_DIV_QW() {
    _context << TAB << "DIV QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_DIV_DW() {
    _context << TAB << "DIV DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_DIV_W() {
    _context << TAB << "DIV W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_DIV_B() {
    _context << TAB << "DIV B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_DIV_FLT() {
    _context << TAB << "DIV FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_DIV_DBL() {
    _context << TAB << "DIV DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MOD_QW() {
    _context << TAB << "MOD QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MOD_DW() {
    _context << TAB << "MOD DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MOD_W() {
    _context << TAB << "MOD W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MOD_B() {
    _context << TAB << "MOD B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MOD_FLT() {
    _context << TAB << "MOD FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_MOD_DBL() {
    _context << TAB << "MOD DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LT_QW() {
    _context << TAB << "LT QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LT_DW() {
    _context << TAB << "LT DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LT_W() {
    _context << TAB << "LT W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LT_B() {
    _context << TAB << "LT B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LT_FLT() {
    _context << TAB << "LT FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LT_DBL() {
    _context << TAB << "LT DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LE_QW() {
    _context << TAB << "LE QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LE_DW() {
    _context << TAB << "LE DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LE_W() {
    _context << TAB << "LE W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LE_B() {
    _context << TAB << "LE B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LE_FLT() {
    _context << TAB << "LE FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LE_DBL() {
    _context << TAB << "LE DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_EQ_QW() {
    _context << TAB << "EQ QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_EQ_DW() {
    _context << TAB << "EQ DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_EQ_W() {
    _context << TAB << "EQ W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_EQ_B() {
    _context << TAB << "EQ B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_EQ_FLT() {
    _context << TAB << "EQ FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_EQ_DBL() {
    _context << TAB << "EQ DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NE_QW() {
    _context << TAB << "NE QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NE_DW() {
    _context << TAB << "NE DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NE_W() {
    _context << TAB << "NE W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NE_B() {
    _context << TAB << "NE B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NE_FLT() {
    _context << TAB << "NE FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NE_DBL() {
    _context << TAB << "NE DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_GE_QW() {
    _context << TAB << "GE QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_GE_DW() {
    _context << TAB << "GE DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_GE_W() {
    _context << TAB << "GE W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_GE_B() {
    _context << TAB << "GE B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_GE_FLT() {
    _context << TAB << "GE FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_GE_DBL() {
    _context << TAB << "GE DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_GT_QW() {
    _context << TAB << "GT QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_GT_DW() {
    _context << TAB << "GT DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_GT_W() {
    _context << TAB << "GT W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_GT_B() {
    _context << TAB << "GT B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_GT_FLT() {
    _context << TAB << "GT FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_GT_DBL() {
    _context << TAB << "GT DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LAND() {
    _context << TAB << "LAND" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LOR() {
    _context << TAB << "LOR" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_LNOT() {
    _context << TAB << "LNOT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BAND_QW() {
    _context << TAB << "BAND QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BAND_DW() {
    _context << TAB << "BAND DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BAND_W() {
    _context << TAB << "BAND W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BAND_B() {
    _context << TAB << "BAND B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BOR_QW() {
    _context << TAB << "BOR QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BOR_DW() {
    _context << TAB << "BOR DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BOR_W() {
    _context << TAB << "BOR W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BOR_B() {
    _context << TAB << "BOR B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BXOR_QW() {
    _context << TAB << "BXOR QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BXOR_DW() {
    _context << TAB << "BXOR DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BXOR_W() {
    _context << TAB << "BXOR W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BXOR_B() {
    _context << TAB << "BXOR B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BNOT_QW() {
    _context << TAB << "BNOT QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BNOT_DW() {
    _context << TAB << "BNOT DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BNOT_W() {
    _context << TAB << "BNOT W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_BNOT_B() {
    _context << TAB << "BNOT B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SHL_QW() {
    _context << TAB << "SHL QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SHL_DW() {
    _context << TAB << "SHL DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SHL_W() {
    _context << TAB << "SHL W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SHL_B() {
    _context << TAB << "SHL B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SHR_QW() {
    _context << TAB << "SHR QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SHR_DW() {
    _context << TAB << "SHR DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SHR_W() {
    _context << TAB << "SHR W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SHR_B() {
    _context << TAB << "SHR B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SHRZ_QW() {
    _context << TAB << "SHRZ QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SHRZ_DW() {
    _context << TAB << "SHRZ DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SHRZ_W() {
    _context << TAB << "SHRZ W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_SHRZ_B() {
    _context << TAB << "SHRZ B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NEG_QW() {
    _context << TAB << "NEG QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NEG_DW() {
    _context << TAB << "NEG DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NEG_W() {
    _context << TAB << "NEG W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NEG_B() {
    _context << TAB << "NEG B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NEG_FLT() {
    _context << TAB << "NEG FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NEG_DBL() {
    _context << TAB << "NEG DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_RSZ(const char * from, const char * to) {
    _context << TAB << "RSZ" << SPACE << from << SPACE << to << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_HALT() {
    _context << TAB << "HALT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_J(const std::string & label) {
    _context << TAB << "J" << SPACE << LABEL_PREFIX << label << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_JT(const std::string & label) {
    _context << TAB << "JT" << SPACE << LABEL_PREFIX << label << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_JF(const std::string & label) {
    _context << TAB << "JF" << SPACE << LABEL_PREFIX << label << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_CALL(const std::string & fn) {
    _context << TAB << "CALL" << SPACE << fn << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NRET() {
    _context << TAB << "NRET" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_RET_QW() {
    _context << TAB << "RET QW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_RET_DW() {
    _context << TAB << "RET DW" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_RET_W() {
    _context << TAB << "RET W" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_RET_B() {
    _context << TAB << "RET B" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_RET_FLT() {
    _context << TAB << "RET FLT" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_RET_DBL() {
    _context << TAB << "RET DBL" << STMT_SUFFIX << std::endl;
}

void ILGenerator::append_NOP() {
    _context << TAB << "NOP" << STMT_SUFFIX << std::endl;
}