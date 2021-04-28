#ifndef __SEM_ERROR_DRV_HEADER__
#define __SEM_ERROR_DRV_HEADER__

#include "ErrorDriver.hpp"
#include "ExprDefinition.hpp"

template <typename T = ErrorDriver>
class SemErrorDriver {
private:
    std::shared_ptr<typename std::enable_if<
            std::is_base_of<ErrorDriverBase, T>::value, T>::type>
            _pErrDrv;
    
    std::string _filename;

public:
    explicit SemErrorDriver(const std::string & filename) :
        _pErrDrv(std::make_shared<T>()),
        _filename(filename) { }
    
    virtual ~SemErrorDriver() = default;
    
    void postErr(const ErrorType & type, const LocatedUtfString & name);
    
    void postErr(const ErrorType & type, const ExprPtr & pExpr);
    
    void fatalErr(const ErrorType & type, const LocatedUtfString & name);
    
    void fatalErr(const ErrorType & type, const ExprPtr & pExpr);
    
    const Error & lastErr() const;
    
    bool hasErr() const;
    
    void clearLastErr();
    
    void clearLastErr(size_t n);
    
    size_t getErrCount() const;
    
    const std::vector<Error> & getErrList() const;
    
    std::string getFileName() const;
    
    void setFileName(const std::string & newFileName);
};

#endif