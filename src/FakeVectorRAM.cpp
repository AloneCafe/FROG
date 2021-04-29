#include "FakeVectorRAM.hpp"

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

VectorHandler VectorsManager::newVectorVEC() {
    VectorHandler handler = applyUniqueHandler();
    IVector * pVec = new VectorEntity<VectorHandler>(handler);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorHandler VectorsManager::newVectorB() {
    VectorHandler handler = applyUniqueHandler();
    IVector * pVec = new VectorEntity<int8_t>(handler);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorHandler VectorsManager::newVectorW() {
    VectorHandler handler = applyUniqueHandler();
    IVector * pVec = new VectorEntity<int16_t>(handler);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorHandler VectorsManager::newVectorDW() {
    VectorHandler handler = applyUniqueHandler();
    IVector * pVec = new VectorEntity<int32_t>(handler);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorHandler VectorsManager::newVectorQW() {
    VectorHandler handler = applyUniqueHandler();
    IVector * pVec = new VectorEntity<int64_t>(handler);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorHandler VectorsManager::newVectorFLT() {
    VectorHandler handler = applyUniqueHandler();
    IVector * pVec = new VectorEntity<float>(handler);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}

VectorHandler VectorsManager::newVectorDBL() {
    VectorHandler handler = applyUniqueHandler();
    IVector * pVec = new VectorEntity<double>(handler);
    _map.insert(std::make_pair(handler, pVec));
    return handler;
}