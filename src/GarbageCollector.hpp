#ifndef __GARBAGE_COLLECTOR_HEADER__
#define __GARBAGE_COLLECTOR_HEADER__

#include "FakeVectorRAM.hpp"
#include "FakeScalarRAM.hpp"
#include "FakeOPStack.hpp"

class GarbageCollector {
private:
    const FakeOPStack & _ops;
    const FakeScalarRAM & _sram;
    FakeVectorRAM & _vram;
    
public:
    GarbageCollector(const FakeOPStack & opStack,
                     const FakeScalarRAM & sram,
                     FakeVectorRAM & vram) :
        _ops(opStack),
        _vram(vram),
        _sram(sram) {}
        
    /*
    int KMP(const std::vector<char> & mainly, const std::vector<char> & sub) {
        int n = mainly.size(), m = sub.size();
        if (m == 0) {
            return 0;
        }
        std::vector<int> pi(m);
        for (int i = 1, j = 0; i < m; i++) {
            while (j > 0 && sub[i] != sub[j]) {
                j = pi[j - 1];
            }
            if (sub[i] == sub[j]) {
                j++;
            }
            pi[i] = j;
        }
        for (int i = 0, j = 0; i < n; i++) {
            while (j > 0 && mainly[i] != sub[j]) {
                j = pi[j - 1];
            }
            if (mainly[i] == sub[j]) {
                j++;
            }
            if (j == m) {
                return i - m + 1;
            }
        }
        return -1;
    }
    */
    
    void mark_OPSTACK() {
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
    
    void mark_SRAM() {
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
    
    void sweep() {
        VectorsManager & vecman = _vram._vecman;
        auto it = vecman._map.cbegin();
        while (it != vecman._map.cend()) {
            if (!it->second->getMark()) {
                delete it->second;
                vecman._map.erase(it);
            } else {
                ++it;
            }
        }
    }
};

#endif
