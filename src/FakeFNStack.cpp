#include "FakeFNStack.hpp"

uint32_t FakeFNStack::popRetAddr() {
    uint32_t e = _fnStack.top();
    _fnStack.pop();
    return e;
}

void FakeFNStack::pushRetAddr(uint32_t e) {
    _fnStack.push(e);
}
