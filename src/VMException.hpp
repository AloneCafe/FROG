#ifndef __VMEXCEPTION_HEADER__
#define __VMEXCEPTION_HEADER__

#include <string>

enum class VMET : int {
    E_OPROM_ACCESS_OVERFLOW,
    E_OPSTACK_ACCESS_OVERFLOW,
    E_SRAM_REISZE_ERROR,
    E_NOT_ATTACH_OPROM,
    E_NOT_ATTACH_OPSTACK,
    E_NOT_ATTACH_FNSTACK,
    E_NOT_ATTACH_SRAM,
    E_NOT_ATTACH_VRAM,
    E_ILLEGAL_GRANULARITY,
    E_DIVIDE_BY_ZERO,
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
