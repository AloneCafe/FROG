#ifndef __FAKE_OPSTACK_HEADER__
#define __FAKE_OPSTACK_HEADER__

#include <vector>
#include <cstdint>

class FakeOPStack {
    friend class GarbageCollector;
private:
    std::vector<char> _opStack;
    
public:
    FakeOPStack() = default;
    
    int8_t  popB();
    int16_t popW();
    int32_t popDW();
    int64_t popQW();
    float   popFLT();
    double  popDBL();
    
    int8_t  topB();
    int16_t topW();
    int32_t topDW();
    int64_t topQW();
    float   topFLT();
    double  topDBL();
    
    void pushB(int8_t e);
    void pushW(int16_t e);
    void pushDW(int32_t e);
    void pushQW(int64_t e);
    void pushFLT(float e);
    void pushDBL(double e);
};

#endif
