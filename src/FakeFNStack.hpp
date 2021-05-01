#ifndef __FAKE_FNSTACK_HEADER__
#define __FAKE_FNSTACK_HEADER__

#include <stack>
#include <cstdint>

class FakeFNStack {
private:
    std::stack<uint32_t> _fnStack;

public:
    FakeFNStack() = default;
    
    uint32_t popRetAddr();
    
    void pushRetAddr(uint32_t e);
    
    bool empty() const;
};

#endif
