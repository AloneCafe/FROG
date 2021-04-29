#ifndef __FAKE_OPROM_HEADER__
#define __FAKE_OPROM_HEADER__

#include <vector>
#include <cstdint>

class FakeOPROM {
private:
    const std::vector<char> _staticByteCodes;
    const std::vector<char> _funcsByteCodes;
    
public:
    FakeOPROM(const std::vector<char> & staticByteCodes,
              const std::vector<char> & funcsByteCodes);
    
    int8_t  getStaticB(uint32_t i) const;
    int16_t getStaticW(uint32_t i) const;
    int32_t getStaticDW(uint32_t i) const;
    int64_t getStaticQW(uint32_t i) const;
    float   getStaticFLT(uint32_t i) const;
    double  getStaticDBL(uint32_t i) const;
    
    int8_t  getFuncsB(uint32_t i) const;
    int16_t getFuncsW(uint32_t i) const;
    int32_t getFuncsDW(uint32_t i) const;
    int64_t getFuncsQW(uint32_t i) const;
    float   getFuncsFLT(uint32_t i) const;
    double  getFuncsDBL(uint32_t i) const;
};

#endif
