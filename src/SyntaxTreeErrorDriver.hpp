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
    
    void postErr(const ErrorType & type, const Token & tk);
    
    void fatalErr(const ErrorType & type, const Token & tk);
    
    const Error & lastErr() const;
    
    bool hasErr() const;
    
    void clearLastErr();
    
    void clearLastErr(uint32_t n);
    
    uint32_t getErrCount() const;
    
    const std::vector<Error> & getErrList() const;
    
    std::string getFileName() const;
    
    void setFileName(const std::string & newFileName);
};

#endif
