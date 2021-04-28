#ifndef __FAKEOPSTACK_HEADER__
#define __FAKEOPSTACK_HEADER__

#include <stack>
#include <cstdint>

class FakeOPStack {
private:
    std::stack<char> _opStack;
    
public:
    FakeOPStack() = default;
    
    int8_t  popB();
    int16_t popW();
    int32_t popDW();
    int64_t popQW();
    float   popFLT();
    double  popDBL();
    
    void pushB(int8_t e);
    void pushW(int16_t e);
    void pushDW(int32_t e);
    void pushQW(int64_t e);
    void pushFLT(float e);
    void pushDBL(double e);
};

#endif
