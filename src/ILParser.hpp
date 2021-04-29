#ifndef __IL_PARSER_HEADER__
#define __IL_PARSER_HEADER__

#include <unordered_map>
#include <vector>
#include <cassert>

#include "UnitedTokenizer.hpp"
#include "ByteCodeRelocator.hpp"

#define IL_PARSER_E(x) return raiseFatalErrAt(tks[i], (x)), false;


class UniVarAllocTable {
private:
    int32_t _currStaticOffset = INT32_MIN; // 负增长, 负方向存放
    int32_t _currFuncsOffset = 0;  // 正增长, 正方向存放
    std::unordered_map<std::string, std::pair<bool, int>> _map;

public:
    std::pair<bool, std::pair<bool, int>> getOffset(const std::string & varName) const;
    
    bool setOffset(const std::string & varName, uint32_t varSiz, bool isStatic);
    
    void resetNonStatic();
};

class ILParser {
private:
    std::string _filename;
    
    std::vector<char> _bytesStatic;
    AddrLocateTable _altStatic;
    AddrRelocateTable _artStatic;
    
    std::vector<char> _bytesFuncs;
    AddrLocateTable _altFuncs;
    AddrRelocateTable _artFuncs;
    
    UniVarAllocTable _uvat;
    
    UniTokenizer _ut;
    
    void raiseFatalErrAt(const Token & tk, const std::string & msg) const;
    
    
public:
    const std::vector<char> & getBytesFuncs() const;
    
    const std::vector<char> & getBytesStatic() const;
    
public:
    ILParser(const std::string & filename) :
        _filename(filename), _ut(_filename) {}
        
    ILParser() :
        _filename("<stdin>"), _ut() {}
        
    ~ILParser() = default;
    
    bool parse();
};

#endif
