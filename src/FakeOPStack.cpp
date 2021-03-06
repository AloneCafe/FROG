#include "FakeOPStack.hpp"
#include "VMException.hpp"

int8_t FakeOPStack::popB() {
    if (_opStack.size() < sizeof(int8_t)) {
        throw VMException(VMET::E_OPSTACK_ACCESS_OVERFLOW);
    }
    int8_t e = 0;
    e = _opStack.back();
    _opStack.pop_back();
    return e;
}

int16_t FakeOPStack::popW() {
    if (_opStack.size() < sizeof(int16_t)) {
        throw VMException(VMET::E_OPSTACK_ACCESS_OVERFLOW);
    }
    uint32_t pos = _opStack.size() - sizeof(int16_t);
    int16_t e = *reinterpret_cast<int16_t *>(&_opStack[pos]);
    for (uint32_t i = 0; i < sizeof(int16_t); ++i) {
        _opStack.pop_back();
    }
    return e;
}

int32_t FakeOPStack::popDW() {
    if (_opStack.size() < sizeof(int32_t)) {
        throw VMException(VMET::E_OPSTACK_ACCESS_OVERFLOW);
    }
    uint32_t pos = _opStack.size() - sizeof(int32_t);
    int32_t e = *reinterpret_cast<int32_t *>(&_opStack[pos]);
    for (uint32_t i = 0; i < sizeof(int32_t); ++i) {
        _opStack.pop_back();
    }
    return e;
}

int64_t FakeOPStack::popQW() {
    if (_opStack.size() < sizeof(int64_t)) {
        throw VMException(VMET::E_OPSTACK_ACCESS_OVERFLOW);
    }
    uint32_t pos = _opStack.size() - sizeof(int64_t);
    int64_t e = *reinterpret_cast<int64_t *>(&_opStack[pos]);
    for (uint32_t i = 0; i < sizeof(int64_t); ++i) {
        _opStack.pop_back();
    }
    return e;
}

float FakeOPStack::popFLT() {
    if (_opStack.size() < sizeof(float)) {
        throw VMException(VMET::E_OPSTACK_ACCESS_OVERFLOW);
    }
    uint32_t pos = _opStack.size() - sizeof(float);
    float e = *reinterpret_cast<float *>(&_opStack[pos]);
    for (uint32_t i = 0; i < sizeof(float); ++i) {
        _opStack.pop_back();
    }
    return e;
}

double FakeOPStack::popDBL() {
    if (_opStack.size() < sizeof(double)) {
        throw VMException(VMET::E_OPSTACK_ACCESS_OVERFLOW);
    }
    uint32_t pos = _opStack.size() - sizeof(double);
    double e = *reinterpret_cast<double *>(&_opStack[pos]);
    for (uint32_t i = 0; i < sizeof(double); ++i) {
        _opStack.pop_back();
    }
    return e;
}


int8_t FakeOPStack::topB() {
    if (_opStack.size() < sizeof(int8_t)) {
        throw VMException(VMET::E_OPSTACK_ACCESS_OVERFLOW);
    }
    auto e = popB();
    pushB(e);
    return e;
}

int16_t FakeOPStack::topW() {
    if (_opStack.size() < sizeof(int16_t)) {
        throw VMException(VMET::E_OPSTACK_ACCESS_OVERFLOW);
    }
    auto e = popW();
    pushW(e);
    return e;
}

int32_t FakeOPStack::topDW() {
    if (_opStack.size() < sizeof(int32_t)) {
        throw VMException(VMET::E_OPSTACK_ACCESS_OVERFLOW);
    }
    auto e = popDW();
    pushDW(e);
    return e;
}

int64_t FakeOPStack::topQW() {
    if (_opStack.size() < sizeof(int64_t)) {
        throw VMException(VMET::E_OPSTACK_ACCESS_OVERFLOW);
    }
    auto e = popQW();
    pushQW(e);
    return e;
}

float FakeOPStack::topFLT() {
    if (_opStack.size() < sizeof(float)) {
        throw VMException(VMET::E_OPSTACK_ACCESS_OVERFLOW);
    }
    auto e = popFLT();
    pushFLT(e);
    return e;
}

double FakeOPStack::topDBL() {
    if (_opStack.size() < sizeof(double)) {
        throw VMException(VMET::E_OPSTACK_ACCESS_OVERFLOW);
    }
    auto e = popDBL();
    pushDBL(e);
    return e;
}




void FakeOPStack::pushB(int8_t e) {
    _opStack.push_back(e);
}

void FakeOPStack::pushW(int16_t e) {
    for (uint32_t i = 0; i < sizeof(int16_t); ++i) {
        _opStack.push_back(reinterpret_cast<char *>(&e)[i]);
    }
}

void FakeOPStack::pushDW(int32_t e) {
    for (uint32_t i = 0; i < sizeof(int32_t); ++i) {
        _opStack.push_back(reinterpret_cast<char *>(&e)[i]);
    }
}

void FakeOPStack::pushQW(int64_t e) {
    for (uint32_t i = 0; i < sizeof(int64_t); ++i) {
        _opStack.push_back(reinterpret_cast<char *>(&e)[i]);
    }
}

void FakeOPStack::pushFLT(float e) {
    for (uint32_t i = 0; i < sizeof(float); ++i) {
        _opStack.push_back(reinterpret_cast<char *>(&e)[i]);
    }
}

void FakeOPStack::pushDBL(double e) {
    for (uint32_t i = 0; i < sizeof(double); ++i) {
        _opStack.push_back(reinterpret_cast<char *>(&e)[i]);
    }
}




