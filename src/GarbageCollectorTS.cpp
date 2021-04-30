#include "GarbageCollectorTS.hpp"


void GarbageCollectorTS::mark_OPSTACK() {
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

void GarbageCollectorTS::mark_SRAM() {
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

void GarbageCollectorTS::sweep() {
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

