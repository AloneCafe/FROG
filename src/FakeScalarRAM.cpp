#include "FakeScalarRAM.hpp"
#include "VMException.hpp"

static int vmRandomInit = (srand(time(nullptr)), 0);

std::vector<char> * FakeScalarRAM::tryResize(int32_t addr, int32_t siz) const {
    std::vector<char> * pSRAM;
    if (addr < 0) {
        addr &= 0x7FFFFFFF;
        pSRAM = &_staticSRAM;
    } else {
        pSRAM = &_funcsSRAM;
    }
    if (addr + siz > pSRAM->size()) {
        try {
            pSRAM->resize(addr + siz, 0);
        } catch (std::exception &) {
            throw VMException(VMET::E_SRAM_REISZE_ERROR);
        }
    }
    return pSRAM;
}

int8_t FakeScalarRAM::getB(int32_t addr) const {
    std::vector<char> * pSRAM =
            tryResize(addr, sizeof(int8_t));
    return (*pSRAM)[addr];
}

int16_t FakeScalarRAM::getW(int32_t addr) const {
    std::vector<char> * pSRAM =
            tryResize(addr, sizeof(int16_t));
    int16_t e;
    for (uint32_t i = 0; i < sizeof(int16_t); ++i) {
        reinterpret_cast<char *>(&e)[i] = (*pSRAM)[addr + i];
    }
    return e;
}


int32_t FakeScalarRAM::getDW(int32_t addr) const {
    std::vector<char> * pSRAM =
            tryResize(addr, sizeof(int32_t));
    int32_t e;
    for (uint32_t i = 0; i < sizeof(int32_t); ++i) {
        reinterpret_cast<char *>(&e)[i] = (*pSRAM)[addr + i];
    }
    return e;
}

int64_t FakeScalarRAM::getQW(int32_t addr) const {
    std::vector<char> * pSRAM =
            tryResize(addr, sizeof(int64_t));
    int64_t e;
    for (uint32_t i = 0; i < sizeof(int64_t); ++i) {
        reinterpret_cast<char *>(&e)[i] = (*pSRAM)[addr + i];
    }
    return e;
}

float FakeScalarRAM::getFLT(int32_t addr) const {
    std::vector<char> * pSRAM =
            tryResize(addr, sizeof(float));
    float e;
    for (uint32_t i = 0; i < sizeof(float); ++i) {
        reinterpret_cast<char *>(&e)[i] = (*pSRAM)[addr + i];
    }
    return e;
}

double FakeScalarRAM::getDBL(int32_t addr) const {
    std::vector<char> * pSRAM =
            tryResize(addr, sizeof(double));
    double e;
    for (uint32_t i = 0; i < sizeof(double); ++i) {
        reinterpret_cast<char *>(&e)[i] = (*pSRAM)[addr + i];
    }
    return e;
}

void FakeScalarRAM::setB(int32_t addr, int8_t e) {
    std::vector<char> * pSRAM =
            tryResize(addr, sizeof(int8_t));
    for (uint32_t i = 0; i < sizeof(int8_t); ++i) {
        (*pSRAM)[addr + i] = reinterpret_cast<char *>(&e)[i];
    }
}

void FakeScalarRAM::setW(int32_t addr, int16_t e) {
    std::vector<char> * pSRAM =
            tryResize(addr, sizeof(int16_t));
    for (uint32_t i = 0; i < sizeof(int16_t); ++i) {
        (*pSRAM)[addr + i] = reinterpret_cast<char *>(&e)[i];
    }
}

void FakeScalarRAM::setDW(int32_t addr, int32_t e) {
    std::vector<char> * pSRAM =
            tryResize(addr, sizeof(int32_t));
    for (uint32_t i = 0; i < sizeof(int32_t); ++i) {
        (*pSRAM)[addr + i] = reinterpret_cast<char *>(&e)[i];
    }
}

void FakeScalarRAM::setQW(int32_t addr, int64_t e) {
    std::vector<char> * pSRAM =
            tryResize(addr, sizeof(int64_t));
    for (uint32_t i = 0; i < sizeof(int64_t); ++i) {
        (*pSRAM)[addr + i] = reinterpret_cast<char *>(&e)[i];
    }
}

void FakeScalarRAM::setFLT(int32_t addr, float e) {
    std::vector<char> * pSRAM =
            tryResize(addr, sizeof(float));
    for (uint32_t i = 0; i < sizeof(float); ++i) {
        (*pSRAM)[addr + i] = reinterpret_cast<char *>(&e)[i];
    }
}

void FakeScalarRAM::setDBL(int32_t addr, double e) {
    std::vector<char> * pSRAM =
            tryResize(addr, sizeof(double));
    for (uint32_t i = 0; i < sizeof(double); ++i) {
        (*pSRAM)[addr + i] = reinterpret_cast<char *>(&e)[i];
    }
}

void FakeScalarRAM::clearNonStaticSRAM() {
    _funcsSRAM.clear();
}
