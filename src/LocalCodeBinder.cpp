#include <tuple>

#include "GUIDGenerator.hpp"
#include "PreAllocatedEntry.hpp"
#include "LocalCodeBinder.hpp"

int LocalCodeBinder::idCnt = 0;

LocalCodeAllocator & LocalCodeBinder::makeScope(IBreakContinueAnchor * pAnchor) {
    idCnt += 1;
    _tables.emplace_back();
    _bcas.emplace_back(pAnchor);
    return _tables.back();
}

void LocalCodeBinder::clearScope() {
    _tables.pop_back();
    _bcas.pop_back();
}

const std::string * LocalCodeBinder::getNearestBreak() const {
    for (auto it = _bcas.crbegin(); it != _bcas.crend(); ++it) {
        IBreakContinueAnchor * pAnchor = *it;
        if (pAnchor) {
            const std::string * pBreak = pAnchor->getBreak();
            if (pBreak) {
                return pBreak;
            }
        }
    }
    return nullptr;
}

const std::string * LocalCodeBinder::getNearestContinue() const {
    for (auto it = _bcas.crbegin(); it != _bcas.crend(); ++it) {
        IBreakContinueAnchor * pAnchor = *it;
        if (pAnchor) {
            const std::string * pContinue = pAnchor->getContinue();
            if (pContinue) {
                return pContinue;
            }
        }
    }
    return nullptr;
}

bool LocalCodeBinder::bindDef(const LocatedUtfString & objName, const VarType & objType) {
    std::stringstream ss;
    ss << objName.toString();
    ss << "_";
    ss << LocalCodeBinder::idCnt;
    LocatedUtfString uniqueObjName(ss.str().c_str(), objName.lineno(), objName.colno());
    return _tables.back().addEntry(objName, uniqueObjName, objType);
}

// 得到偏移和大小
auto LocalCodeBinder::getDef(const LocatedUtfString & objName) const
-> std::tuple<bool, LocatedUtfString /*uniqueObjName*/, VarType>
{
    std::tuple<bool, LocalCodeAllocEntry, VarType> tuple;
    int32_t curNegOffset = 0;
    for (auto it = _tables.crbegin(); it != _tables.crend();) {
        tuple = it->getEntryByName(objName);
        if (!std::get<0>(tuple)) {
            ++it;
        } else {
            return std::make_tuple(true,
                    std::get<1>(tuple)._t2,
                    std::get<2>(tuple));
        }
    }
    return std::make_tuple(false, std::get<1>(tuple)._t2, std::get<2>(tuple));
}