#ifndef __FAKE_SCALAR_RAM_HEADER__
#define __FAKE_SCALAR_RAM_HEADER__

#include <stack>
#include <vector>
#include <cstdint>

class FakeScalarRAM {
    friend class GarbageCollector;
private:
    mutable std::vector<char> _staticSRAM;
    mutable std::vector<std::vector<char>> _funcsSRAM;
    
public:
    int8_t  getB(int32_t addr) const;
    int16_t getW(int32_t addr) const;
    int32_t getDW(int32_t addr) const;
    int64_t getQW(int32_t addr) const;
    float   getFLT(int32_t addr) const;
    double  getDBL(int32_t addr) const;
    
    void setB(int32_t addr, int8_t e);
    void setW(int32_t addr, int16_t e);
    void setDW(int32_t addr, int32_t e);
    void setQW(int32_t addr, int64_t e);
    void setFLT(int32_t addr, float e);
    void setDBL(int32_t addr, double e);
    
    void newNonStaticSRAM();
    void clearNonStaticSRAM();
    
    FakeScalarRAM();
    
private:
    std::vector<char> *
            tryResize(int32_t & addr, int32_t siz) const;
};

#endif
