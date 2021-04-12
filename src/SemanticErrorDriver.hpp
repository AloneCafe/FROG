#ifndef __SEM_ERROR_DRV_HEADER__
#define __SEM_ERROR_DRV_HEADER__

#include "ErrorDriver.hpp"

template <typename T = ErrorDriver>
class SemErrorDriver {
private:
    std::shared_ptr<typename std::enable_if<
            std::is_base_of<ErrorDriverBase, T>::value, T>::type>
            _pErrDrv;
    std::string _filename;

public:
    explicit SemErrorDriver(const std::string & filename)
            : _pErrDrv(std::make_shared<T>()), _filename(filename) { }
    
    virtual ~SemErrorDriver() = default;
    void postErr(const ErrorType & type, const LocatedUtfString & name) {
        _pErrDrv->postErr(type, _filename, name.lineno(), name.colno());
    }
    void postErr(const ErrorType & type,
            const ExprPtr & pExpr) {
        _pErrDrv->postErr(type, _filename, pExpr->lineno(), pExpr->colno());
    }
    void fatalErr(const ErrorType & type,
            const LocatedUtfString & name) {
        _pErrDrv->fatalErr(type, _filename, name.lineno(), name.colno());
    }
    void fatalErr(const ErrorType & type,
            const ExprPtr & pExpr) {
        _pErrDrv->fatalErr(type, _filename, pExpr->lineno(), pExpr->colno());
    }
    auto lastErr() const { return _pErrDrv->lastErr(); }
    bool hasErr() const { return _pErrDrv->hasErr(); }
    void clearLastErr() { _pErrDrv->clearLastErr(); }
    void clearLastErr(size_t n) { _pErrDrv->clearLastErr(n); }
    auto getErrCount() const { return _pErrDrv->getErrList().size(); }
    auto getErrList() const -> const std::vector<Error> & {
        return _pErrDrv->getErrList();
    }
    std::string getFileName() const { return _filename; }
    void setFileName(const std::string & newFileName) { _filename = newFileName; }
};

#endif