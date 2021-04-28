#ifndef __AST_ERROR_DRV_HEADER__
#define __AST_ERROR_DRV_HEADER__

#include "ErrorDriver.hpp"
#include "Token.hpp"

template <typename T = ErrorDriver>
class AstErrorDriver {
private:
    std::shared_ptr<typename std::enable_if<
            std::is_base_of<ErrorDriverBase, T>::value, T>::type>
            _pErrDrv;
    
    std::string _filename;

public:
    explicit AstErrorDriver(const std::string & filename) :
        _pErrDrv(std::make_shared<T>()),
        _filename(filename) { }
    
    virtual ~AstErrorDriver() = default;
    
    void postErr(const ErrorType & type, const Token & tk) {
        _pErrDrv->postErr(type, _filename, tk.lineno(), tk.colno());
    }
    
    void fatalErr(const ErrorType & type, const Token & tk) {
        _pErrDrv->fatalErr(type, _filename, tk.lineno(), tk.colno());
    }
    
    const Error & lastErr() const {
        return _pErrDrv->lastErr();
    }
    
    bool hasErr() const {
        return _pErrDrv->hasErr();
    }
    
    void clearLastErr() {
        _pErrDrv->clearLastErr();
    }
    
    void clearLastErr(size_t n) {
        _pErrDrv->clearLastErr(n);
    }
    
    size_t getErrCount() const {
        return _pErrDrv->getErrList().size();
    }
    
    const std::vector<Error> & getErrList() const {
        return _pErrDrv->getErrList();
    }
    
    std::string getFileName() const {
        return _filename;
    }
    
    void setFileName(const std::string & newFileName) {
        _filename = newFileName;
    }
};

#endif
