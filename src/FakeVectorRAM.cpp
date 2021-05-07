#include <iostream>
#include "FakeVectorRAM.hpp"
#include "VMException.hpp"


void fillNewVector(const RealVectorEntity<VectorHandler> *pVectorEntity, uint32_t beg, uint32_t end) {
    uint32_t degree = pVectorEntity->getDegree();
    if (degree > 2) { // 填充向量
        for (; beg < end; ++beg)
            pVectorEntity->_vec[beg] =
                    pVectorEntity->_pVecMan->newVectorVEC(
                            pVectorEntity->getLowLvElemType(), degree - 1
                    );
    } else {
        switch (pVectorEntity->getLowLvElemType()) {
        case VectorLowLvElemType::B:
            for (; beg < end; ++beg)
                pVectorEntity->_vec[beg] =
                        pVectorEntity->_pVecMan->newVectorB();
            break;
        
        case VectorLowLvElemType::W:
            for (; beg < end; ++beg)
                pVectorEntity->_vec[beg] =
                        pVectorEntity->_pVecMan->newVectorW();
            break;
        
        case VectorLowLvElemType::DW:
            for (; beg < end; ++beg)
                pVectorEntity->_vec[beg] =
                        pVectorEntity->_pVecMan->newVectorDW();
            break;
        
        case VectorLowLvElemType::QW:
            for (; beg < end; ++beg)
                pVectorEntity->_vec[beg] =
                        pVectorEntity->_pVecMan->newVectorQW();
            break;
        
        case VectorLowLvElemType::FLT:
            for (; beg < end; ++beg)
                pVectorEntity->_vec[beg] =
                        pVectorEntity->_pVecMan->newVectorFLT();
            break;
        
        case VectorLowLvElemType::DBL:
            for (; beg < end; ++beg)
                pVectorEntity->_vec[beg] =
                        pVectorEntity->_pVecMan->newVectorDBL();
            break;
        }
    }
}




// 模板对于类型为矢量句柄的特化，当二维以上的矢量 (即元素为矢量类型) 空间不足时，不仅需要扩充空间，还需要新建向量
template <>
const VectorHandler & RealVectorEntity<VectorHandler>::get(uint32_t i) const {
    if (i >= _vec.size()) {
        uint32_t j = _vec.size();
        _vec.resize(MAKE_RESIZ(i), 0);
        // 填充新向量
        fillNewVector(this, j, _vec.size());
        
        /*
        uint32_t degree = getDegree();
        if (degree > 2) { // 填充向量
            for (; j < _vec.size(); ++j)
                _vec[j] = _pVecMan->newVectorVEC(getLowLvElemType(), degree - 1);
        } else {
            switch (getLowLvElemType()) {
            case VectorLowLvElemType::B:
                for (; j < _vec.size(); ++j)
                    _vec[j] = _pVecMan->newVectorB();
                break;

            case VectorLowLvElemType::W:
                for (; j < _vec.size(); ++j)
                    _vec[j] = _pVecMan->newVectorW();
                break;
                
            case VectorLowLvElemType::DW:
                for (; j < _vec.size(); ++j)
                    _vec[j] = _pVecMan->newVectorDW();
                break;
            
            case VectorLowLvElemType::QW:
                for (; j < _vec.size(); ++j)
                    _vec[j] = _pVecMan->newVectorQW();
                break;
            
            case VectorLowLvElemType::FLT:
                for (; j < _vec.size(); ++j)
                    _vec[j] = _pVecMan->newVectorFLT();
                break;
            
            case VectorLowLvElemType::DBL:
                for (; j < _vec.size(); ++j)
                    _vec[j] = _pVecMan->newVectorDBL();
                break;
            }
        }
         */
    }
    return _vec[i];
}

template <>
void RealVectorEntity<VectorHandler>::set(uint32_t i, const VectorHandler & e) {
    if (i >= _vec.size()) {
        uint32_t j = _vec.size();
        _vec.resize(MAKE_RESIZ(i), 0);
        // 填充新向量
        fillNewVector(this, j, _vec.size());
    }
    _vec[i] = e;
}

template <>
uint32_t RealVectorEntity<VectorHandler>::getElemSize() const {
    return sizeof(VectorHandler);
}

template <>
uint32_t RealVectorEntity<VectorHandler>::getTotalSize() const {
    return _vec.size() * sizeof(VectorHandler);
}

template <>
ElemHandler RealVectorEntity<VectorHandler>::getOffsetT(uint32_t i) const {
    if (i >= _vec.size()) {
        uint32_t j = _vec.size();
        _vec.resize(MAKE_RESIZ(i), 0);
        // 填充新向量
        fillNewVector(this, j, _vec.size());
    }
    return &_vec[i];
}

