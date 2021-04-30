#ifndef __FAKE_VECTOR_RAM_HEADER__
#define __FAKE_VECTOR_RAM_HEADER__

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <cstdlib>

using VectorHandler = uint32_t;
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
    
    virtual VectorHandler toOffset(uint32_t i) const = 0;
    
    virtual uint32_t getElemSize() const = 0;
    
};

template <typename T>
class VectorEntity : public IVector {
private:
    mutable std::vector<T> _vec;
    
public:
    VectorEntity(const VectorHandler & handler) :
        IVector(handler) {}
        
    virtual ~VectorEntity() = default;
    
    const T & get(uint32_t i) const;
    
    void set(uint32_t i, const T & e);
    
    uint32_t getElemSize() const override;
    
    VectorHandler toOffset(uint32_t i) const override;
};


class VectorsManager {
private:
    std::unordered_map<VectorHandler, IVector *> _map;
    
public:
    VectorsManager() = default;
    
    VectorHandler newVectorVEC();
    VectorHandler newVectorB();
    VectorHandler newVectorW();
    VectorHandler newVectorDW();
    VectorHandler newVectorQW();
    VectorHandler newVectorFLT();
    VectorHandler newVectorDBL();
    
    IVector * getVectorByHandler(const VectorHandler & handler) const;

private:
    VectorHandler applyUniqueHandler() const;
};

class FakeVectorRAM {
private:
    VectorsManager _vecman;

public:
    VectorHandler makeVectorB(uint32_t degree);
    VectorHandler makeVectorW(uint32_t degree);
    VectorHandler makeVectorDW(uint32_t degree);
    VectorHandler makeVectorQW(uint32_t degree);
    VectorHandler makeVectorFLT(uint32_t degree);
    VectorHandler makeVectorDBL(uint32_t degree);
    
    VectorHandler getOffset(const VectorHandler & handler);
    
};



// member function templates implementation

template <typename T>
const T & VectorEntity<T>::get(uint32_t i) const {
    if (i >= _vec.size()) {
        _vec.resize(i + 1, 0);
    }
    return _vec[i];
}

template <typename T>
void VectorEntity<T>::set(uint32_t i, const T & e) {
    if (i >= _vec.size()) {
        _vec.resize(i + 1, 0);
    }
    _vec[i] = e;
}

template <typename T>
uint32_t VectorEntity<T>::getElemSize() const {
    return sizeof(T);
}

template <typename T>
VectorHandler VectorEntity<T>::toOffset(uint32_t i) const {

}

#endif
