#ifndef __VMEXCEPTION_HEADER__
#define __VMEXCEPTION_HEADER__

#include <string>

enum class VMET : int {
    E_OPROM_ACCESS_OVERFLOW,
    E_OPSTACK_ACCESS_OVERFLOW
};

class VMException {
private:
    const static char *_pNativeMessage[];
    
private:
    std::string _message;
    
public:
    VMException(const std::string & nativeMessage);
    VMException(VMET t);
    const std::string & getMessage();
};

#endif
