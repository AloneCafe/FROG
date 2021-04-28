#ifndef __EWDRIVER_HEADER__
#define __EWDRIVER_HEADER__

#include <iostream>
#include <list>
#include <memory>
#include <queue>
#include <string>
#include <sstream>
#include <unordered_set>
#include <type_traits>
#include <tuple>

#include "LittleAssist.hpp"
#include "TokenDefinition.hpp"

enum ErrorType {
    E_NOERR,
    E_UNKNOWN,
    // E_INVALID_DIGIT_SEQ,
    E_INVALID_OCTDIGIT_SEQ,
    E_INCMP_HEXDIGIT_SEQ,
    E_INCMP_STRING_SEQ,
    E_INCMP_CHAR_SEQ,
    E_INVALID_EXP,
    E_INVALID_CHAR,
    E_ID_TOO_LONG,
    E_CHARSEQ_TOO_LONG,
    E_STRING_TOO_LONG,
    
    E_ILLEGAL_TOKEN,
    E_UNEXPECTED_EOF,
    E_INCMP_ID_LIST,
    E_INCMP_IMPLEMENTS_LIST,
    E_INCMP_EXTENDS_LIST,
    E_ILLEGAL_PKGNAME,
    E_ILLEGAL_IMPORT_CLASSNAME,
    E_LOSE_SEMICOLON,
    E_MULTIPLE_DEFINITION_PKGNAME,
    E_ILLEGAL_CLASS_MODIFIER,
    E_ILLEGAL_INTERFACE_MODIFIER,
    E_INTERFACE_EXPECT_EXTENDS,
    E_ILLEGAL_PACKAGE_POS,
    E_ILLEGAL_IMPORT_POS,
    E_LOSE_RP,
    E_LOSE_COLON,
    E_LOSE_RBRACKET,
    E_LOSE_RBRACE,
    E_EXPECT_LP,
    E_ILLEGAL_FORMAL_ARG_TYPE,
    E_ILLEGAL_FORMAL_ARG_NAME,
    E_INCMP_NEW_EXPRESSION,
    E_DUPLICATED_FINAL_MODIFIER,
    E_DUPLICATED_STATIC_MODIFIER,
    E_DUPLICATED_ABSTRACT_MODIFIER,
    E_DUPLICATED_ACCESS_MODIFIER,
    E_ILLEGAL_MODIFIER,
    E_INCMP_TYPE_CONVERT,
    E_INCMP_TYPE,
    E_BOTH_FINAL_AND_ABSTRACT,
    E_DEFINE_STATIC_OBJ,
    E_ILLEGAL_EXPR,
    E_ASSIGN_LEFT_VAL,
    
    E_PKG_NOT_EXIST,
    E_IMPORT_TYPE_NOT_EXIST,
    E_TYPE_REDEFINED,
    E_UNRECOGNIZED_TYPE_NAME,
    E_AMBIGUOUS_TYPE_NAME,
    E_INHERIT_I2I,
    E_INHERIT_C2I,
    E_INHERIT_C2C,
    E_FUNC_REDEFINED,
    E_FIELD_REDEFINED,
    E_CYCLIC_INHERITANCE,
    E_INHERIT_FINAL_SUPER_CLASS,
    E_OVERRIDE_FINAL_FUNCTION,
    E_CLASS_HAS_UNIMPLEMENTED_ABSTRACT_FUNCTIONS,
    E_INCMP_OVERRIDE_FUNC_RET_TYPE,
    E_REDUCE_OVERRIDE_FUNC_ACCESSIBLITY,
    E_ILLEGAL_TYPE_FOR_OBJ_DEFINITION,
    E_VOID_FOR_OBJ_DEFINITION,
    E_OBJECT_REDEFINED,
    E_INCMP_TYPE_BOOLEAN,
    E_INCMP_TYPE_BYTE,
    E_INCMP_TYPE_GENERIC_INT,
    E_INCMP_TYPE_GENERIC_NUMBER,
    E_INCMP_TYPE_DW,
    E_INCMP_TYPE_QW,
    E_INCMP_TYPE_FLT,
    E_INCMP_TYPE_DBL,
    E_INCMP_TYPE_CHAR,
    E_INCMP_TYPE_SHORT,
    E_INCMP_TYPE_LITERAL_STRING,
    E_INCMP_TYPE_REF,
    E_ID_UNDEFINED,
    E_ILLEGAL_THIS,
    E_ILLEGAL_SUPER,
    E_ILLEGAL_MEMBER,
    E_ILLEGAL_TYPE_CONVERT_TYPE,
    E_ILLEGAL_FUNCTION_TO_CALL,
    E_FUNC_UNDEFINED,
    E_FUNC_ARG_NUM_MISMATCH,
    E_UNSUPPORTED_LIST_INIT,
    E_VAR_REDEFINED,
    E_FUNC_HAS_NO_RET_VALUE,
    E_FUNC_NEED_RET_VALUE,
    E_IILEGAL_BREAK,
    E_IILEGAL_CONTINUE,
    E_ILLEGAL_RET_TYPE,
    E_MISMATCHING_FUNC,
    E_ILLEGAL_INIT_STMT,
};

