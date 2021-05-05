#ifndef __PARSER_CONFIG_HEADER__
#define __PARSER_CONFIG_HEADER__

#if defined (_MSC_VER) 
#	define INT3 _asm { int 3 }
#elif defined (__GNUC__)
#	define INT3 asm("int3");
#else
#	define INT3 
#endif

#define DBGPRINT (std::cerr << "[DBGPRINT] code in function: " << __FUNCTION__ << ", lineno: " << __LINE__ << std::endl)

#if !defined(_DEBUG)
#   define AST_E(x)         (void)(x != E_UNEXPECTED_EOF ? _aed.postErr((x), *it) : _aed.fatalErr((x), *it))
#   define AST_FATAL(x)     (void)(_aed.fatalErr((x), *it))
#   define SEM_E(x, l)      (void)(_sed.postErr((x), l))
#   define SEM_FATAL(x, l)  (void)(_sed.fatalErr((x), l))
#else
#   define AST_E(x)         DBGPRINT, (void)(x != E_UNEXPECTED_EOF ? _aed.postErr((x), *it) : _aed.fatalErr((x), *it))
#   define AST_FATAL(x)     DBGPRINT, (void)(_aed.fatalErr((x), *it))
#   define SEM_E(x, l)      DBGPRINT, (void)(x != E_ID_UNDEFINED ? _sed.postErr((x), l) : _sed.fatalErr((x), l))
#   define SEM_FATAL(x, l)  DBGPRINT, (void)(_sed.fatalErr((x), l))
#endif

#define SEM_E_COUNT_RECOND auto _rem_rec_count_ = _sed.getErrCount()
#define SEM_E_COUNT_HAS_INCREASED  bool(_sed.getErrCount() - _rem_rec_count_)
#define SEM_E_COUNT_CLEAR_DELTA _sed.clearLastErr(_sed.getErrCount() - _rem_rec_count_)

#define AST_REC_LOCATION1    long _ast_rec_location_lineno1_ = it->lineno(), _ast_rec_location_colno1_ = it->colno();
#define AST_ARG_LOCATION1    _ast_rec_location_lineno1_,_ast_rec_location_colno1_
#define AST_REC_LOCATION2    long _ast_rec_location_lineno2_ = it->lineno(), _ast_rec_location_colno2_ = it->colno();
#define AST_ARG_LOCATION2    _ast_rec_location_lineno2_,_ast_rec_location_colno2_

#endif
