#ifndef __LITTLE_ASSIST_HEADER__
#define __LITTLE_ASSIST_HEADER__

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <clocale>
#include <fstream>
#include <climits>
#include <cwchar>
#include <stack>
#include <sstream>

#include "UTFSequence.hpp"

template <class T>
class StateIndicator {
private:
    T _state;
    T _last_state;
public:
    explicit StateIndicator(T state)
            : _state(state), _last_state(static_cast<T>(0)) { }
    void stateTransfer(const T & est_state);
    void stateKeep() const { }
    void stateRollback();
    const T & getLastState() const { return this->_last_state; }
    const T & getState() const { return this->_state; }
    const T & operator()() const { return this->getState(); }
    bool operator==(const T & state) const { return this->_state == state; }
    bool operator==(const StateIndicator<T> & si) const { return si.operator==(this->_state); }
    bool operator!=(const T & state) const { return this->_state != state; }
    bool operator!=(const StateIndicator<T> & si) const { return si.operator!=(this->_state); }
    bool operator>=(const T & state) const { return this->_state >= state; }
    bool operator>=(const StateIndicator<T> & si) const { return this->_state >= si._state; }
    bool operator<=(const T & state) const { return this->_state <= state; }
    bool operator<=(const StateIndicator<T> & si) const { return this->_state <= si._state; }
};

template <typename T>
void StateIndicator<T>::stateTransfer(const T & dest_state) {
    _last_state = _state;
    _state = dest_state;
}

template <typename T>
void StateIndicator<T>::stateRollback() {
    if (this->_last_state == 0)
        throw std::logic_error("已经回滚到了上一个状态");
    _state = _last_state;
    _last_state = static_cast<T>(0);
}

class StringOperator {
public:
    static std::string format(const char * fmt, ...);
    static std::string W2A(const std::wstring & ws);
    static std::wstring A2W(const std::string & as);
};

class SeqConverter {
public:
    explicit SeqConverter() = default;
    
    class DigitSeq {
    public:
        static unsigned long dec2oct(uint64_t dec);
    };
    
    static UtfString toEscape(UtfString & native);
    static UtfString toEscape(UtfChar & native);
    static UtfString toNative(UtfString & escape);
    
    static std::string toEscape(const std::string & native);
    static std::string toEscape(char native);
    static std::string toNative(const std::string & escape);
};

class FileSystem {
public:
    static std::string getCWD();
    static bool fileExist(const std::string & filename);
    static std::string path2FileName(const std::string & filename);
    static bool fileExist(const char * filename);
    static std::string path2FileName(const char * filename);
};

#endif
