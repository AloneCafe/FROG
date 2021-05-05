#include <thread>
#include <iostream>

#include "GarbageCollector.hpp"
#include "FakeCPU.hpp"

std::mutex GCScheduler::_gcLock;

static void procMark(GarbageCollector * pGC) {
    pGC->mark_OPSTACK();
    pGC->mark_SRAM();
    pGC->mark_VOIDHOLE();
    pGC->mark_VRAM();
}

static void procSweep(GarbageCollector * pGC) {
    pGC->sweep();
}

static void procMarkSweep(GarbageCollector * pGC) {
    procMark(pGC);
    procSweep(pGC);
}

GCScheduler::GCScheduler(
        const FakeOPStack & opStack,
        const FakeScalarRAM & sram,
        FakeVectorRAM & vram,
        char * voidhole) :
    _gc(opStack, sram, vram, voidhole),
    _flagVMExited(false) {}
        
void GCScheduler::runBlockStaticSchedule(uint32_t ms) {
    _sche.ms = ms;
    while (1) {
        std::this_thread::sleep_for(
                std::chrono::milliseconds(_sche.ms));
        GCLockGuard lck(getGCLock());
        if (_flagVMExited.load())
            return;
        else
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


// 先在其他存储器中试图标记矢量，如果顶层矢量状态为 false，那么就用 false 标记 (即不操作) 其下层所有矢量
// 如果顶层矢量被标为 true，那么就将其下层所有矢量标记为 true
static void markRecur_VEC(
        VectorsManager & vecman,
        RealVectorEntity<VectorHandler> * pNativeVec,
        VectorHandler srcHandler)
{
    uint32_t siz = pNativeVec->getVec().size();
    IVector *pSrcVec = vecman.getVectorByHandler(srcHandler);
    for (uint32_t i = 0; i < siz; ++i) {
        VectorHandler subHandler = pNativeVec->get(i);
        
        IVector *pDstVec = vecman.getVectorByHandler(subHandler);
        if (!pDstVec)
            continue;
    
        pDstVec->setMark(true);
        
        if (pDstVec->getElemType() == VectorElemType::VectorHandlerDW) {
            // 往下递归
            RealVectorEntity<VectorHandler> * pNativeDstVec =
                static_cast<RealVectorEntity<VectorHandler> *>(pDstVec);
            markRecur_VEC(vecman, pNativeDstVec, srcHandler);
        }
    }
}

void GarbageCollector::mark_VRAM() {
    VectorsManager & vecman = _vram._vecman;
    auto it = vecman._map.begin();
    while (it != vecman._map.end()) {
        VectorHandler srcHandler = it->first;
        IVector * pSrcVec = it->second;
    
        auto jt = vecman._map.begin();
        while (jt != vecman._map.end()) {
            IVector * pDstVec = jt->second;
            
            if (pSrcVec->getElemType() == VectorElemType::VectorHandlerDW) {
                RealVectorEntity<VectorHandler> * pNativeDstVec =
                        static_cast<RealVectorEntity<VectorHandler> *>(pDstVec);
                if (pSrcVec->getMark()) {
                    markRecur_VEC(vecman, pNativeDstVec, srcHandler);
                }
            }
            ++jt;
        }
        ++it;
    }
}

void GarbageCollector::mark_VOIDHOLE() {
    VectorsManager & vecman = _vram._vecman;
    VectorHandler probaVH = *reinterpret_cast<const VectorHandler *>(&_voidhole[0]);
    IVector *pVec = vecman.getVectorByHandler(probaVH);
    if (!pVec)
        return;
    pVec->setMark(true);
}

void GarbageCollector::sweep() {
    VectorsManager & vecman = _vram._vecman;
    auto it = vecman._map.begin();
    while (it != vecman._map.end()) {
        if (!it->second->getMark()) {
            //std::cerr << "sweep VECTOR: " << it->first << std::endl;
            delete it->second;
            it = vecman._map.erase(it);
        } else {
            it->second->setMark(false);
            ++it;
        }
    }
}

std::mutex & GCScheduler::getGCLock() {
    return _gcLock;
}

void doGCScheduler(GCScheduler * pGCS, uint32_t ms) {
    pGCS->runBlockStaticSchedule(ms);
}