template <>
ElemHandler RealVectorEntity<VectorHandler>::getOffsetB(uint32_t i) const {
    if (i >= _vec.size()) {
        uint32_t j = _vec.size();
        _vec.resize(MAKE_RESIZ(i), 0);
        // 填充新向量
        fillNewVector(this, j, _vec.size());
    }
    return &(((char *)_vec.data())[i]);
}



const VectorHandler & IVector::getHandler() {
    return _handler;
}

void IVector::setMark(bool mark) {
    _mark = mark;
}

bool IVector::getMark() const {
    return _mark;
}

IVector * VectorsManager::getVectorByHandler(const VectorHandler & handler) const {
    auto pair = _map.find(handler);
    if (pair == _map.cend())
        return nullptr;
    return pair->second;
}

VectorHandler VectorsManager::applyUniqueHandler() const {
    VectorHandler handler;
    for (uint32_t i = 0; i < sizeof(VectorHandler); ++i) {
        reinterpret_cast<char *>(&handler)[i] = rand() % 0x100;
    }
    while (_map.find(handler) != _map.cend())
        handler += 1;
    return handler;
}

VectorHandler VectorsManager::newVectorVEC(const VectorLowLvElemType & lowLvElemType, uint32_t degree) {
    VectorHandler handler = applyUniqueHandler();
    //std::cerr << "apply VECTOR: " << handler << ", elemType: VEC" << std::endl;
    IVector * pVec = new RealVectorEntity<VectorHandler>(handler, this, lowLvElemType, degree);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorHandler VectorsManager::newVectorB() {
    VectorHandler handler = applyUniqueHandler();
    //std::cerr << "apply VECTOR: " << handler << ", elemType: B" << std::endl;
    IVector * pVec = new RealVectorEntity<int8_t>(handler, this, VectorLowLvElemType::B, 1);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorHandler VectorsManager::newVectorW() {
    VectorHandler handler = applyUniqueHandler();
    //std::cerr << "apply VECTOR: " << handler << ", elemType: W" << std::endl;
    IVector * pVec = new RealVectorEntity<int16_t>(handler, this, VectorLowLvElemType::W, 1);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorHandler VectorsManager::newVectorDW() {
    VectorHandler handler = applyUniqueHandler();
    //std::cerr << "apply VECTOR: " << handler << ", elemType: DW" << std::endl;
    IVector * pVec = new RealVectorEntity<int32_t>(handler, this, VectorLowLvElemType::DW, 1);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorHandler VectorsManager::newVectorQW() {
    VectorHandler handler = applyUniqueHandler();
    //std::cerr << "apply VECTOR: " << handler << ", elemType: QW" << std::endl;
    IVector * pVec = new RealVectorEntity<int64_t>(handler, this, VectorLowLvElemType::QW, 1);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorHandler VectorsManager::newVectorFLT() {
    VectorHandler handler = applyUniqueHandler();
    //std::cerr << "apply VECTOR: " << handler << ", elemType: FLT" << std::endl;
    IVector * pVec = new RealVectorEntity<float>(handler, this, VectorLowLvElemType::FLT, 1);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorHandler VectorsManager::newVectorDBL() {
    VectorHandler handler = applyUniqueHandler();
    //std::cerr << "apply VECTOR: " << handler << ", elemType: DBL" << std::endl;
    IVector * pVec = new RealVectorEntity<double>(handler, this, VectorLowLvElemType::DBL, 1);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorsManager::~VectorsManager() {
    for (const auto & e : _map) {
        delete e.second;
    }
    _map.clear();
}

uint32_t VectorsManager::getTotalUsage() const {
    uint32_t total = 0;
    for (const auto & e : _map) {
        total += e.second->getTotalSize();
    }
    return total;
}

uint32_t VectorsManager::getHandlerCount() const {
    return _map._map.size();
}

VectorHandler FakeVectorRAM::makeVectorB(uint32_t degree) {
    VectorHandler handler;
    if (degree == 1) {
        handler = _vecman.newVectorB();
        return handler;
    } else {
        handler = _vecman.newVectorVEC(VectorLowLvElemType::B, degree);
        IVector *pVec = _vecman.getVectorByHandler(handler);
        RealVectorEntity<VectorHandler> *pNativeVec =
                static_cast<RealVectorEntity<VectorHandler> *>(pVec);
        pNativeVec->set(0, makeVectorB(degree - 1));
        return handler;
    }
}

VectorHandler FakeVectorRAM::makeVectorW(uint32_t degree) {
    VectorHandler handler;
    if (degree == 1) {
        handler = _vecman.newVectorW();
        return handler;
    } else {
        handler = _vecman.newVectorVEC(VectorLowLvElemType::W, degree);
        IVector *pVec = _vecman.getVectorByHandler(handler);
        RealVectorEntity<VectorHandler> *pNativeVec =
                static_cast<RealVectorEntity<VectorHandler> *>(pVec);
        pNativeVec->set(0, makeVectorW(degree - 1));
        return handler;
    }
}

VectorHandler FakeVectorRAM::makeVectorDW(uint32_t degree) {
    VectorHandler handler;
    if (degree == 1) {
        handler = _vecman.newVectorDW();
        return handler;
    } else {
        handler = _vecman.newVectorVEC(VectorLowLvElemType::DW, degree);
        IVector *pVec = _vecman.getVectorByHandler(handler);
        RealVectorEntity<VectorHandler> *pNativeVec =
                static_cast<RealVectorEntity<VectorHandler> *>(pVec);
        pNativeVec->set(0, makeVectorDW(degree - 1));
        return handler;
    }
}

VectorHandler FakeVectorRAM::makeVectorQW(uint32_t degree) {
    VectorHandler handler;
    if (degree == 1) {
        handler = _vecman.newVectorQW();
        return handler;
    } else {
        handler = _vecman.newVectorVEC(VectorLowLvElemType::QW, degree);
        IVector *pVec = _vecman.getVectorByHandler(handler);
        RealVectorEntity<VectorHandler> *pNativeVec =
                static_cast<RealVectorEntity<VectorHandler> *>(pVec);
        pNativeVec->set(0, makeVectorQW(degree - 1));
        return handler;
    }
}

VectorHandler FakeVectorRAM::makeVectorFLT(uint32_t degree) {
    VectorHandler handler;
    if (degree == 1) {
        handler = _vecman.newVectorFLT();
        return handler;
    } else {
        handler = _vecman.newVectorVEC(VectorLowLvElemType::FLT, degree);
        IVector *pVec = _vecman.getVectorByHandler(handler);
        RealVectorEntity<VectorHandler> *pNativeVec =
                static_cast<RealVectorEntity<VectorHandler> *>(pVec);
        pNativeVec->set(0, makeVectorFLT(degree - 1));
        return handler;
    }
}

VectorHandler FakeVectorRAM::makeVectorDBL(uint32_t degree) {
    VectorHandler handler;
    if (degree == 1) {
        handler = _vecman.newVectorDBL();
        return handler;
    } else {
        handler = _vecman.newVectorVEC(VectorLowLvElemType::DBL, degree);
        IVector *pVec = _vecman.getVectorByHandler(handler);
        RealVectorEntity<VectorHandler> *pNativeVec =
                static_cast<RealVectorEntity<VectorHandler> *>(pVec);
        pNativeVec->set(0, makeVectorDBL(degree - 1));
        return handler;
    }
}

ElemHandler FakeVectorRAM::getElemHandlerByOffsetT(
        const VectorHandler & handler,
        uint32_t offset) const
{
    IVector *pVec = _vecman.getVectorByHandler(handler);
    if (!pVec)
        throw VMException(VMET::E_ILLEGAL_VECTOR_HANDLER);
    return pVec->getOffsetT(offset);
}

uint32_t FakeVectorRAM::getTotalSizeByHandler(
        const VectorHandler & handler) const
{
    IVector *pVec = _vecman.getVectorByHandler(handler);
    if (!pVec)
        throw VMException(VMET::E_ILLEGAL_VECTOR_HANDLER);
    return pVec->getTotalSize();
}

uint32_t FakeVectorRAM::getElemSizeByHandler(
        const VectorHandler & handler) const
{
    IVector *pVec = _vecman.getVectorByHandler(handler);
    if (!pVec)
        throw VMException(VMET::E_ILLEGAL_VECTOR_HANDLER);
    return pVec->getElemSize();
}

ElemHandler FakeVectorRAM::getElemHandlerByOffsetB(const VectorHandler & handler, uint32_t offset) const {
    IVector *pVec = _vecman.getVectorByHandler(handler);
    if (!pVec)
        throw VMException(VMET::E_ILLEGAL_VECTOR_HANDLER);
    return pVec->getOffsetB(offset);
}

IVector * FakeVectorRAM::getVectorByHandler(const VectorHandler & handler) const {
    return _vecman.getVectorByHandler(handler);
}

uint32_t FakeVectorRAM::getHandlerCount() const {
    return _vecman.getHandlerCount();
}

uint32_t FakeVectorRAM::getTotalUsage() const {
    return _vecman.getTotalUsage();
}
