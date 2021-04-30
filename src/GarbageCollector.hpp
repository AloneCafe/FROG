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
    
    void mark_OPSTACK();
    
    void mark_SRAM();
    
    void sweep();
};

#endif
