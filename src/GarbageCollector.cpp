#include <thread>

#include "GarbageCollector.hpp"

static void procMark(GarbageCollector * pGC) {
    pGC->mark_OPSTACK();
    pGC->mark_SRAM();
}

static void procSweep(GarbageCollector * pGC) {
    pGC->sweep();
}

static void procMarkSweep(GarbageCollector * pGC, uint32_t ms = 0) {
    procMark(pGC);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    procSweep(pGC);
}

GCScheduler::GCScheduler(
        const FakeOPStack & opStack,
        const FakeScalarRAM & sram,
        FakeVectorRAM & vram) :
        _gc(opStack, sram, vram) {}
        
void GCScheduler::runBlockStaticSchedule(uint32_t ms) {
    _sche.ms = ms;
    while (1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(_sche.ms));
        procMarkSweep(&_gc);
    }
}

void GarbageCollector::mark_OPSTACK() {
    VectorsManager & vecman = _vram._vecman;
    if (_ops._opStack.size() < sizeof(VectorHandler)) {
        return;
    }
    uint32_t end = _ops._opStack.size() - sizeof(VectorHandler);
    for (uint32_t i = 0; i <= end; ++i) {
        VectorHandler probaVH = *reinterpret_cast<const VectorHandler *>(&_ops._opStack[i]);
        IVector *pVec = vecman.getVectorByHandler(probaVH);
        if (!pVec)
            continue;
        pVec->setMark(true);
    }
}

void GarbageCollector::mark_SRAM() {
    VectorsManager & vecman = _vram._vecman;
    if (_sram._funcsSRAM.size() < sizeof(VectorHandler)) {
        return;
    }
    uint32_t end = _sram._funcsSRAM.size() - sizeof(VectorHandler);
    for (uint32_t i = 0; i <= end; ++i) {
        VectorHandler probaVH = *reinterpret_cast<const VectorHandler *>(&_sram._funcsSRAM[i]);
        IVector *pVec = vecman.getVectorByHandler(probaVH);
        if (!pVec)
            continue;
        pVec->setMark(true);
    }
}

void GarbageCollector::sweep() {
    VectorsManager & vecman = _vram._vecman;
    auto it = vecman._map.cbegin();
    while (it != vecman._map.cend()) {
        if (!it->second->getMark()) {
            delete it->second;
            vecman._map.erase(it);
        } else {
            it->second->setMark(false);
            ++it;
        }
    }
}

void doGCScheduler(GCScheduler * pGCS, uint32_t ms) {
    pGCS->runBlockStaticSchedule(ms);
}
