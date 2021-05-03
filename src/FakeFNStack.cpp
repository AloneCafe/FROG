#include "FakeFNStack.hpp"
#include "VMException.hpp"

uint32_t FakeFNStack::popRetAddr() {
    if (_fnStack.empty()) {
        throw VMException(VMET::E_FNSTACK_ACCESS_OVERFLOW);
    }
    uint32_t e = _fnStack.top();
    _fnStack.pop();
    return e;
}

void FakeFNStack::pushRetAddr(uint32_t e) {
    _fnStack.push(e);
}

bool FakeFNStack::empty() const {
    return _fnStack.empty();
}
