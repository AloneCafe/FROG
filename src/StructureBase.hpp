#ifndef __STRUCTURE_HEADER__
#define __STRUCTURE_HEADER__

#include <memory>
#include <list>
#include <sstream>

#include "UTFSequence.hpp"

struct VarType {
protected:
    LocatedUtfString _lowLvType;
    int _degree = 0;

public:
    static VarType buildFromStr(const std::string & str);
    
    VarType(const std::string & s);
    
    VarType(const char *p);
    
    VarType() = default;
    VarType(const VarType &) = default;
    VarType & operator=(const VarType &) = default;
    
    bool operator==(const VarType & objType) const;
    
    bool operator==(const std::string & str) const;
    
	bool operator==(const char * str) const;
    
    bool operator!=(const VarType & objType) const;
    
    bool operator!=(const std::string & str) const;
    
    bool operator!=(const char * str) const;
    
    std::string toString() const;
    
    const LocatedUtfString & getLowLvType() const;
    
    LocatedUtfString & getLowLvTypeRef();
    
    int & getDegreeRef();
    
    int getDegree() const;
    
    void setLowLvType(const LocatedUtfString & lowLvType);
    
    void setDegree(int degree);
};

using VarName = LocatedUtfString;

struct FormalArg {
private:
    VarType _type;
    LocatedUtfString _name;

public:
    void setType(const VarType & type);
    
    void setName(const LocatedUtfString & name);
    
    const VarType & getType() const;
    
    const LocatedUtfString & getName() const;
};

using FormalArgList = std::vector<FormalArg>;

#endif 
