#ifndef __FAKE_VECTOR_RAM_HEADER__
#define __FAKE_VECTOR_RAM_HEADER__

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <mutex>

#include "TSMapWarpper.hpp"

#define MAKE_RESIZ(x) ((x) + 1)

using VectorHandler = uint32_t;
using ElemHandler = void *;

using VectorFakeHandler = uint32_t;

struct IVector {
protected:
    bool _mark = false;
    VectorHandler _handler;
    
public:
    IVector(const VectorHandler & handler) :
        _handler(handler) {}
        
    virtual ~IVector() = default;
    
    const VectorHandler & getHandler();
    
    void setMark(bool mark);
    
    bool getMark() const;
    
    virtual ElemHandler getOffsetT(uint32_t i) const = 0;
    virtual ElemHandler getOffsetB(uint32_t i) const = 0;
    
    virtual uint32_t getElemSize() const = 0;
    virtual uint32_t getTotalSize() const = 0;
    
};

template <typename T>
class RealVectorEntity : public IVector {
private:
    mutable std::vector<T> _vec;
    
public:
    RealVectorEntity(const VectorHandler & handler) :
        IVector(handler) {}
        
    virtual ~RealVectorEntity() = default;
    
    const T & get(uint32_t i) const;
    
    void set(uint32_t i, const T & e);
    
    uint32_t getElemSize() const override;
    uint32_t getTotalSize() const override;
    
    ElemHandler getOffsetT(uint32_t i) const override;
    ElemHandler getOffsetB(uint32_t i) const override;
};

/*
template <typename T>
class FakeVectorEntity : public IVector {
private:
    mutable T * _pData;
    typename std::vector<T>::size_type _siz;

public:
    FakeVectorEntity(const VectorHandler & handler) :
            IVector(handler) {}
    
    virtual ~FakeVectorEntity() = default;
    
    const T & get(uint32_t i) const;
    
    void set(uint32_t i, const T & e);
    
    uint32_t getElemSize() const override;
    
    VectorHandler getOffsetT(uint32_t i) const override;
    
    VectorEntityType getVectorEntityType() const override;
};
 */

class VectorsManager {
    friend class GarbageCollector;
private:
    TSMapWrapper<VectorHandler, IVector *> _map;
    
public:
    VectorsManager() = default;
    virtual ~VectorsManager();
    
    VectorHandler newVectorVEC();
    VectorHandler newVectorB();
    VectorHandler newVectorW();
    VectorHandler newVectorDW();
    VectorHandler newVectorQW();
    VectorHandler newVectorFLT();
    VectorHandler newVectorDBL();
    
    IVector * getVectorByHandler(const VectorHandler & handler) const;
    
    uint32_t getTotalUse() const;

private:
    VectorHandler applyUniqueHandler() const;
};


class FakeVectorRAM {
    friend class GarbageCollector;
private:
    VectorsManager _vecman;

public:
    VectorHandler makeVectorB(uint32_t degree);
    VectorHandler makeVectorW(uint32_t degree);
    VectorHandler makeVectorDW(uint32_t degree);
    VectorHandler makeVectorQW(uint32_t degree);
    VectorHandler makeVectorFLT(uint32_t degree);
    VectorHandler makeVectorDBL(uint32_t degree);
    
    ElemHandler getElemHandlerByOffsetT(
            const VectorHandler & handler, uint32_t offset) const;
    
    ElemHandler getElemHandlerByOffsetB(
            const VectorHandler & handler, uint32_t offset) const;
    
    uint32_t getTotalSizeByHandler(const VectorHandler & handler) const;
    
    uint32_t getElemSizeByHandler(const VectorHandler & handler) const;
    
    IVector * getVectorByHandler(const VectorHandler & handler) const;
};



// member function templates implementation

template <typename T>
const T & RealVectorEntity<T>::get(uint32_t i) const {
    if (i >= _vec.size()) {
        _vec.resize(MAKE_RESIZ(i), 0);
    }
    return _vec[i];
}

template <typename T>
void RealVectorEntity<T>::set(uint32_t i, const T & e) {
    if (i >= _vec.size()) {
        _vec.resize(MAKE_RESIZ(i), 0);
    }
    _vec[i] = e;
}

template <typename T>
uint32_t RealVectorEntity<T>::getElemSize() const {
    return sizeof(T);
}

template <typename T>
uint32_t RealVectorEntity<T>::getTotalSize() const {
    return _vec.size() * sizeof(T);
}

template <typename T>
ElemHandler RealVectorEntity<T>::getOffsetT(uint32_t i) const {
    if (i >= _vec.size()) {
        _vec.resize(MAKE_RESIZ(i), 0);
    }
    return &_vec[i];
}

template <typename T>
ElemHandler RealVectorEntity<T>::getOffsetB(uint32_t i) const {
    if (i >= _vec.size()) {
        _vec.resize(MAKE_RESIZ(i), 0);
    }
    return &(((char *)_vec.data())[i]);
}

#endif
