#include "UTFSequence.hpp"

UtfChar::UtfChar(const char * const str) {
    char c = str[0];
    if (c != CHARACTER_EOF && c & 0B10000000) {
        // 1zzzzzzz, 各种 UTF-8 字符
        if ((c & 0B11100000) == 0B11000000) {
            _utfCharSeq[0] = c;
            _utfCharSeq[1] = str[1];
            _seqLen = 2;
        } else if ((c & 0B11110000) == 0B11100000) {
            _utfCharSeq[0] = c;
            _utfCharSeq[1] = str[1];
            _utfCharSeq[2] = str[2];
            _seqLen = 3;
        } else if ((c & 0B11111000) == 0B11110000) {
            _utfCharSeq[0] = c;
            _utfCharSeq[1] = str[1];
            _utfCharSeq[2] = str[2];
            _utfCharSeq[3] = str[3];
            _seqLen = 4;
        }
    } else {
        // 0zzzzzzz, 简单 ASCII 字符
        _utfCharSeq[0] = c;
        _seqLen = 1;
    }
}

bool UtfChar::seqcmp(const char seq_a[], const char seq_b[]) {
    for (int i = 0; i < _seqLen; i ++)
        if (seq_a[i] != seq_b[i]) return false;
    return true;
}

UtfChar::UtfChar(const UtfChar & u) {
    memcpy(_utfCharSeq, u._utfCharSeq, u._seqLen);
    _seqLen = u._seqLen;
}

UtfChar UtfChar::parseUtfChar(const char *& str) {
    UtfChar uc(str);
    str += uc.getSeqLen();
    return uc;
}

short UtfChar::getSeqLen() const {
    return _seqLen;
}

bool UtfChar::isAsciChar() const {
    return getSeqLen() == 1;
}

bool UtfChar::isUtfCharSeq() const {
    return getSeqLen() >= 2;
}

char UtfChar::getAsciChar() const {
    return _utfCharSeq[0];
}

const char * UtfChar::getUtfCharSeq() const {
    return _utfCharSeq;
}

void UtfChar::setAsciChar(const char c) {
    _utfCharSeq[0] = c;
}

void UtfChar::setUtfCharSeq(char utfCharSeq[SEQMAXLEN]) {
    for (int i = 0; i < SEQMAXLEN; i ++)
        _utfCharSeq[i] = utfCharSeq[i];
}

bool UtfChar::operator==(const UtfChar & ch) {
    return this->_seqLen == ch._seqLen && seqcmp(this->_utfCharSeq, ch.getUtfCharSeq());
}

bool UtfChar::operator!=(const UtfChar & ch) {
    return this->_seqLen != ch._seqLen || ! seqcmp(this->_utfCharSeq, ch.getUtfCharSeq());
}

bool UtfChar::operator==(const char c) {
    return isAsciChar() && getAsciChar() == c;
}

bool UtfChar::operator!=(const char c) {
    return ! isAsciChar() || getAsciChar() != c;
}

/*UtfChar & UtfChar::operator=(const UtfChar & u) {
    ::memcpy(_utfCharSeq, u._utfCharSeq, u._seqLen);
    _seqLen = u._seqLen;
}*/

UtfString::UtfString(const char * str) {
    _utfCharVec.clear();
    while (*str != '\0') {
        UtfChar uc = UtfChar::parseUtfChar(str);
        _utfCharVec.push_back(uc);
    }
}

UtfString::UtfString(const UtfChar uc) {
    _utfCharVec.clear();
    _utfCharVec.push_back(uc);
}

std::string UtfString::toString() const {
    std::string s;
    for (const UtfChar & uc : _utfCharVec) {
        long len = uc.getSeqLen();
        for (long i = 0; i < len; i ++) {
            const char * const pStr = uc.getUtfCharSeq();
            s.push_back(pStr[i]);
        }
    }
    return s;
}

void UtfString::push(const UtfChar & uc) {
    _utfCharVec.push_back(uc);
}

long UtfString::length() const {
    return _utfCharVec.size();
}

UtfChar UtfString::pop() {
    UtfChar uc = _utfCharVec.back();
    _utfCharVec.pop_back();
    return uc;
}

void UtfString::clear() {
    _utfCharVec.clear();
}

bool UtfString::empty() const {
    return _utfCharVec.empty();
}

UtfChar & UtfString::get(long pos) {
    return _utfCharVec[pos];
}

UtfChar & UtfString::operator[](long pos) {
    return _utfCharVec[pos];
}

UtfString & UtfString::operator+=(const UtfString & utfStr) {
    for (const UtfChar & uc : utfStr._utfCharVec)
        this->push(uc);
    return *this;
}

UtfString UtfString::operator+(const UtfString & utfStr) const {
    UtfString utfStrResult;
    for (const UtfChar & uc : _utfCharVec)
        utfStrResult.push(uc);
    for (const UtfChar & uc : utfStr._utfCharVec)
        utfStrResult.push(uc);
    return utfStrResult;
}

bool UtfString::operator==(const UtfString & utfStr) {
    if (_utfCharVec.size() != utfStr._utfCharVec.size()) {
        return false;
    }
    
    uint32_t sz = _utfCharVec.size();
    for (uint32_t i = 0; i < sz; ++ i) {
        if (_utfCharVec[i] != utfStr._utfCharVec[i]) {
            return false;
        }
    }
    return true;
}

bool UtfString::operator!=(const UtfString & utfStr) {
    return ! operator==(utfStr);
}
