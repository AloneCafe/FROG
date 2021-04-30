#ifndef __FAKE_SCALAR_RAM_HEADER__
#define __FAKE_SCALAR_RAM_HEADER__

#include <vector>
#include <cstdint>

class FakeScalarRAM {
    friend class GarbageCollector;
private:
    mutable std::vector<char> _staticSRAM;
    mutable std::vector<char> _funcsSRAM;
    
public:
    int8_t  getB(uint32_t addr) const;
    int16_t getW(uint32_t addr) const;
    int32_t getDW(uint32_t addr) const;
    int64_t getQW(uint32_t addr) const;
    float   getFLT(uint32_t addr) const;
    double  getDBL(uint32_t addr) const;
    
    void setB(uint32_t addr, int8_t e);
    void setW(uint32_t addr, int16_t e);
    void setDW(uint32_t addr, int32_t e);
    void setQW(uint32_t addr, int64_t e);
    void setFLT(uint32_t addr, float e);
    void setDBL(uint32_t addr, double e);
    
    void clearNonStaticSRAM();
    
private:
    std::vector<char> *
            tryResize(uint32_t addr, int32_t siz) const;
};

#endif
