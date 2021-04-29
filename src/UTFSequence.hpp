#ifndef __UTFSEQ_HEADER__
#define __UTFSEQ_HEADER__

#include <vector>
#include <string>
#include <memory.h>

#define SEQMAXLEN (4)
#define CHARACTER_EOF (-1)

class UtfChar {
private:
    char _utfCharSeq[SEQMAXLEN];
    short _seqLen;
    
    bool seqcmp(const char seq_a[], const char seq_b[]);

public:
    UtfChar(const UtfChar & u);
    //UtfChar & operator=(const UtfChar & u);
    
    explicit UtfChar()
            : _utfCharSeq{0}, _seqLen(0) { }
    UtfChar(const char c)
            : _utfCharSeq{c}, _seqLen(1) { }
    UtfChar(const char * const str);
    
    static UtfChar parseUtfChar(const char *& str);
    short getSeqLen() const;
    bool isAsciChar() const;
    bool isUtfCharSeq() const;
    char getAsciChar() const;
    const char * getUtfCharSeq() const;
    void setAsciChar(const char c);
    void setUtfCharSeq(char utfCharSeq[SEQMAXLEN]);
    
    bool operator==(const UtfChar & ch);
    bool operator!=(const UtfChar & ch);
    bool operator==(const char c);
    bool operator!=(const char c);
};

class UtfString {
protected:
    std::vector<UtfChar> _utfCharVec;

public:
    explicit UtfString() = default;
    UtfString(const char * str);
    UtfString(const UtfChar uc);
    
    std::string toString() const;
    void push(const UtfChar & uc);
    long length() const;
    UtfChar pop();
    void clear();
    bool empty() const;
    UtfChar & get(long pos);
    UtfChar & operator[](long pos);
    UtfString & operator+=(const UtfString & utfStr);
    UtfString operator+(const UtfString & utfStr) const;
    bool operator==(const UtfString & utfStr);
    bool operator!=(const UtfString & utfStr);
    //operator std::string() { return toDebugString(); }
};

class LocatedUtfString : public UtfString {
private:
    long _lineno = 0;
    long _colno = 0;
public:
    LocatedUtfString(const char * str, long lineno, long colno)
            : UtfString::UtfString(str) {
        _lineno = lineno;
        _colno = colno;
    }

	LocatedUtfString(const std::string & str, long lineno, long colno)
		: UtfString::UtfString(str.c_str()) {
		_lineno = lineno;
		_colno = colno;
	}
    
    LocatedUtfString() = default;
    
    static LocatedUtfString make(const char * str, long lineno, long colno) {
        LocatedUtfString lus(str, lineno, colno);
        return lus;
    }

	static LocatedUtfString make(const std::string & str, long lineno, long colno) {
		LocatedUtfString lus(str, lineno, colno);
		return lus;
	}
    
    bool operator==(const LocatedUtfString & lus) const {
        if (UtfString::_utfCharVec.size() != lus._utfCharVec.size())
            return false;
        uint32_t sz = UtfString::_utfCharVec.size();
        for (uint32_t i = 0; i < sz; ++ i) {
            if (const_cast<UtfChar &>(UtfString::_utfCharVec[i]) != lus._utfCharVec[i]) {
                return false;
            }
        }
        return true;
    }
    
    bool operator==(const char * s) const {
        return *this == make(s, -1, -1);
    }
    
    bool operator!=(const LocatedUtfString & lus) const { return ! (*this == lus); }
    bool operator!=(const char * s) const {
        return ! (*this == s);
    }
    
    UtfString toUtfString() const {
        auto & old = *static_cast<const UtfString *>(this);
        UtfString s(old);
        return std::move(s);
    }
    long lineno() const { return _lineno; }
    long colno() const { return _colno; }
};

struct LocatedUtfStringHash {
	uint32_t operator()(const LocatedUtfString & lus) const {
		return std::hash<std::string>{}(lus.toString());
	}
};

#endif