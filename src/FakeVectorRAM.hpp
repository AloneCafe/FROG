#ifndef __FAKE_VECTOR_RAM_HEADER__
#define __FAKE_VECTOR_RAM_HEADER__

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <cstdlib>

using VectorHandler = uint32_t;

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
};

template <typename T>
class VectorEntity : public IVector {
private:
    std::vector<T> _vec;
    
public:
    VectorEntity(const VectorHandler & handler) :
        IVector(handler) {}
        
    virtual ~VectorEntity() = default;
    
    const T & get(size_t i);
    void set(size_t i, const T & e);
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


public:
    VectorHandler makeVectorB(int degree);
    VectorHandler makeVectorW(int degree);
    VectorHandler makeVectorDW(int degree);
    VectorHandler makeVectorQW(int degree);
    VectorHandler makeVectorFLT(int degree);
    VectorHandler makeVectorDBL(int degree);
    
    

};



template <typename T>
const T & VectorEntity<T>::get(size_t i) {
    if (i >= _vec.size()) {
        _vec.resize(i + 1, 0);
    }
    return _vec[i];
}

template <typename T>
void VectorEntity<T>::set(size_t i, const T & e) {
    if (i >= _vec.size()) {
        _vec.resize(i + 1, 0);
    }
    _vec[i] = e;
}



#endif
