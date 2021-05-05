#ifndef __GARBAGE_COLLECTOR_TS_HEADER__
#define __GARBAGE_COLLECTOR_TS_HEADER__

#define GC_DEFAULT_ARG_TIME 1000

#include <mutex>

#include "FakeVectorRAM.hpp"
#include "FakeScalarRAM.hpp"
#include "FakeOPStack.hpp"


using GCLock = std::mutex;
using GCLockGuard = std::lock_guard<GCLock>;

class GarbageCollector {
private:
    const FakeOPStack & _ops;
    const FakeScalarRAM & _sram;
    FakeVectorRAM & _vram;
    
    char * _voidhole;
    
public:
    GarbageCollector(const FakeOPStack & opStack,
                     const FakeScalarRAM & sram,
                     FakeVectorRAM & vram,
                     char * voidhole) :
        _ops(opStack),
        _sram(sram),
        _vram(vram),
        _voidhole(voidhole) {}
        
    
    void mark_OPSTACK();
    
    void mark_SRAM();
    
    void mark_VRAM();
    
    void mark_VOIDHOLE();
    
    void sweep();
};


class GCScheduler {
private:
    GarbageCollector _gc;
    
public:
    std::atomic<bool> _flagVMExited;

private:
    struct {
        uint32_t ms;
    } _sche;
    
    static GCLock _gcLock;

public:
    GCScheduler(GarbageCollector &gc)
        : _gc(gc), _flagVMExited(false) {}
    
    GCScheduler(const FakeOPStack & opStack,
                const FakeScalarRAM & sram,
                FakeVectorRAM & vram,
                char * voidhole);
        
    GCScheduler(const GCScheduler &) = delete;
    
    static std::mutex & getGCLock();

public:
    // do GC per ms
    void runBlockStaticSchedule(uint32_t ms = GC_DEFAULT_ARG_TIME);
    
};

void doGCScheduler(GCScheduler *pGCS, uint32_t ms);

#endif
