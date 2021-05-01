#ifndef __FAKE_CPU_HEADER__
#define __FAKE_CPU_HEADER__

#include "FakeOPROM.hpp"
#include "FakeOPStack.hpp"
#include "FakeFNStack.hpp"
#include "FakeScalarRAM.hpp"
#include "FakeVectorRAM.hpp"

class FakeCPU {
private:
    FakeOPROM     * _pOPROM   = nullptr;
    FakeOPStack   * _pOPStack = nullptr;
    FakeFNStack   * _pFNStack = nullptr;
    FakeScalarRAM * _pSRAM    = nullptr;
    FakeVectorRAM * _pVRAM    = nullptr;
    
public:
    FakeCPU() = default;
    FakeCPU(const FakeCPU &) = delete;
    
    void attachOPROM(FakeOPROM * pOPROM);
    void attachOPStack(FakeOPStack * pOPStack);
    void attachFNStack(FakeFNStack * pFNStack);
    void attachScalarRAM(FakeScalarRAM * pSRAM);
    void attachVectorRAM(FakeVectorRAM * pVRAM);
    
    int32_t runFuncs(bool verbose, bool step, uint32_t startAddr);
    int32_t runStatic(bool verbose, bool step);

private:
    int32_t run(bool verbose, bool step,
             bool fromStaticByteCodes, uint32_t startAddr);
    
};

#endif
