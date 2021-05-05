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

class VectorsManager;

enum class VectorElemType {
    INVALID,
    VectorHandlerDW,
    B,
    W,
    DW,
    QW,
    FLT,
    DBL
};

enum class VectorLowLvElemType {
    INVALID,
    B,
    W,
    DW,
    QW,
    FLT,
    DBL
};

struct IVector {
protected:
    bool _mark = false;
    VectorHandler _handler;
    
    VectorLowLvElemType _lowLvElemType;
    uint32_t _degree;
    
public:
    IVector(const VectorHandler & handler, const VectorLowLvElemType & lowLvElemType, uint32_t degree) :
        _handler(handler),
        _lowLvElemType(lowLvElemType),
        _degree(degree) {}
        
    virtual ~IVector() = default;
    
    const VectorHandler & getHandler();
    
    void setMark(bool mark);
    
    bool getMark() const;
    
    virtual VectorElemType getElemType() const = 0;
    virtual VectorLowLvElemType getLowLvElemType() const = 0;
    virtual uint32_t getDegree() const = 0;
    
    virtual ElemHandler getOffsetT(uint32_t i) const = 0;
    virtual ElemHandler getOffsetB(uint32_t i) const = 0;
    
    virtual uint32_t getElemSize() const = 0;
    virtual uint32_t getTotalSize() const = 0;
    
};

template <typename T>
class RealVectorEntity : public IVector {
    friend void fillNewVector(const RealVectorEntity<VectorHandler> *pVectorEntity, uint32_t beg, uint32_t end);
private:
    mutable std::vector<T> _vec;
    
    VectorsManager * _pVecMan;
    
public:
    RealVectorEntity(const VectorHandler & handler, VectorsManager * pVecMan, const VectorLowLvElemType &lowLvType, uint32_t degree) :
        IVector(handler, lowLvType, degree),
        _pVecMan(pVecMan) {}
        
    virtual ~RealVectorEntity() = default;
    
    const T & get(uint32_t i) const;
    
    void set(uint32_t i, const T & e);
    
    VectorElemType getElemType() const override;
    VectorLowLvElemType getLowLvElemType() const override;
    uint32_t getDegree() const override;
    
    uint32_t getElemSize() const override;
    uint32_t getTotalSize() const override;
    
    ElemHandler getOffsetT(uint32_t i) const override;
    ElemHandler getOffsetB(uint32_t i) const override;
    
    const std::vector<T> & getVec() const;
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
    
    VectorHandler newVectorVEC(const VectorLowLvElemType & lowLvElemType, uint32_t degree);
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
/*
template <typename T>
const T & RealVectorEntity<T>::get(uint32_t i) const {
    if (i >= _vec.size()) {
        _vec.resize(MAKE_RESIZ(i), 0);
    }
    return _vec[i];
}*/

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

template <typename T>
VectorElemType RealVectorEntity<T>::getElemType() const {
    if (std::is_same<T, int8_t>::value) {
        return VectorElemType::B;
    } else if (std::is_same<T, int16_t>::value) {
        return VectorElemType::W;
    } else if (std::is_same<T, int32_t>::value) {
        return VectorElemType::DW;
    } else if (std::is_same<T, int64_t>::value) {
        return VectorElemType::QW;
    } else if (std::is_same<T, float>::value) {
        return VectorElemType::FLT;
    } else if (std::is_same<T, double>::value) {
        return VectorElemType::DBL;
    } else if (std::is_same<T, VectorHandler>::value) {
        return VectorElemType::VectorHandlerDW;
    } else {
        return VectorElemType::INVALID;
    }
}

template <typename T>
VectorLowLvElemType RealVectorEntity<T>::getLowLvElemType() const {
    return _lowLvElemType;
}

template <typename T>
uint32_t RealVectorEntity<T>::getDegree() const {
    return _degree;
}

template <typename T>
const std::vector<T> & RealVectorEntity<T>::getVec() const {
    return _vec;
}


#endif
