#include <cstring>
#include "FakeCPU.hpp"
#include "VMException.hpp"

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
    
    char voidhole[8] = { 0 };
    while (pc < siz) {
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
            pc = _pFNStack->popRetAddr();
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
        
        }
    }
    
    
}

int32_t FakeCPU::runFuncs(bool verbose, bool step, uint32_t startAddr) {
    run(verbose, step, false, startAddr);
}

int32_t FakeCPU::runStatic(bool verbose, bool step) {
    run(verbose, step, true, 0);
}



