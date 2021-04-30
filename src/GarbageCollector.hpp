#ifndef __GARBAGE_COLLECTOR_HEADER__
#define __GARBAGE_COLLECTOR_HEADER__

#include "FakeVectorRAM.hpp"
#include "FakeScalarRAM.hpp"

class GarbageCollector {
private:
    const FakeVectorRAM & _vram;
    const FakeScalarRAM & _sram;
    
public:
    GarbageCollector(const FakeVectorRAM & vram,
                     const FakeScalarRAM & sram) :
        _vram(vram),
        _sram(sram) {}
        
    void mark() {
    
    
    
    
    
    }
    
};

#endif
