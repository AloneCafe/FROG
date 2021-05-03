#ifndef __GARBAGE_COLLECTOR_TS_HEADER__
#define __GARBAGE_COLLECTOR_TS_HEADER__

#define GC_DEFAULT_ARG_TIME 1000
#define GC_DEFAULT_ARG_USE  10000000

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
    
    static GCLock _gclocker;
    
public:
    GarbageCollector(const FakeOPStack & opStack,
                     const FakeScalarRAM & sram,
                     FakeVectorRAM & vram) :
        _ops(opStack),
        _sram(sram),
        _vram(vram) {}
        
    
    void mark_OPSTACK();
    
    void mark_SRAM();
    
    void sweep();
    
    static inline std::mutex & getGCLock();
};


class GCScheduler {
private:
    GarbageCollector _gc;
    struct {
        uint32_t ms;
    } _sche;

private:
    GCScheduler(GarbageCollector &gc)
        : _gc(gc) {}
    
    GCScheduler(const FakeOPStack & opStack,
                const FakeScalarRAM & sram,
                FakeVectorRAM & vram);
        
    GCScheduler(const GCScheduler &) = delete;

public:
    // do GC per ms
    void runBlockStaticSchedule(uint32_t ms = GC_DEFAULT_ARG_TIME);
    
};

void doGCScheduler(GCScheduler *pGCS, uint32_t ms);

#endif
