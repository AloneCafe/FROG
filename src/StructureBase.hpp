#ifndef __STRUCTURE_HEADER__
#define __STRUCTURE_HEADER__

#include <memory>
#include <list>
#include <sstream>

#include "UTFSequence.hpp"

struct VarType {
private:
    LocatedUtfString _lowLvType;
    int _degree = 0;

public:
    static VarType buildFromStr(const std::string & str) {
        VarType objType;
        std::stringstream ss;
        auto it = str.cbegin();
        while (it != str.cend() && *it != '[') {
            ss << *it;
            ++it;
        }
        objType._lowLvType = LocatedUtfString::make(ss.str().c_str(), -1, -1);
        
        while (it != str.cend()) {
            if (*it == '[') {
                ++it;
                if (*it == ']') {
                    ++it;
                    objType._degree += 1;
                }
            }
        }
        
        return objType;
    }
    
    VarType(const std::string & s) {
        *this = buildFromStr(s);
    }
    VarType(const char *p) {
        std::string s = p;
        *this = buildFromStr(s);
    }
    VarType() = default;
    VarType(const VarType &) = default;
    VarType & operator=(const VarType &) = default;
    
    bool operator==(const VarType & objType) const {
        return _lowLvType == objType._lowLvType && _degree == objType._degree;
    }
    bool operator==(const std::string & str) const {
        return toString() == str;
	}
	bool operator==(const char * str) const {
		return toString() == str;
	}
    
    bool operator!=(const VarType & objType) const {
        return !(_lowLvType == objType._lowLvType && _degree == objType._degree);
    }
    bool operator!=(const std::string & str) const {
        return toString() != str;
    }
    bool operator!=(const char * str) const {
        return toString() != str;
    }
    
    std::string toString() const {
        std::stringstream ss;
        if (_lowLvType.empty())
            return "";
        ss << _lowLvType.toString();
        for (int d = 0; d < _degree; ++ d)
            ss << "[]";
        return ss.str();
    }
    
    const LocatedUtfString & getLowLvType() const { return _lowLvType; }
    LocatedUtfString & getLowLvTypeRef() { return _lowLvType; }
    int & getDegreeRef() { return _degree; }
    int getDegree() const { return _degree; }
    void setLowLvType(const LocatedUtfString & lowLvType) { _lowLvType = lowLvType; }
    void setDegree(int degree) { _degree = degree; }
};

using VarName = LocatedUtfString;

struct FormalArg {
private:
    VarType _type;
    LocatedUtfString _name;

public:
    void setType(const VarType & type) { _type = type; }
    void setName(const LocatedUtfString & name) { _name = name; }
    
    auto & getType() const { return _type; }
    auto & getName() const { return _name; }
};

using FormalArgList = std::vector<FormalArg>;

#endif 
