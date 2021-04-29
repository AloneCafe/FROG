#include "SyntaxTreeErrorDriver.hpp"

template<> void AstErrorDriver<ErrorDriver>::
postErr(const ErrorType & type, const Token & tk) {
    _pErrDrv->postErr(type, _filename, tk.lineno(), tk.colno());
}

template<> void AstErrorDriver<ErrorDriver>::
fatalErr(const ErrorType & type, const Token & tk) {
    _pErrDrv->fatalErr(type, _filename, tk.lineno(), tk.colno());
}

template<> const Error &
AstErrorDriver<ErrorDriver>::
lastErr() const {
    return _pErrDrv->lastErr();
}

template<> bool AstErrorDriver<ErrorDriver>::
hasErr() const {
    return _pErrDrv->hasErr();
}

template<> void AstErrorDriver<ErrorDriver>::
clearLastErr() {
    _pErrDrv->clearLastErr();
}

template<> void AstErrorDriver<ErrorDriver>::
clearLastErr(uint32_t n) {
    _pErrDrv->clearLastErr(n);
}

template<> uint32_t AstErrorDriver<ErrorDriver>::
getErrCount() const {
    return _pErrDrv->getErrList().size();
}

template<> const std::vector<Error> &
AstErrorDriver<ErrorDriver>::
getErrList() const {
    return _pErrDrv->getErrList();
}

template<> std::string AstErrorDriver<ErrorDriver>::
getFileName() const {
    return _filename;
}

template<> void AstErrorDriver<ErrorDriver>::
setFileName(const std::string & newFileName) {
    _filename = newFileName;
}