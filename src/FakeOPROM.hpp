#ifndef __FAKEOPROM_HEADER__
#define __FAKEOPROM_HEADER__

#include <vector>
#include <cstdint>

class FakeOPROM {
private:
    const std::vector<char> _staticByteCodes;
    const std::vector<char> _funcsByteCodes;
    
public:
    FakeOPROM(const std::vector<char> & staticByteCodes,
              const std::vector<char> & funcsByteCodes);
    
    int8_t  getStaticB(size_t i) const;
    int16_t getStaticW(size_t i) const;
    int32_t getStaticDW(size_t i) const;
    int64_t getStaticQW(size_t i) const;
    float   getStaticFLT(size_t i) const;
    double  getStaticDBL(size_t i) const;
    
    int8_t  getFuncsB(size_t i) const;
    int16_t getFuncsW(size_t i) const;
    int32_t getFuncsDW(size_t i) const;
    int64_t getFuncsQW(size_t i) const;
    float   getFuncsFLT(size_t i) const;
    double  getFuncsDBL(size_t i) const;
};

#endif
