#include "FakeOPROM.hpp"
#include "VMException.hpp"

FakeOPROM::FakeOPROM(
        const std::vector<char> & staticByteCodes,
        const std::vector<char> & funcsByteCodes) :
        _staticByteCodes(staticByteCodes),
        _funcsByteCodes(funcsByteCodes) { }

int8_t FakeOPROM::getStaticB(uint32_t i) const {
    if (i + sizeof(int8_t) > _staticByteCodes.size())
        throw VMException(VMET::E_OPROM_ACCESS_OVERFLOW);
    return *(reinterpret_cast<const int8_t *>(&_staticByteCodes[i]));
}

int16_t FakeOPROM::getStaticW(uint32_t i) const {
    if (i + sizeof(int16_t) > _staticByteCodes.size())
        throw VMException(VMET::E_OPROM_ACCESS_OVERFLOW);
    return *(reinterpret_cast<const int16_t *>(&_staticByteCodes[i]));
}

int32_t FakeOPROM::getStaticDW(uint32_t i) const {
    if (i + sizeof(int32_t) > _staticByteCodes.size())
        throw VMException(VMET::E_OPROM_ACCESS_OVERFLOW);
    return *(reinterpret_cast<const int32_t *>(&_staticByteCodes[i]));
}

int64_t FakeOPROM::getStaticQW(uint32_t i) const {
    if (i + sizeof(int64_t) > _staticByteCodes.size())
        throw VMException(VMET::E_OPROM_ACCESS_OVERFLOW);
    return *(reinterpret_cast<const int64_t *>(&_staticByteCodes[i]));
}

float FakeOPROM::getStaticFLT(uint32_t i) const {
    if (i + sizeof(float) > _staticByteCodes.size())
        throw VMException(VMET::E_OPROM_ACCESS_OVERFLOW);
    return *(reinterpret_cast<const float *>(&_staticByteCodes[i]));
}

double FakeOPROM::getStaticDBL(uint32_t i) const {
    if (i + sizeof(double) > _staticByteCodes.size())
        throw VMException(VMET::E_OPROM_ACCESS_OVERFLOW);
    return *(reinterpret_cast<const double *>(&_staticByteCodes[i]));
}


int8_t FakeOPROM::getFuncsB(uint32_t i) const {
    if (i + sizeof(int8_t) > _funcsByteCodes.size())
        throw VMException(VMET::E_OPROM_ACCESS_OVERFLOW);
    return *(reinterpret_cast<const int8_t *>(&_funcsByteCodes[i]));
}

int16_t FakeOPROM::getFuncsW(uint32_t i) const {
    if (i + sizeof(int16_t) > _funcsByteCodes.size())
        throw VMException(VMET::E_OPROM_ACCESS_OVERFLOW);
    return *(reinterpret_cast<const int16_t *>(&_funcsByteCodes[i]));
}

int32_t FakeOPROM::getFuncsDW(uint32_t i) const {
    if (i + sizeof(int32_t) > _funcsByteCodes.size())
        throw VMException(VMET::E_OPROM_ACCESS_OVERFLOW);
    return *(reinterpret_cast<const int32_t *>(&_funcsByteCodes[i]));
}

int64_t FakeOPROM::getFuncsQW(uint32_t i) const {
    if (i + sizeof(int64_t) > _funcsByteCodes.size())
        throw VMException(VMET::E_OPROM_ACCESS_OVERFLOW);
    return *(reinterpret_cast<const int64_t *>(&_funcsByteCodes[i]));
}

float FakeOPROM::getFuncsFLT(uint32_t i) const {
    if (i + sizeof(float) > _funcsByteCodes.size())
        throw VMException(VMET::E_OPROM_ACCESS_OVERFLOW);
    return *(reinterpret_cast<const float *>(&_funcsByteCodes[i]));
}

double FakeOPROM::getFuncsDBL(uint32_t i) const {
    if (i + sizeof(double) > _funcsByteCodes.size())
        throw VMException(VMET::E_OPROM_ACCESS_OVERFLOW);
    return *(reinterpret_cast<const double *>(&_funcsByteCodes[i]));
}