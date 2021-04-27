#ifndef __LOCAL_CODE_BINDER_HEADER__
#define __LOCAL_CODE_BINDER_HEADER__

#include <stack>
#include <list>
#include <tuple>

#include "GUIDGenerator.hpp"
#include "PreAllocatedEntry.hpp"


using LocalCodeAllocEntry = PreAllocEntry<LocatedUtfString>;
using LocalCodeAllocator = PreAllocator<LocatedUtfString, LocatedUtfStringHash>;

class LocalCodeBinder {
private:
    std::list<LocalCodeAllocator> _tables;
    std::list<IBreakContinueAnchor *> _bcas;
    static int idCnt;

public:
    LocalCodeAllocator & makeScope(IBreakContinueAnchor * pAnchor = nullptr);
    
    void clearScope();

public:
    const std::string * getNearestBreak() const;

    const std::string * getNearestContinue() const;

    bool bindDef(const LocatedUtfString & objName, const VarType & objType);
    
    // 得到偏移和大小
    auto getDef(const LocatedUtfString & objName) const
    -> std::tuple<bool, LocatedUtfString /*uniqueObjName*/, VarType>;
};

#endif