class Error {
    friend struct ErrorHash;
private:
    std::string _msg;
    ErrorType _type;
    int _lineno;
    int _colno;

public:
    explicit Error(const ErrorType & type, const std::string & filename,
            long lineno, long colno);
    const std::string & getMsg() const { return this->_msg; }
    long getErrCode() const { return static_cast<long>(this->_type); }
    int lineno() const { return _lineno; }
    int colno() const { return _colno; }
    const ErrorType & getErrType() const { return this->_type; }
    bool operator==(const Error & err) {
        return this->getErrType() == err.getErrType();
    }
    bool operator==(const ErrorType & type) { return this->getErrType() == type; }
    bool operator!=(const Error & err) {
        return this->getErrType() != err.getErrType();
    }
    bool operator!=(const ErrorType & type) { return this->getErrType() != type; }
};

struct ErrorHash {
    size_t operator()(const std::tuple<ErrorType, int, int> & err) const {
        std::stringstream ss;
        ss << std::get<0>(err) 
            << "@" << std::get<1>(err) 
            << ":" << std::get<2>(err);
        return std::hash<std::string>{}(ss.str());
    }
};

class ErrorDriverBase { };

class ErrorDriver : public ErrorDriverBase {
public:
    using FuncBeforeExit = void (*)(std::vector<Error> & _errors);

private:
    std::unordered_set<std::tuple<ErrorType, int, int>, ErrorHash> _set;
    std::vector<Error> _errors;
    FuncBeforeExit _pfuncBeforeExit = nullptr;

public:
    explicit ErrorDriver() = default;
    explicit ErrorDriver(void (* pfuncBeforeExit)(std::vector<Error> & _errors))
            : _pfuncBeforeExit(pfuncBeforeExit) { }
    virtual ~ErrorDriver() = default;
    void postErr(const ErrorType & type, const std::string & filename, long lineno,
            long colno);
    void postErr(const Error & err);
    void fatalErr(const ErrorType & type, const std::string & filename, long lineno,
            long colno);
    void fatalErr(const Error & err);
    auto lastErr() -> decltype(_errors.back()) { return _errors.back(); }
    auto getErrList() const -> const std::vector<Error> & { return _errors; }
    void clearLastErr() { 
        clearLastErr(1);
    }
    void clearLastErr(size_t n) {
        for (auto it = _errors.cend() - n; it < _errors.cend(); ++it) {
            _set.erase(std::make_tuple(it->getErrType(), it->lineno(), it->colno()));
        }
        _errors.erase(_errors.cend() - n, _errors.cend());
    }
    bool hasErr() const { return ! _errors.empty(); }
    
    FuncBeforeExit assocFuncBeforeExit(const FuncBeforeExit & newFunc) {
        FuncBeforeExit oldFunc = _pfuncBeforeExit;
        return _pfuncBeforeExit = newFunc, oldFunc;
    }
};

enum WarningType { W_UNKNOWN, W_UNKNOWN_ESCAPE_SEQUENCE };
/*
    class Warning {
    private:
        const String _msg;
        const WarningType &_type;

    public:
        explicit Warning(const WarningType &type) : _type(type) {
            switch (_type) {
                case W_UNKNOWN_ESCAPE_SEQUENCE:
                    _msg = "未知的转义序列";
                    break;

                case W_UNKNOWN:
                default:
                    _msg = "发生未知警告";
                    break;
            }
        }

        const char *getMsg() const { return this->_msg; }

        long getWarnCode() const { return static_cast<long>(this->_type); }

        const WarningType &getWarnType() const { return this->_type; }

        bool operator==(const Warning &err) { return this->getWarnType() ==
   err.getWarnType(); }

        bool operator==(const WarningType &type) { return this->getWarnType() ==
   type; }

        bool operator!=(const Warning &err) { return this->getWarnType() !=
   err.getWarnType(); }

        bool operator!=(const WarningType &type) { return this->getWarnType() !=
   type; }
    };
    */

#endif