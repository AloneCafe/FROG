#ifndef __PRE_ALLOC_HEADER__
#define __PRE_ALLOC_HEADER__

#include <stack>
#include <cstdint>
#include <vector>
#include <tuple>
#include <unordered_map>

#include "StructureBase.hpp"

template <class T>
struct PreAllocEntry {
    T _t1; // 名称
    T _t2; // 全局唯一名称
    
    PreAllocEntry() = default;
    
    PreAllocEntry(const T & t1, const T & t2)
            : _t1(t1), _t2(t2) { }
            
    PreAllocEntry & operator=(const PreAllocEntry & ae) {
        _t1 = ae._t1;
        _t2 = ae._t2;
        return *this;
    }
};

template <class T, class HASH>
class PreAllocator {
private:
    std::vector<std::pair<PreAllocEntry<T>, VarType>> _entries;
    std::unordered_map<T, int32_t, HASH>
            _fastIndexes;
    bool _resetFlag = false;
    
public:
    bool addEntry(const T & t1, const T & t2, const VarType & objType) {
        int32_t curIdx = _entries.size();
        auto insertResult = _fastIndexes.insert(
                std::make_pair(t1, curIdx));
		if (insertResult.second) {
            _entries.emplace_back(std::make_pair(PreAllocEntry<T>{t1, t2}, objType));
            return true;
            
        } else {
            // 必然是在此作用域中，发生作用域对象的重定义
            return false;
        }
    }
    
    std::tuple<bool, PreAllocEntry<T>, VarType> getEntryByName(const T & t) const {
        decltype(_fastIndexes.cbegin()) it;
        if ((it = _fastIndexes.find(t)) != _fastIndexes.cend()) {
            return std::make_tuple(true, _entries[it->second].first, _entries[it->second].second);
            
        } else {
            // 作用域对象未定义的情况
            return std::make_tuple(false, PreAllocEntry<T>{}, VarType{});
        }
    }
    
    auto & getFastIdxTableRef() const {
        return _fastIndexes;
    }
    
    auto & getEntriesRef() const {
        return _entries;
    }
};

#endif
