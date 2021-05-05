#include <cstring>
#include <cmath>
#include <cassert>
#include <iostream>
#include <sstream>

#include "FakeCPU.hpp"
#include "VMException.hpp"
#include "GarbageCollector.hpp"

void FakeCPU::attachOPROM(FakeOPROM * pOPROM) {
    _pOPROM = pOPROM;
}

void FakeCPU::attachOPStack(FakeOPStack * pOPStack) {
    _pOPStack = pOPStack;
}

void FakeCPU::attachFNStack(FakeFNStack * pFNStack) {
    _pFNStack = pFNStack;
}

void FakeCPU::attachScalarRAM(FakeScalarRAM * pSRAM) {
    _pSRAM = pSRAM;
}

void FakeCPU::attachVectorRAM(FakeVectorRAM * pVRAM) {
    _pVRAM = pVRAM;
}

int32_t FakeCPU::run(bool verbose, bool step, bool fromStaticByteCodes, uint32_t startAddr) {
    if (!_pOPROM)
        throw VMException(VMET::E_NOT_ATTACH_OPROM);
    if (!_pOPStack)
        throw VMException(VMET::E_NOT_ATTACH_OPSTACK);
    if (!_pFNStack)
        throw VMException(VMET::E_NOT_ATTACH_FNSTACK);
    if (!_pSRAM)
        throw VMException(VMET::E_NOT_ATTACH_SRAM);
    if (!_pVRAM)
        throw VMException(VMET::E_NOT_ATTACH_VRAM);
    
    const std::vector<char> & oprom = fromStaticByteCodes ?
            _pOPROM->_staticByteCodes : _pOPROM->_funcsByteCodes;
    uint32_t siz = oprom.size();
    uint32_t pc = startAddr;
    
    try {
        while (pc < siz) {
            GCLockGuard lck(GCScheduler::getGCLock());
            std::string buf;
            uint32_t hardAddr = 0;
            switch (oprom[pc]) {
            case 0x00: {
                ++pc;
                break;
            }
        
            case 0x01: {
                ++pc;
                return _pOPStack->popDW();
                break;
            }
        
            case 0x02: {
                ++pc;
                while (oprom[pc] != 0x00)
                    buf.push_back(oprom[pc++]);
                ++pc;
                executeVMEF(buf);
                break;
            }
        
            case 0x03: {
                ++pc;
                uint32_t j;
                for (j = 0; j < sizeof(uint32_t); ++j)
                    reinterpret_cast<char *>(&hardAddr)[j] = oprom[pc + j];
                pc += j;
                _pFNStack->pushRetAddr(pc);
                pc = hardAddr;  // 设置程序计数器
                break;
            }
        
            case 0x05: //  暂时通用 RET 与 NRET
                pc += 1;
            case 0x04:
                pc += 1;
            
                // 检查程序是否结束，判断函数返回栈是否为空
                if (_pFNStack->empty()) {
                    if (_pOPStack->_opStack.size() >= 4)
                        return _pOPStack->popDW();
                    else if (!_pOPStack->_opStack.empty())
                        return _pOPStack->popB();
                    else
                        return 0;
                
                } else {
                    pc = _pFNStack->popRetAddr();
                }
                break;
        
            case 0x06: {
                ++pc;
                uint8_t gh = *reinterpret_cast
                        <const uint8_t *>(&oprom[pc]);
                uint8_t hb[2];
                hb[0] = (gh >> 4);
                hb[1] = (gh & 0x0F);
            
                ++pc;
            
                if (hb[0] == 0B0000) { // src: VOID
                    if (hb[1] == 0B0000) { // dst: VOID
                        memset(voidhole, 0, sizeof(voidhole)); // VOID 对 VOID 暂且清空 voidhole
                    } else if (hb[1] == 0B0001) { // dst: B
                        int8_t b = *reinterpret_cast<int8_t *>(voidhole);
                        _pOPStack->pushB(b);
                    } else if (hb[1] == 0B0010) { // dst: W
                        int16_t w = *reinterpret_cast<int16_t *>(voidhole);
                        _pOPStack->pushW(w);
                    } else if (hb[1] == 0B0100) { // dst: DW
                        int32_t dw = *reinterpret_cast<int32_t *>(voidhole);
                        _pOPStack->pushDW(dw);
                    } else if (hb[1] == 0B1000) { // dst: QW
                        int64_t qw = *reinterpret_cast<int64_t *>(voidhole);
                        _pOPStack->pushQW(qw);
                    } else if (hb[1] == 0B1011) { // dst: FLT
                        float f = *reinterpret_cast<float *>(voidhole);
                        _pOPStack->pushFLT(f);
                    } else if (hb[1] == 0B1111) { // dst: DBL
                        double d = *reinterpret_cast<double *>(voidhole);
                        _pOPStack->pushDBL(d);
                    } else {
                        throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                    }
                
                } else if (hb[0] == 0B0001) { // src: B
                    auto b = _pOPStack->popB();
                    if (hb[1] == 0B0000) { // dst: VOID
                        memset(voidhole, 0, sizeof(voidhole));
                        *reinterpret_cast<int8_t *>(voidhole) = b;
                    } else if (hb[1] == 0B0001) { // dst: B
                        ;
                    } else if (hb[1] == 0B0010) { // dst: W
                        _pOPStack->pushW(b);
                    } else if (hb[1] == 0B0100) { // dst: DW
                        _pOPStack->pushDW(b);
                    } else if (hb[1] == 0B1000) { // dst: QW
                        _pOPStack->pushQW(b);
                    } else if (hb[1] == 0B1011) { // dst: FLT
                        _pOPStack->pushFLT(b);
                    } else if (hb[1] == 0B1111) { // dst: DBL
                        _pOPStack->pushDBL(b);
                    } else {
                        throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                    }
                
                } else if (hb[0] == 0B0010) { // src: W
                    auto w = _pOPStack->popW();
                    if (hb[1] == 0B0000) { // dst: VOID
                        memset(voidhole, 0, sizeof(voidhole));
                        *reinterpret_cast<int16_t *>(voidhole) = w;
                    } else if (hb[1] == 0B0001) { // dst: B
                        _pOPStack->pushB(w);
                    } else if (hb[1] == 0B0010) { // dst: W
                        ;
                    } else if (hb[1] == 0B0100) { // dst: DW
                        _pOPStack->pushDW(w);
                    } else if (hb[1] == 0B1000) { // dst: QW
                        _pOPStack->pushQW(w);
                    } else if (hb[1] == 0B1011) { // dst: FLT
                        _pOPStack->pushFLT(w);
                    } else if (hb[1] == 0B1111) { // dst: DBL
                        _pOPStack->pushDBL(w);
                    } else {
                        throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                    }
                
                } else if (hb[0] == 0B0100) { // src: DW
                    auto dw = _pOPStack->popDW();
                    if (hb[1] == 0B0000) { // dst: VOID
                        memset(voidhole, 0, sizeof(voidhole));
                        *reinterpret_cast<int32_t *>(voidhole) = dw;
                    } else if (hb[1] == 0B0001) { // dst: B
                        _pOPStack->pushB(dw);
                    } else if (hb[1] == 0B0010) { // dst: W
                        _pOPStack->pushW(dw);
                    } else if (hb[1] == 0B0100) { // dst: DW
                        ;
                    } else if (hb[1] == 0B1000) { // dst: QW
                        _pOPStack->pushQW(dw);
                    } else if (hb[1] == 0B1011) { // dst: FLT
                        _pOPStack->pushFLT(dw);
                    } else if (hb[1] == 0B1111) { // dst: DBL
                        _pOPStack->pushDBL(dw);
                    } else {
                        throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                    }
                
                } else if (hb[0] == 0B1000) { // src: QW
                    auto qw = _pOPStack->popQW();
                    if (hb[1] == 0B0000) { // dst: VOID
                        memset(voidhole, 0, sizeof(voidhole));
                        *reinterpret_cast<int64_t *>(voidhole) = qw;
                    } else if (hb[1] == 0B0001) { // dst: B
                        _pOPStack->pushB(qw);
                    } else if (hb[1] == 0B0010) { // dst: W
                        _pOPStack->pushW(qw);
                    } else if (hb[1] == 0B0100) { // dst: DW
                        _pOPStack->pushDW(qw);
                    } else if (hb[1] == 0B1000) { // dst: QW
                        ;
                    } else if (hb[1] == 0B1011) { // dst: FLT
                        _pOPStack->pushFLT(qw);
                    } else if (hb[1] == 0B1111) { // dst: DBL
                        _pOPStack->pushDBL(qw);
                    } else {
                        throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                    }
                
                } else if (hb[0] == 0B1011) { // src: FLT
                    auto f = _pOPStack->popFLT();
                    if (hb[1] == 0B0000) { // dst: VOID
                        memset(voidhole, 0, sizeof(voidhole));
                        *reinterpret_cast<float *>(voidhole) = f;
                    } else if (hb[1] == 0B0001) { // dst: B
                        _pOPStack->pushB(f);
                    } else if (hb[1] == 0B0010) { // dst: W
                        _pOPStack->pushW(f);
                    } else if (hb[1] == 0B0100) { // dst: DW
                        _pOPStack->pushDW(f);
                    } else if (hb[1] == 0B1000) { // dst: QW
                        _pOPStack->pushQW(f);
                    } else if (hb[1] == 0B1011) { // dst: FLT
                        ;
                    } else if (hb[1] == 0B1111) { // dst: DBL
                        _pOPStack->pushDBL(f);
                    } else {
                        throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                    }
                
                } else if (hb[0] == 0B1111) { // src: DBL
                    auto d = _pOPStack->popDBL();
                    if (hb[1] == 0B0000) { // dst: VOID
                        memset(voidhole, 0, sizeof(voidhole));
                        *reinterpret_cast<double *>(voidhole) = d;
                    } else if (hb[1] == 0B0001) { // dst: B
                        _pOPStack->pushB(d);
                    } else if (hb[1] == 0B0010) { // dst: W
                        _pOPStack->pushW(d);
                    } else if (hb[1] == 0B0100) { // dst: DW
                        _pOPStack->pushDW(d);
                    } else if (hb[1] == 0B1000) { // dst: QW
                        _pOPStack->pushQW(d);
                    } else if (hb[1] == 0B1011) { // dst: FLT
                        _pOPStack->pushFLT(d);
                    } else if (hb[1] == 0B1111) { // dst: DBL
                        ;
                    } else {
                        throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                    }
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x08: { // BAND
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b1 & b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    _pOPStack->pushW(w1 & w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    _pOPStack->pushDW(dw1 & dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    _pOPStack->pushDW(qw1 & qw2);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x09: { // BOR
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b1 | b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    _pOPStack->pushW(w1 | w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    _pOPStack->pushDW(dw1 | dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    _pOPStack->pushDW(qw1 | qw2);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x0A: { // BXOR
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b1 ^ b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    _pOPStack->pushW(w1 ^ w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    _pOPStack->pushDW(dw1 ^ dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    _pOPStack->pushDW(qw1 ^ qw2);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x0B: { // BNOT
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    _pOPStack->pushB(~b1);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    _pOPStack->pushW(~w1);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    _pOPStack->pushDW(~dw1);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    _pOPStack->pushQW(~qw1);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x0C: { // SHL
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto bits = _pOPStack->popB();
                    _pOPStack->pushB(b1 << bits);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto bits = _pOPStack->popB();
                    _pOPStack->pushW(w1 << bits);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto bits = _pOPStack->popB();
                    _pOPStack->pushDW(dw1 << bits);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto bits = _pOPStack->popB();
                    _pOPStack->pushQW(qw1 << bits);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x0D: { // SHR
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto bits = _pOPStack->popB();
                    _pOPStack->pushB(b1 >> bits);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto bits = _pOPStack->popB();
                    _pOPStack->pushW(w1 >> bits);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto bits = _pOPStack->popB();
                    _pOPStack->pushDW(dw1 >> bits);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto bits = _pOPStack->popB();
                    _pOPStack->pushQW(qw1 >> bits);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x0E: { // SHRZ
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    uint8_t b1 = _pOPStack->popB();
                    auto bits = _pOPStack->popB();
                    _pOPStack->pushB(b1 >> bits);
                
                } else if (g == 0B0010) { // dst: W
                    uint16_t w1 = _pOPStack->popW();
                    auto bits = _pOPStack->popB();
                    _pOPStack->pushW(w1 >> bits);
                
                } else if (g == 0B0100) { // dst: DW
                    uint32_t dw1 = _pOPStack->popDW();
                    auto bits = _pOPStack->popB();
                    _pOPStack->pushDW(dw1 >> bits);
                
                } else if (g == 0B1000) { // dst: QW
                    uint64_t qw1 = _pOPStack->popQW();
                    auto bits = _pOPStack->popB();
                    _pOPStack->pushQW(qw1 >> bits);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x10: { // LT
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b1 < b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    _pOPStack->pushB(w1 < w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    _pOPStack->pushB(dw1 < dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    _pOPStack->pushB(qw1 < qw2);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    auto f2 = _pOPStack->popFLT();
                    _pOPStack->pushB(f1 < f2);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    auto d2 = _pOPStack->popDBL();
                    _pOPStack->pushB(d1 < d2);
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x11: { // LE
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b1 <= b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    _pOPStack->pushB(w1 <= w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    _pOPStack->pushB(dw1 <= dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    _pOPStack->pushB(qw1 <= qw2);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    auto f2 = _pOPStack->popFLT();
                    _pOPStack->pushB(f1 <= f2);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    auto d2 = _pOPStack->popDBL();
                    _pOPStack->pushB(d1 <= d2);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x12: { // EQ
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b1 == b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    _pOPStack->pushB(w1 == w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    _pOPStack->pushB(dw1 == dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    _pOPStack->pushB(qw1 == qw2);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    auto f2 = _pOPStack->popFLT();
                    _pOPStack->pushB(f1 == f2);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    auto d2 = _pOPStack->popDBL();
                    _pOPStack->pushB(d1 == d2);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x13: { // NE
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b1 != b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    _pOPStack->pushB(w1 != w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    _pOPStack->pushB(dw1 != dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    _pOPStack->pushB(qw1 != qw2);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    auto f2 = _pOPStack->popFLT();
                    _pOPStack->pushB(f1 != f2);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    auto d2 = _pOPStack->popDBL();
                    _pOPStack->pushB(d1 != d2);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x14: { // GE
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b1 >= b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    _pOPStack->pushB(w1 >= w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    _pOPStack->pushB(dw1 >= dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    _pOPStack->pushB(qw1 >= qw2);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    auto f2 = _pOPStack->popFLT();
                    _pOPStack->pushB(f1 >= f2);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    auto d2 = _pOPStack->popDBL();
                    _pOPStack->pushB(d1 >= d2);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x15: { // GT
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b1 > b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    _pOPStack->pushB(w1 > w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    _pOPStack->pushB(dw1 > dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    _pOPStack->pushB(qw1 > qw2);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    auto f2 = _pOPStack->popFLT();
                    _pOPStack->pushB(f1 > f2);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    auto d2 = _pOPStack->popDBL();
                    _pOPStack->pushB(d1 > d2);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x18: { // LNOT
                ++pc;
                auto b1 = _pOPStack->popB();
                _pOPStack->pushB(!b1);
                break;
            }
        
            case 0x19: { // LOR
                ++pc;
                auto b1 = _pOPStack->popB();
                if (b1) { // 短路
                    _pOPStack->popB();
                    _pOPStack->pushB(true);
                    break;
                } else {
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b2);
                }
                break;
            }
        
            case 0x1A: { // LAND
                ++pc;
                auto b1 = _pOPStack->popB();
                if (b1) { // 短路
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b2);
                
                } else {
                    _pOPStack->popB();
                    _pOPStack->pushB(false);
                    break;
                }
                break;
            }
        
            case 0x20: { // ADD
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b1 + b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    _pOPStack->pushW(w1 + w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    _pOPStack->pushDW(dw1 + dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    _pOPStack->pushQW(qw1 + qw2);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    auto f2 = _pOPStack->popFLT();
                    _pOPStack->pushFLT(f1 + f2);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    auto d2 = _pOPStack->popDBL();
                    _pOPStack->pushDBL(d1 + d2);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x21: { // SUB
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b1 - b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    _pOPStack->pushW(w1 - w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    _pOPStack->pushDW(dw1 - dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    _pOPStack->pushQW(qw1 - qw2);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    auto f2 = _pOPStack->popFLT();
                    _pOPStack->pushFLT(f1 - f2);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    auto d2 = _pOPStack->popDBL();
                    _pOPStack->pushDBL(d1 - d2);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x22: { // MUL
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    _pOPStack->pushB(b1 * b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    _pOPStack->pushW(w1 * w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    _pOPStack->pushDW(dw1 * dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    _pOPStack->pushQW(qw1 * qw2);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    auto f2 = _pOPStack->popFLT();
                    _pOPStack->pushFLT(f1 * f2);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    auto d2 = _pOPStack->popDBL();
                    _pOPStack->pushDBL(d1 * d2);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x23: { // DIV
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    if (b2 == 0)
                        throw VMException(VMET::E_DIVIDE_BY_ZERO);
                    _pOPStack->pushB(b1 / b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    if (w2 == 0)
                        throw VMException(VMET::E_DIVIDE_BY_ZERO);
                    _pOPStack->pushW(w1 / w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    if (dw2 == 0)
                        throw VMException(VMET::E_DIVIDE_BY_ZERO);
                    _pOPStack->pushDW(dw1 / dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    if (qw2 == 0)
                        throw VMException(VMET::E_DIVIDE_BY_ZERO);
                    _pOPStack->pushQW(qw1 / qw2);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    auto f2 = _pOPStack->popFLT();
                    _pOPStack->pushFLT(f1 / f2);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    auto d2 = _pOPStack->popDBL();
                    _pOPStack->pushDBL(d1 / d2);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x24: { // MOD
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    auto b2 = _pOPStack->popB();
                    if (b2 == 0)
                        throw VMException(VMET::E_DIVIDE_BY_ZERO);
                    _pOPStack->pushB(b1 % b2);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    auto w2 = _pOPStack->popW();
                    if (w2 == 0)
                        throw VMException(VMET::E_DIVIDE_BY_ZERO);
                    _pOPStack->pushW(w1 % w2);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    auto dw2 = _pOPStack->popDW();
                    if (dw2 == 0)
                        throw VMException(VMET::E_DIVIDE_BY_ZERO);
                    _pOPStack->pushDW(dw1 % dw2);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    auto qw2 = _pOPStack->popQW();
                    if (qw2 == 0)
                        throw VMException(VMET::E_DIVIDE_BY_ZERO);
                    _pOPStack->pushQW(qw1 % qw2);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    auto f2 = _pOPStack->popFLT();
                    _pOPStack->pushFLT(fmod(f1, f2));
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    auto d2 = _pOPStack->popDBL();
                    _pOPStack->pushDBL(fmod(d1, d2));
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x25: { // NEG
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    _pOPStack->pushB(-b1);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    _pOPStack->pushW(-w1);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    _pOPStack->pushDW(-dw1);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    _pOPStack->pushQW(-qw1);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    _pOPStack->pushFLT(-f1);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    _pOPStack->pushDBL(-d1);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
                break;
            }
        
            case 0x30: { // OFFSET
                ++pc;
                auto off = _pOPStack->popDW();
                auto handler = _pOPStack->popDW();
                ElemHandler native = _pVRAM->getElemHandlerByOffsetT(handler, off);
            
                if (sizeof(ElemHandler) == 4) {
                    uint32_t dw = 0;
                    for (uint32_t i = 0; i < sizeof(ElemHandler); ++i) {
                        reinterpret_cast<uint8_t *>(&dw)[i] =
                                reinterpret_cast<uint8_t *>(&native)[i];
                    }
                    _pOPStack->pushDW(dw);
                
                } else if (sizeof(ElemHandler) == 8) {
                    uint32_t qw = 0;
                    for (uint32_t i = 0; i < sizeof(ElemHandler); ++i) {
                        reinterpret_cast<uint8_t *>(&qw)[i] =
                                reinterpret_cast<uint8_t *>(&native)[i];
                    }
                    _pOPStack->pushQW(qw);
                
                } else {
                    assert(0);
                }
                break;
            }
        
            case 0x31: { // HPUSH
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
                ElemHandler native;
                if (sizeof(ElemHandler) == 4) {
                    uint32_t dw = _pOPStack->popDW();
                    for (uint32_t i = 0; i < sizeof(ElemHandler); ++i) {
                        reinterpret_cast<uint8_t *>(&native)[i] =
                                reinterpret_cast<uint8_t *>(&dw)[i];
                    }
                
                } else if (sizeof(ElemHandler) == 8) {
                    uint32_t qw = _pOPStack->popQW();
                    for (uint32_t i = 0; i < sizeof(ElemHandler); ++i) {
                        reinterpret_cast<uint8_t *>(&native)[i] =
                                reinterpret_cast<uint8_t *>(&qw)[i];
                    }
                
                } else {
                    assert(0);
                }
            
                if (g == 0B0001) { // dst: B
                    auto b1 = *static_cast<int8_t *>(native);
                    _pOPStack->pushB(b1);
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = *static_cast<int16_t *>(native);
                    _pOPStack->pushW(w1);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = *static_cast<int32_t *>(native);
                    _pOPStack->pushDW(dw1);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = *static_cast<int64_t *>(native);
                    _pOPStack->pushQW(qw1);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = *static_cast<float *>(native);
                    _pOPStack->pushFLT(f1);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = *static_cast<double *>(native);
                    _pOPStack->pushDBL(d1);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
            
                break;
            }
        
            case 0x32: { // HPOP
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                auto fPopNativeAddr = [](FakeOPStack * pOPStack) {
                    ElemHandler native;
                    if (sizeof(ElemHandler) == 4) {
                        uint32_t dw = pOPStack->popDW();
                        for (uint32_t i = 0; i < sizeof(ElemHandler); ++i) {
                            reinterpret_cast<uint8_t *>(&native)[i] =
                                    reinterpret_cast<uint8_t *>(&dw)[i];
                        }
                    
                    } else if (sizeof(ElemHandler) == 8) {
                        uint32_t qw = pOPStack->popQW();
                        for (uint32_t i = 0; i < sizeof(ElemHandler); ++i) {
                            reinterpret_cast<uint8_t *>(&native)[i] =
                                    reinterpret_cast<uint8_t *>(&qw)[i];
                        }
                    
                    } else {
                        assert(0);
                    }
                    return native;
                };
            
                ElemHandler native;
                if (g == 0B0001) { // dst: B
                    auto b1 = _pOPStack->popB();
                    native = fPopNativeAddr(_pOPStack);
                    *static_cast<int8_t *>(native) = b1;
                
                } else if (g == 0B0010) { // dst: W
                    auto w1 = _pOPStack->popW();
                    native = fPopNativeAddr(_pOPStack);
                    *static_cast<int16_t *>(native) = w1;
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw1 = _pOPStack->popDW();
                    native = fPopNativeAddr(_pOPStack);
                    *static_cast<int32_t *>(native) = dw1;
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw1 = _pOPStack->popQW();
                    native = fPopNativeAddr(_pOPStack);
                    *static_cast<int64_t *>(native) = qw1;
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f1 = _pOPStack->popFLT();
                    native = fPopNativeAddr(_pOPStack);
                    *static_cast<float *>(native) = f1;
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d1 = _pOPStack->popDBL();
                    native = fPopNativeAddr(_pOPStack);
                    *static_cast<double *>(native) = d1;
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
            
                break;
            }
        
            case 0x33: { // LEN
                ++pc;
                VectorHandler handler = _pOPStack->popDW();
                _pOPStack->pushDW(
                        _pVRAM->getTotalSizeByHandler(handler) /
                                _pVRAM->getElemSizeByHandler(handler));
                break;
            }
        
            case 0x34: { // MKVEC
                ++pc;
                uint8_t gd = *reinterpret_cast
                        <const uint8_t *>(&oprom[pc]);
                uint8_t g, d;
                g = (gd >> 4);
                d = (gd & 0x0F);
            
                ++pc;
            
                VectorHandler handler;
                if (g == 0B0001) { // dst: B
                    handler = _pVRAM->makeVectorB(d);
                    _pOPStack->pushDW(handler);
                
                } else if (g == 0B0010) { // dst: W
                    handler = _pVRAM->makeVectorW(d);
                    _pOPStack->pushDW(handler);
                
                } else if (g == 0B0100) { // dst: DW
                    handler = _pVRAM->makeVectorDW(d);
                    _pOPStack->pushDW(handler);
                
                } else if (g == 0B1000) { // dst: QW
                    handler = _pVRAM->makeVectorQW(d);
                    _pOPStack->pushDW(handler);
                
                } else if (g == 0B1011) { // dst: FLT
                    handler = _pVRAM->makeVectorFLT(d);
                    _pOPStack->pushDW(handler);
                
                } else if (g == 0B1111) { // dst: DBL
                    handler = _pVRAM->makeVectorDBL(d);
                    _pOPStack->pushDW(handler);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
            
                break;
            }
        
            case 0x40: { // IPUSH
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    int8_t b;
                    for (uint32_t j = 0; j < sizeof(b); ++j)
                        reinterpret_cast<char *>(&b)[j] = oprom[pc++];
                    _pOPStack->pushB(b);
                
                } else if (g == 0B0010) { // dst: W
                    int16_t w;
                    for (uint32_t j = 0; j < sizeof(w); ++j)
                        reinterpret_cast<char *>(&w)[j] = oprom[pc++];
                    _pOPStack->pushW(w);
                
                } else if (g == 0B0100) { // dst: DW
                    int32_t dw;
                    for (uint32_t j = 0; j < sizeof(dw); ++j)
                        reinterpret_cast<char *>(&dw)[j] = oprom[pc++];
                    _pOPStack->pushDW(dw);
                
                } else if (g == 0B1000) { // dst: QW
                    int64_t qw;
                    for (uint32_t j = 0; j < sizeof(qw); ++j)
                        reinterpret_cast<char *>(&qw)[j] = oprom[pc++];
                    _pOPStack->pushQW(qw);
                
                } else if (g == 0B1011) { // dst: FLT
                    float f;
                    for (uint32_t j = 0; j < sizeof(f); ++j)
                        reinterpret_cast<char *>(&f)[j] = oprom[pc++];
                    _pOPStack->pushFLT(f);
                
                } else if (g == 0B1111) { // dst: DBL
                    double d;
                    for (uint32_t j = 0; j < sizeof(d); ++j)
                        reinterpret_cast<char *>(&d)[j] = oprom[pc++];
                    _pOPStack->pushDBL(d);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
            
                break;
            }
        
            case 0x41: { // DUP
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                if (g == 0B0001) { // dst: B
                    _pOPStack->pushB(_pOPStack->topB());
                
                } else if (g == 0B0010) { // dst: W
                    _pOPStack->pushW(_pOPStack->topW());
                
                } else if (g == 0B0100) { // dst: DW
                    _pOPStack->pushDW(_pOPStack->topDW());
                
                } else if (g == 0B1000) { // dst: QW
                    _pOPStack->pushQW(_pOPStack->topQW());
                
                } else if (g == 0B1011) { // dst: FLT
                    _pOPStack->pushFLT(_pOPStack->topFLT());
                
                } else if (g == 0B1111) { // dst: DBL
                    _pOPStack->pushDBL(_pOPStack->topDBL());
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
            
                break;
            }
        
            case 0x50: { // DEF
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                int32_t addr = *reinterpret_cast<const int32_t *>(&oprom[pc]);
                pc += sizeof(int32_t);
            
                if (g == 0B0001) { // dst: B
                    _pSRAM->setB(addr, 0);
                
                } else if (g == 0B0010) { // dst: W
                    _pSRAM->setW(addr, 0);
                
                } else if (g == 0B0100) { // dst: DW
                    _pSRAM->setDW(addr, 0);
                
                } else if (g == 0B1000) { // dst: QW
                    _pSRAM->setQW(addr, 0);
                
                } else if (g == 0B1011) { // dst: FLT
                    _pSRAM->setFLT(addr, 0);
                
                } else if (g == 0B1111) { // dst: DBL
                    _pSRAM->setDBL(addr, 0);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
            
                break;
            }
        
            case 0x51: { // PUSH
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                int32_t addr = *reinterpret_cast<const int32_t *>(&oprom[pc]);
                pc += sizeof(int32_t);
            
                if (g == 0B0001) { // dst: B
                    auto b = _pSRAM->getB(addr);
                    _pOPStack->pushB(b);
                
                } else if (g == 0B0010) { // dst: W
                    auto w = _pSRAM->getW(addr);
                    _pOPStack->pushW(w);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw = _pSRAM->getDW(addr);
                    _pOPStack->pushDW(dw);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw = _pSRAM->getQW(addr);
                    _pOPStack->pushQW(qw);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f = _pSRAM->getFLT(addr);
                    _pOPStack->pushFLT(f);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d = _pSRAM->getDBL(addr);
                    _pOPStack->pushDBL(d);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
            
                break;
            }
        
            case 0x52: { // POP
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                int32_t addr = *reinterpret_cast<const int32_t *>(&oprom[pc]);
                pc += sizeof(int32_t);
            
                if (g == 0B0001) { // dst: B
                    auto b = _pOPStack->popB();
                    _pSRAM->setB(addr, b);
                
                } else if (g == 0B0010) { // dst: W
                    auto w = _pOPStack->popW();
                    _pSRAM->setW(addr, w);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw = _pOPStack->popDW();
                    _pSRAM->setDW(addr, dw);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw = _pOPStack->popQW();
                    _pSRAM->setQW(addr, qw);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f = _pOPStack->popFLT();
                    _pSRAM->setFLT(addr, f);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d = _pOPStack->popDBL();
                    _pSRAM->setDBL(addr, d);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
            
                break;
            }
        
            case 0x53: { // TOP
                ++pc;
                uint8_t g = oprom[pc];
                g >>= 4;
                ++pc;
            
                int32_t addr = *reinterpret_cast<const int32_t *>(&oprom[pc]);
                pc += sizeof(int32_t);
            
                if (g == 0B0001) { // dst: B
                    auto b = _pOPStack->topB();
                    _pSRAM->setB(addr, b);
                
                } else if (g == 0B0010) { // dst: W
                    auto w = _pOPStack->topW();
                    _pSRAM->setW(addr, w);
                
                } else if (g == 0B0100) { // dst: DW
                    auto dw = _pOPStack->topDW();
                    _pSRAM->setDW(addr, dw);
                
                } else if (g == 0B1000) { // dst: QW
                    auto qw = _pOPStack->topQW();
                    _pSRAM->setQW(addr, qw);
                
                } else if (g == 0B1011) { // dst: FLT
                    auto f = _pOPStack->topFLT();
                    _pSRAM->setFLT(addr, f);
                
                } else if (g == 0B1111) { // dst: DBL
                    auto d = _pOPStack->topDBL();
                    _pSRAM->setDBL(addr, d);
                
                } else {
                    throw VMException(VMET::E_ILLEGAL_GRANULARITY);
                }
            
                break;
            }
        
            case 0x60: { // J
                ++pc;
            
                int32_t addr = *reinterpret_cast<const int32_t *>(&oprom[pc]);
                pc += sizeof(int32_t);
                pc = addr; // 跳转
            
                break;
            }
        
            case 0x61: { // JT
                ++pc;
            
                int32_t addr = *reinterpret_cast<const int32_t *>(&oprom[pc]);
                pc += sizeof(int32_t);
            
                bool b = _pOPStack->popB();
                if (b)
                    pc = addr; // 跳转
            
                break;
            }
        
            case 0x62: { // JF
                ++pc;
            
                int32_t addr = *reinterpret_cast<const int32_t *>(&oprom[pc]);
                pc += sizeof(int32_t);
            
                bool b = _pOPStack->popB();
                if (!b)
                    pc = addr; // 跳转
            
                break;
            }
        
            default: {
                throw VMException(VMET::E_ILLEGAL_INSTRUCTION);
                break;
            }
            
            }
        }
        
    } catch (VMException & ex) {
        std::cerr << "~ 运行时错误: " << ex.getMessage() << std::endl;
    } catch (std::exception & ex) {
        std::cerr << "~ 运行时错误: " << ex.what() << std::endl;
    }
    
    return 0;
}

int32_t FakeCPU::runFuncs(bool verbose, bool step, uint32_t startAddr) {
    int32_t ret = run(verbose, step, false, startAddr);
    return ret;
}

int32_t FakeCPU::runStatic(bool verbose, bool step) {
    int32_t ret = run(verbose, step, true, 0);
    return ret;
}

void FakeCPU::executeVMEF(const std::string & name) {
    if (name == "stderr_nl") {
        std::cerr << _pOPStack->popQW();
    } else if (name == "stderr_ni") {
        std::cerr << _pOPStack->popDW();
    } else if (name == "stderr_ns") {
        std::cerr << _pOPStack->popW();
    } else if (name == "stderr_nb") {
        std::cerr << _pOPStack->popB();
    } else if (name == "stderr_flt") {
        std::cerr << _pOPStack->popFLT();
    } else if (name == "stderr_dbl") {
        std::cerr << _pOPStack->popDBL();
    } else if (name == "stderr_c") {
        std::cerr << char(_pOPStack->popB());
    } else if (name == "stderr_s") {
        std::stringstream ss;
        VectorHandler handler = _pOPStack->popDW();
        uint32_t totalSiz = _pVRAM->getTotalSizeByHandler(handler);
        
        char ch;
        for (uint32_t i = 0;
            i < totalSiz &&
            (ch = *reinterpret_cast<char *>(_pVRAM->getElemHandlerByOffsetB(handler, i))) != 0; ++i) {
            ss << ch;
        }
        std::cerr << ss.str();
    }
    
    else if (name == "stdout_nl") {
        std::cout << _pOPStack->popQW();
    } else if (name == "stdout_ni") {
        std::cout << _pOPStack->popDW();
    } else if (name == "stdout_ns") {
        std::cout << _pOPStack->popW();
    } else if (name == "stdout_nb") {
        std::cout << _pOPStack->popB();
    } else if (name == "stdout_flt") {
        std::cout << _pOPStack->popFLT();
    } else if (name == "stdout_dbl") {
        std::cout << _pOPStack->popDBL();
    } else if (name == "stdout_c") {
        std::cout << char(_pOPStack->popB());
    } else if (name == "stdout_s") {
        std::stringstream ss;
        VectorHandler handler = _pOPStack->popDW();
        uint32_t totalSiz = _pVRAM->getTotalSizeByHandler(handler);
    
        char ch;
        for (uint32_t i = 0; i < totalSiz &&
         (ch = *reinterpret_cast<char *>(_pVRAM->getElemHandlerByOffsetB(handler, i))) != 0; ++i) {
            ss << ch;
        }
        std::cout << ss.str();
    }

    else if (name == "stdin_nl") {
        int64_t qw;
        std::cin >> qw;
        _pOPStack->pushQW(qw);
    } else if (name == "stdin_ni") {
        int32_t dw;
        std::cin >> dw;
        _pOPStack->pushDW(dw);
    } else if (name == "stdin_ns") {
        int16_t w;
        std::cin >> w;
        _pOPStack->pushW(w);
    } else if (name == "stdin_nb") {
        int8_t b;
        std::cin >> b;
        _pOPStack->pushB(b);
    } else if (name == "stdin_flt") {
        float f;
        std::cin >> f;
        _pOPStack->pushFLT(f);
    } else if (name == "stdin_dbl") {
        double d;
        std::cin >> d;
        _pOPStack->pushDBL(d);
    } else if (name == "stdin_c") {
        char ch;
        std::cin >> ch;
        _pOPStack->pushB(ch);
    } else if (name == "stdin_s") {
        std::string s;
        std::cin >> s;
    
        VectorHandler handler = _pVRAM->makeVectorB(1);
        IVector *pVec = _pVRAM->getVectorByHandler(handler);
        RealVectorEntity<int8_t> *pNativeVec =
                static_cast<RealVectorEntity<int8_t> *>(pVec);
        uint32_t i; uint32_t siz = s.size();
        for (i = 0; i < siz; ++i) {
            pNativeVec->set(i, s[i]);
        }
        pNativeVec->set(i, 0);
        _pOPStack->pushDW(handler);
    } else {
        throw VMException(VMET::E_ILLEGAL_VMFE_NAME);
    }
}


