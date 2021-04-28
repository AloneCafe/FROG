#include <memory>
#include <list>
#include <sstream>

#include "StructureBase.hpp"

VarType VarType::buildFromStr(const std::string & str) {
    VarType objType;
    std::stringstream ss;
    auto it = str.cbegin();
    while (it != str.cend() && !isdigit(*it)) {
        ss << *it;
        ++it;
    }
    objType._lowLvType = LocatedUtfString::make(ss.str().c_str(), -1, -1);
    
    std::stringstream nums;
    while (it != str.cend()) {
        
        if (isdigit(*it)) {
            ++it;
            nums << *it;
        }
    }
    objType._degree = atoi(nums.str().c_str());
    
    return objType;
}

VarType::VarType(const std::string & s) {
    *this = buildFromStr(s);
}

VarType::VarType(const char * p) {
    std::string s = p;
    *this = buildFromStr(s);
}

bool VarType::operator==(const VarType & objType) const {
    return _lowLvType == objType._lowLvType && _degree == objType._degree;
}

bool VarType::operator==(const std::string & str) const {
    return toString() == str;
}

bool VarType::operator==(const char * str) const {
    return toString() == str;
}

bool VarType::operator!=(const VarType & objType) const {
    return !(_lowLvType == objType._lowLvType && _degree == objType._degree);
}

bool VarType::operator!=(const std::string & str) const {
    return toString() != str;
}

bool VarType::operator!=(const char * str) const {
    return toString() != str;
}

std::string VarType::toString() const {
    std::stringstream ss;
    if (_lowLvType.empty())
        return "";
    ss << _lowLvType.toString();
    if (_degree > 0)
        ss << _degree;
    return ss.str();
}

const LocatedUtfString & VarType::getLowLvType() const {
    return _lowLvType;
}

LocatedUtfString & VarType::getLowLvTypeRef() {
    return _lowLvType;
}

int & VarType::getDegreeRef() {
    return _degree;
}

int VarType::getDegree() const {
    return _degree;
}

void VarType::setLowLvType(const LocatedUtfString & lowLvType) {
    _lowLvType = lowLvType;
}

void VarType::setDegree(int degree) {
    _degree = degree;
}

void FormalArg::setType(const VarType & type) {
    _type = type;
}

void FormalArg::setName(const LocatedUtfString & name) {
    _name = name;
}

const VarType & FormalArg::getType() const {
    return _type;
}

const LocatedUtfString & FormalArg::getName() const {
    return _name;
}
