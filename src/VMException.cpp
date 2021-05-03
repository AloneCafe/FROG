#include "VMException.hpp"

const char * VMException::_pNativeMessage[] = {
    "OPROM 时下标访问越界",
    "FNSTACK 发生栈溢出",
    "OPSTACK 发生栈溢出",
    "Scalar-RAM 伸展发生异常",
    "虚拟 CPU 未连接到 OPROM",
    "虚拟 CPU 未连接到 OPSTACK",
    "虚拟 CPU 未连接到 FNSTACK",
    "虚拟 CPU 未连接到 Scalar-RAM",
    "虚拟 CPU 未连接到 Vector-RAM",
    "虚拟 CPU 指令含有非法的粒度",
    "非法算术运算: 尝试除以零",
    "非法的矢量句柄",
    "非法的虚拟 CPU 指令",
    "非法的虚拟机扩展功能名",
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
