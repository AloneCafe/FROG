#include "VMException.hpp"

const char * VMException::_pNativeMessage[] = {
    "OPROM 时下标访问越界",
    "OPSTACK 发生栈溢出",
    "Scalar-RAM 伸展发生异常"
};

VMException::VMException(const std::string & message) {
    _message = message;
}

const std::string &
VMException::getMessage() {
    return _message;
}

VMException::VMException(VMET t) {
    _message = _pNativeMessage[int(t)];
}
