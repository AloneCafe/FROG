#include "SemanticErrorDriver.hpp"

template<> void SemErrorDriver<ErrorDriver>::
postErr(const ErrorType & type, const LocatedUtfString & name) {
    _pErrDrv->postErr(type, _filename, name.lineno(), name.colno());
}

template<> void SemErrorDriver<ErrorDriver>::
postErr(const ErrorType & type, const ExprPtr & pExpr) {
    _pErrDrv->postErr(type, _filename, pExpr->lineno(), pExpr->colno());
}

template<> void SemErrorDriver<ErrorDriver>::
fatalErr(const ErrorType & type, const LocatedUtfString & name) {
    _pErrDrv->fatalErr(type, _filename, name.lineno(), name.colno());
}

template<> void SemErrorDriver<ErrorDriver>::
fatalErr(const ErrorType & type, const ExprPtr & pExpr) {
    _pErrDrv->fatalErr(type, _filename, pExpr->lineno(), pExpr->colno());
}

template<> const Error & SemErrorDriver<ErrorDriver>::
lastErr() const {
    return _pErrDrv->lastErr();
}

template<> bool SemErrorDriver<ErrorDriver>::
hasErr() const {
    return _pErrDrv->hasErr();
}

template<> void SemErrorDriver<ErrorDriver>::
clearLastErr() {
    _pErrDrv->clearLastErr();
}

template<> void SemErrorDriver<ErrorDriver>::
clearLastErr(uint32_t n) {
    _pErrDrv->clearLastErr(n);
}

template<> uint32_t SemErrorDriver<ErrorDriver>::
getErrCount() const {
    return _pErrDrv->getErrList().size();
}

template<> const std::vector<Error> & SemErrorDriver<ErrorDriver>::
getErrList() const {
    return _pErrDrv->getErrList();
}

template<> std::string SemErrorDriver<ErrorDriver>::
getFileName() const {
    return _filename;
}

template<> void SemErrorDriver<ErrorDriver>::
setFileName(const std::string & newFileName) {
    _filename = newFileName;
}