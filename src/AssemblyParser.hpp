#ifndef __ASSEMBLY_PARSER_HEADER__
#define __ASSEMBLY_PARSER_HEADER__

#include <unordered_map>
#include <vector>
#include <cassert>

#include "UnitedTokenizer.hpp"

#define ASMBR_E(x) raiseFatalErrAt(tks[i], (x))

class AddrLocateTable {
protected:
    std::unordered_map<std::string, size_t> _map;
    
public:
    virtual std::pair<bool, size_t> getOffset(const std::string & symbolName) const {
        auto it = _map.cbegin();
        if ((it = _map.find(symbolName)) != _map.cend()) {
            return std::make_pair(true, it->second);
        }
        return std::make_pair(false, 0);
    }
    
    virtual bool setOffset(const std::string & symbolName, size_t offset) {
        auto it = _map.insert(std::make_pair(symbolName, offset));
        return it.second;
    }
};

class AddrRelocateTable {
private:
    std::unordered_multimap<std::string, size_t> _map;

public:
    virtual std::pair<bool, size_t> getOffset(const std::string & symbolName) const {
        auto it = _map.cbegin();
        if ((it = _map.find(symbolName)) != _map.cend()) {
            return std::make_pair(true, it->second);
        }
        return std::make_pair(false, 0);
    }
    
    virtual void setOffset(const std::string & symbolName, size_t offset) {
        auto it = _map.insert(std::make_pair(symbolName, offset));
    }
};

class UniVarAllocTable {
private:
    size_t currOffset = 0;
    std::unordered_map<std::string, size_t> _map;

public:
    std::pair<bool, size_t> getOffset(const std::string & varName) const {
        auto it = _map.cbegin();
        if ((it = _map.find(varName)) != _map.cend()) {
            return std::make_pair(true, it->second);
        }
        return std::make_pair(false, 0);
    }
    
    bool setOffset(const std::string & varName, size_t varSiz) {
        auto it = _map.insert(std::make_pair(varName, currOffset));
        if (it.second)
            currOffset += varSiz;
        return it.second;
    }
    
    void reset() {
        currOffset = 0;
        _map.clear();
    }
};

class AsmParser {
private:
    std::string _filename;
    std::vector<char> _bytesStatic;
    AddrLocateTable _altStatic;
    AddrRelocateTable _artStatic;
    UniVarAllocTable _uvatStatic;
    
    std::vector<char> _bytesFuncs;
    AddrLocateTable _altFuncs;
    AddrRelocateTable _artFuncs;
    UniVarAllocTable _uvatFuncs;
    
    UniTokenizer _ut;
    
    void raiseFatalErrAt(const Token & tk, const std::string & msg) {
        std::cerr << "~ 汇编文件 " << _filename << " (第 " << tk.lineno() << " 行, 第 " << tk.colno() << " 列), " << msg << std::endl;
        exit(1);
    }
    
public:
    AsmParser(const std::string & filename) : _filename(filename), _ut(_filename) {
        const std::vector<Token> tks = _ut.tokenize();
        size_t sizTks = tks.size();
        
        enum {
            INITIAL,
            BUILDING_STATIC,
            BUILDING_FUNC,
        } stat = INITIAL;
        
        for (size_t i = 0; i < sizTks; ++i) {
            if (tks[i].isPunc<'.'>()) {
                ++i;
                if (tks[i].isId("FUNC") || tks[i].isId("func")) {
                    ++i;
                    if (stat == INITIAL) {
                        if (tks[i].isId()) {
                            const std::string & funcName = tks[i].getId();
                            _altFuncs.setOffset(funcName, _bytesFuncs.size());
                            ++i;
    
                            if (tks[i].isPunc<';'>()) {
                                ++i;
                                stat = BUILDING_FUNC;
        
                            } else if (tks[i].isEnd()) {
                                ASMBR_E("意外的文件结尾");
        
                            } else {
                                ASMBR_E("此处可能缺少分号");
                            }
        
                        } else if (tks[i].isEnd()) {
                            ASMBR_E("意外的文件结尾");
        
                        } else {
                            ASMBR_E("非法的语法元素");
                        }
                        
                    } else {
                        ASMBR_E("非法的区块嵌套");
                    }
                
                } else if (tks[i].isId("STATIC") || tks[i].isId("static")) {
                    ++i;
                    if (stat == INITIAL) {
                        if (tks[i].isPunc<';'>()) {
                            ++i;
                            stat = BUILDING_STATIC;
        
                        } else if (tks[i].isEnd()) {
                            ASMBR_E("意外的文件结尾");
        
                        } else {
                            ASMBR_E("此处可能缺少分号");
                        }
                        
                    } else {
                        ASMBR_E("非法的区块嵌套");
                    }
                    
                } else if (tks[i].isId("END") || tks[i].isId("end")) {
                    ++i;
                    if (stat != BUILDING_STATIC && stat != BUILDING_FUNC)
                        ASMBR_E("非法的结束标记");
                    
                    if (tks[i].isPunc<';'>()) {
                        ++i;
                        stat = INITIAL;
                        _uvatFuncs.reset(); // reset function variable allocation
                        
                    } else if (tks[i].isEnd()) {
                        ASMBR_E("意外的文件结尾");
    
                    } else {
                        ASMBR_E("此处可能缺少分号");
                    }
                    
                } else if (tks[i].isEnd()) {
                    ASMBR_E("意外的文件结尾");
                    
                } else {
                    ASMBR_E("非法的语法元素");
                }
                
            } else if (tks[i].isId()) {
                std::vector<char> * pBytes;
                AddrLocateTable * pALT;
                AddrRelocateTable * pART;
                UniVarAllocTable *pUVAT;
                if (stat == BUILDING_FUNC) {
                    pBytes = &_bytesFuncs;
                    pALT = &_altFuncs;
                    pART = &_artFuncs;
                    pUVAT = &_uvatFuncs;
                } else if (stat == BUILDING_STATIC) {
                    pBytes = &_bytesStatic;
                    pALT = &_altStatic;
                    pART = &_artStatic;
                    pUVAT = &_uvatStatic;
                } else {
                    ASMBR_E("非法的语法元素");
                }
                
                unsigned char gh = 0;
                const std::string & s0 = tks[i].getId();
                if (s0 == "NOP") {
                    ++i;
                    pBytes->push_back(0x00);
                    
                } else if (s0 == "HALT") {
                    ++i;
                    pBytes->push_back(0x01);
                    
                } else if (s0 == "EFCALL") {
                    ++i;
                    if (tks[i].getType() == TokenType::TOKEN_LITERAL_STRING) {
                        pBytes->push_back(0x02);
                        for (char c : tks[i].getVal()._str)
                            pBytes->push_back(c);
                        pBytes->push_back(0x00);
                        ++i;
                        
                    } else {
                        ASMBR_E("非法的 EFCALL 参数");
                    }
                    
                    
                } else if (s0 == "CALL") {
                    ++i;
                    if (tks[i].isId()) {
                        pBytes->push_back(0x03);
                        pART->setOffset(tks[i].getId(), pBytes->size());
                        pBytes->push_back(0x00);
                        pBytes->push_back(0x00);
                        pBytes->push_back(0x00);
                        pBytes->push_back(0x00);
                        ++i;
        
                    } else {
                        ASMBR_E("非法的 EFCALL 参数");
                    }
                
                } else if (s0 == "NRET") {
                    ++i;
                    pBytes->push_back(0x04);
                
                } else if (s0 == "RET") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x05);
                
                } else if (s0 == "RSZ") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else if (tks[i].isId("VOID")) {
                        gh = 0B00000000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
                    
                    if (tks[i].isId("B")) {
                        gh |= 0B00000001;
                    } else if (tks[i].isId("W")) {
                        gh |= 0B00000010;
                    } else if (tks[i].isId("DW")) {
                        gh |= 0B00000100;
                    } else if (tks[i].isId("QW")) {
                        gh |= 0B00001000;
                    } else if (tks[i].isId("FLT")) {
                        gh |= 0B00001011;
                    } else if (tks[i].isId("DBL")) {
                        gh |= 0B00001111;
                    } else if (tks[i].isId("VOID")) {
                        gh |= 0B00000000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x06);
                    pBytes->push_back(gh);
    
                } else if (s0 == "BAND") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x08);
                    pBytes->push_back(gh);
    
                } else if (s0 == "BOR") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x09);
                    pBytes->push_back(gh);
    
                } else if (s0 == "BXOR") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x0A);
                    pBytes->push_back(gh);
    
                } else if (s0 == "BNOT") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x0B);
                    pBytes->push_back(gh);
                    
                } else if (s0 == "SHL") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x0C);
                    pBytes->push_back(gh);
                    
                } else if (s0 == "SHR") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x0D);
                    pBytes->push_back(gh);
    
                } else if (s0 == "SHRZ") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x0E);
                    pBytes->push_back(gh);
    
                } else if (s0 == "LT") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x10);
                    pBytes->push_back(gh);
    
                } else if (s0 == "LE") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x11);
                    pBytes->push_back(gh);
    
                } else if (s0 == "EQ") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x12);
                    pBytes->push_back(gh);
    
                } else if (s0 == "NE") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x13);
                    pBytes->push_back(gh);
    
                } else if (s0 == "GE") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x14);
                    pBytes->push_back(gh);
    
                } else if (s0 == "GT") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x15);
                    pBytes->push_back(gh);
    
                } else if (s0 == "LNOT") {
                    ++i;
                    pBytes->push_back(0x18);
    
                } else if (s0 == "LOR") {
                    ++i;
                    pBytes->push_back(0x19);
    
                } else if (s0 == "LAND") {
                    ++i;
                    pBytes->push_back(0x1A);
    
                } else if (s0 == "ADD") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x20);
                    pBytes->push_back(gh);
    
                } else if (s0 == "SUB") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x21);
                    pBytes->push_back(gh);
    
                } else if (s0 == "MUL") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x22);
                    pBytes->push_back(gh);
    
                } else if (s0 == "DIV") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x23);
                    pBytes->push_back(gh);
    
                } else if (s0 == "MOD") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x24);
                    pBytes->push_back(gh);
    
                } else if (s0 == "NEG") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x25);
                    pBytes->push_back(gh);
    
                } else if (s0 == "OFFSET") {
                    ++i;
                    pBytes->push_back(0x30);
    
                } else if (s0 == "HPUSH") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x31);
                    pBytes->push_back(gh);
    
                } else if (s0 == "HPOP") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x32);
                    pBytes->push_back(gh);
    
                } else if (s0 == "LEN") {
                    ++i;
                    pBytes->push_back(0x33);
    
                } else if (s0 == "MKVEC") {
                    ++i;
                    
                    if (tks[i].getType() == TOKEN_LITERAL_INT) {
                        int d = tks[i].getVal().u._d;
                        if (d > 0B00001111) {
                            ASMBR_E("矢量维度过大");
                        } else {
                            gh = d;
                        }
                    } else {
                        ASMBR_E("非法的维度");
                    }
                    ++i;
                    
                    if (tks[i].isId("B")) {
                        gh |= 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh |= 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh |= 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh |= 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh |= 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh |= 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x34);
                    pBytes->push_back(gh);
                    
                } else if (s0 == "IPUSH") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                        pBytes->push_back(0x40);
                        pBytes->push_back(gh);
                        ++i;
                        if (tks[i].getType() == TokenType::TOKEN_LITERAL_INT || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_INT
                        || tks[i].getType() == TokenType::TOKEN_LITERAL_LONG || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_LONG
                        || tks[i].getType() == TokenType::TOKEN_LITERAL_CHARSEQ) {
                            pBytes->push_back(tks[i].getVal().u._c);
                        } else {
                            ASMBR_E("非法的字面量");
                        }
                        
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                        pBytes->push_back(0x40);
                        pBytes->push_back(gh);
                        ++i;
                        if (tks[i].getType() == TokenType::TOKEN_LITERAL_INT || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_INT
                                || tks[i].getType() == TokenType::TOKEN_LITERAL_LONG || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_LONG
                                || tks[i].getType() == TokenType::TOKEN_LITERAL_CHARSEQ) {
                            pBytes->push_back(((char *)&tks[i].getVal().u._s)[0]);
                            pBytes->push_back(((char *)&tks[i].getVal().u._s)[1]);
                        } else {
                            ASMBR_E("非法的字面量");
                        }
                        
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                        pBytes->push_back(0x40);
                        pBytes->push_back(gh);
                        ++i;
                        if (tks[i].getType() == TokenType::TOKEN_LITERAL_INT || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_INT
                                || tks[i].getType() == TokenType::TOKEN_LITERAL_LONG || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_LONG
                                || tks[i].getType() == TokenType::TOKEN_LITERAL_CHARSEQ) {
                            pBytes->push_back(((char *)&tks[i].getVal().u._d)[0]);
                            pBytes->push_back(((char *)&tks[i].getVal().u._d)[1]);
                            pBytes->push_back(((char *)&tks[i].getVal().u._d)[2]);
                            pBytes->push_back(((char *)&tks[i].getVal().u._d)[3]);
                        } else {
                            ASMBR_E("非法的字面量");
                        }
                        
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                        pBytes->push_back(0x40);
                        pBytes->push_back(gh);
                        ++i;
                        if (tks[i].getType() == TokenType::TOKEN_LITERAL_INT || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_INT
                                || tks[i].getType() == TokenType::TOKEN_LITERAL_LONG || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_LONG
                                || tks[i].getType() == TokenType::TOKEN_LITERAL_CHARSEQ) {
                            pBytes->push_back(((char *)&tks[i].getVal().u._l)[0]);
                            pBytes->push_back(((char *)&tks[i].getVal().u._l)[1]);
                            pBytes->push_back(((char *)&tks[i].getVal().u._l)[2]);
                            pBytes->push_back(((char *)&tks[i].getVal().u._l)[3]);
                            pBytes->push_back(((char *)&tks[i].getVal().u._l)[4]);
                            pBytes->push_back(((char *)&tks[i].getVal().u._l)[5]);
                            pBytes->push_back(((char *)&tks[i].getVal().u._l)[6]);
                            pBytes->push_back(((char *)&tks[i].getVal().u._l)[7]);
                        } else {
                            ASMBR_E("非法的字面量");
                        }
                        
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                        pBytes->push_back(0x40);
                        pBytes->push_back(gh);
                        ++i;
                        float f = .0f;
                        if (tks[i].getType() == TokenType::TOKEN_LITERAL_INT || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_INT) {
                            f = static_cast<float>(tks[i].getVal().u._d);
                        } else if (tks[i].getType() == TokenType::TOKEN_LITERAL_LONG || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_LONG) {
                            f = static_cast<float>(tks[i].getVal().u._l);
                        } else if (tks[i].getType() == TokenType::TOKEN_LITERAL_CHARSEQ) {
                            f = static_cast<float>(tks[i].getVal().u._c);
                        } else if (tks[i].getType() == TokenType::TOKEN_LITERAL_LONG_DOUBLE || tks[i].getType() == TokenType::TOKEN_LITERAL_DOUBLE) {
                            f = static_cast<float>(tks[i].getVal().u._lf);
                        } else if (tks[i].getType() == TokenType::TOKEN_LITERAL_FLOAT) {
                            f = static_cast<float>(tks[i].getVal().u._lf);
                        } else {
                            ASMBR_E("非法的字面量");
                        }
    
                        pBytes->push_back(((char *)&f)[0]);
                        pBytes->push_back(((char *)&f)[1]);
                        pBytes->push_back(((char *)&f)[2]);
                        pBytes->push_back(((char *)&f)[3]);
                        
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                        pBytes->push_back(0x40);
                        pBytes->push_back(gh);
                        ++i;
                        double dbl = .0;
                        if (tks[i].getType() == TokenType::TOKEN_LITERAL_INT || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_INT) {
                            dbl = static_cast<double>(tks[i].getVal().u._d);
                        } else if (tks[i].getType() == TokenType::TOKEN_LITERAL_LONG || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_LONG) {
                            dbl = static_cast<double>(tks[i].getVal().u._l);
                        } else if (tks[i].getType() == TokenType::TOKEN_LITERAL_CHARSEQ) {
                            dbl = static_cast<double>(tks[i].getVal().u._c);
                        } else if (tks[i].getType() == TokenType::TOKEN_LITERAL_LONG_DOUBLE || tks[i].getType() == TokenType::TOKEN_LITERAL_DOUBLE) {
                            dbl = static_cast<double>(tks[i].getVal().u._lf);
                        } else if (tks[i].getType() == TokenType::TOKEN_LITERAL_FLOAT) {
                            dbl = static_cast<double>(tks[i].getVal().u._lf);
                        } else {
                            ASMBR_E("非法的字面量");
                        }
    
                        pBytes->push_back(((char *)&dbl)[0]);
                        pBytes->push_back(((char *)&dbl)[1]);
                        pBytes->push_back(((char *)&dbl)[2]);
                        pBytes->push_back(((char *)&dbl)[3]);
                        pBytes->push_back(((char *)&dbl)[4]);
                        pBytes->push_back(((char *)&dbl)[5]);
                        pBytes->push_back(((char *)&dbl)[6]);
                        pBytes->push_back(((char *)&dbl)[7]);
                        
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
                    
                } else if (s0 == "DEF") {
                    ++i;
                    int siz = 0;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                        siz = 1;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                        siz = 2;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                        siz = 4;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                        siz = 8;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                        siz = 4;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                        siz = 8;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x50);
                    pBytes->push_back(gh);
    
                    if (tks[i].isId()) {
                        if (!pUVAT->setOffset(tks[i].getId(), siz)) {
                            ASMBR_E("DEF 指令对该标识符重定义");
                        }
                        auto pair = pUVAT->getOffset(tks[i].getId());
                        assert(pair.first);
                        size_t offset = pair.second;
                        pBytes->push_back(((char *)&offset)[0]);
                        pBytes->push_back(((char *)&offset)[1]);
                        pBytes->push_back(((char *)&offset)[2]);
                        pBytes->push_back(((char *)&offset)[3]);
                        ++i;
        
                    } else {
                        ASMBR_E("非法的 DEF 变量名");
                    }
                    ++i;
                    
                } else if (s0 == "PUSH") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x51);
                    pBytes->push_back(gh);
    
                    if (tks[i].isId()) {
                        auto pair = pUVAT->getOffset(tks[i].getId());
                        if (!pair.first) {
                            ASMBR_E("该变量未用 DEF 指令定义");
                        }
                        size_t offset = pair.second;
                        pBytes->push_back(((char *)&offset)[0]);
                        pBytes->push_back(((char *)&offset)[1]);
                        pBytes->push_back(((char *)&offset)[2]);
                        pBytes->push_back(((char *)&offset)[3]);
                        ++i;
        
                    } else {
                        ASMBR_E("非法的 DEF 变量名");
                    }
                    ++i;
    
                } else if (s0 == "POP") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x52);
                    pBytes->push_back(gh);
    
                    if (tks[i].isId()) {
                        auto pair = pUVAT->getOffset(tks[i].getId());
                        if (!pair.first) {
                            ASMBR_E("该变量未用 DEF 指令定义");
                        }
                        size_t offset = pair.second;
                        pBytes->push_back(((char *)&offset)[0]);
                        pBytes->push_back(((char *)&offset)[1]);
                        pBytes->push_back(((char *)&offset)[2]);
                        pBytes->push_back(((char *)&offset)[3]);
                        ++i;
        
                    } else {
                        ASMBR_E("非法的 DEF 变量名");
                    }
                    ++i;
    
                } else if (s0 == "TOP") {
                    ++i;
                    if (tks[i].isId("B")) {
                        gh = 0B00010000;
                    } else if (tks[i].isId("W")) {
                        gh = 0B00100000;
                    } else if (tks[i].isId("DW")) {
                        gh = 0B01000000;
                    } else if (tks[i].isId("QW")) {
                        gh = 0B10000000;
                    } else if (tks[i].isId("FLT")) {
                        gh = 0B10110000;
                    } else if (tks[i].isId("DBL")) {
                        gh = 0B11110000;
                    } else {
                        ASMBR_E("非法的粒度");
                    }
                    ++i;
    
                    pBytes->push_back(0x51);
                    pBytes->push_back(gh);
    
                    if (tks[i].isId()) {
                        auto pair = pUVAT->getOffset(tks[i].getId());
                        if (!pair.first) {
                            ASMBR_E("该变量未用 DEF 指令定义");
                        }
                        size_t offset = pair.second;
                        pBytes->push_back(((char *)&offset)[0]);
                        pBytes->push_back(((char *)&offset)[1]);
                        pBytes->push_back(((char *)&offset)[2]);
                        pBytes->push_back(((char *)&offset)[3]);
                        ++i;
        
                    } else {
                        ASMBR_E("非法的 DEF 变量名");
                    }
                    ++i;
    
                } else if (s0 == "J") {
                    ++i;
                    if (tks[i].isPunc<'#'>()) {
                        size_t offset;
                        ++i;
                        pBytes->push_back(0x60);
                        if (tks[i].isId()) {
                            pART->setOffset(std::string{"#"} + tks[i].getId(), pBytes->size());
                            pBytes->push_back(0x00);
                            pBytes->push_back(0x00);
                            pBytes->push_back(0x00);
                            pBytes->push_back(0x00);
                            ++i;
            
                        } else {
                            ASMBR_E("非法的标签名");
                        }
                    } else {
                        ASMBR_E("非法的标签标记");
                    }
                } else if (s0 == "JT") {
                    ++i;
                    if (tks[i].isPunc<'#'>()) {
                        size_t offset;
                        ++i;
                        pBytes->push_back(0x61);
                        if (tks[i].isId()) {
                            pART->setOffset(std::string{"#"} + tks[i].getId(), pBytes->size());
                            pBytes->push_back(0x00);
                            pBytes->push_back(0x00);
                            pBytes->push_back(0x00);
                            pBytes->push_back(0x00);
                            ++i;
            
                        } else {
                            ASMBR_E("非法的标签名");
                        }
                    } else {
                        ASMBR_E("非法的标签标记");
                    }
                } else if (s0 == "JF") {
                    ++i;
                    if (tks[i].isPunc<'#'>()) {
                        size_t offset;
                        ++i;
                        pBytes->push_back(0x62);
                        if (tks[i].isId()) {
                            pART->setOffset(std::string{"#"} + tks[i].getId(), pBytes->size());
                            pBytes->push_back(0x00);
                            pBytes->push_back(0x00);
                            pBytes->push_back(0x00);
                            pBytes->push_back(0x00);
                            ++i;
            
                        } else {
                            ASMBR_E("非法的标签名");
                        }
                    } else {
                        ASMBR_E("非法的标签标记");
                    }
                }
    
                if (tks[i].isPunc<';'>()) {
                    ++i;
        
                } else if (tks[i].isEnd()) {
                    ASMBR_E("意外的文件结尾");
        
                } else {
                    ASMBR_E("此处可能缺少分号");
                }
            
            } else if (tks[i].isPunc<'#'>()) {
                size_t offset;
                ++i;
                if (stat == BUILDING_FUNC) {
                    offset = _bytesFuncs.size();
                    if (tks[i].isId()) {
                        _altFuncs.setOffset(std::string{"#"} + tks[i].getId(), offset);
                        ++i;
                        if (!tks[i].isPunc<':'>())
                            ASMBR_E("缺失标签 ':' 冒号");
                        
                    } else {
                        ASMBR_E("非法的标签名");
                    }
                    
                } else if (stat == BUILDING_STATIC) {
                    offset = _bytesStatic.size();
                    if (tks[i].isId()) {
                        _altStatic.setOffset(std::string{"#"} + tks[i].getId(), offset);
                        ++i;
                        if (!tks[i].isPunc<':'>())
                            ASMBR_E("缺失标签 ':' 冒号");
                        
                    } else if (tks[i].isEnd()) {
                        ASMBR_E("意外的文件结尾");
    
                    } else {
                        ASMBR_E("非法的标签名");
                    }
                    
                } else {
                    ASMBR_E("非法的语法元素");
                }
            
            } else if (tks[i].isEnd()) {
                ASMBR_E("意外的文件结尾");
    
            } else {
                ASMBR_E("非法的语法元素");
            }
        }
        
    }
    
    void parse() {
    
    }
};

class UniAsmParser {
private:
    std::vector<std::string> _filenames;
    std::vector<char> _bytes;
    
public:
    UniAsmParser(const std::vector<std::string> & filenames)
            : _filenames(filenames) { }
    UniAsmParser() = default;
    ~UniAsmParser() = default;
    
    std::string parse() {
        std::stringstream ss;
        if (!_filenames.empty()) {
            std::vector<FileStructurePtr> fileStus;
            for (const std::string & filename : _filenames) {
                AsmStaticParser asp(filename);
                FileStructurePtr pFileStu = asp.parse();
                if (!pFileStu)
                    return "";
                fileStus.push_back(pFileStu);
            }
    
            AsmFuncParser afp;
            afp.parse(fileStus);
            if (afp.hasErr()) {
                for (const auto & e : afp.getErrList())
                    std::cerr << e.getMsg() << std::endl;
                return "";
            }
            ss << afp._asmk.getContextRef().str();
            
        } else {
            AsmStaticParser asp;
            FileStructurePtr pFileStu = asp.parse();
            if (!pFileStu)
                return "";
    
            AsmFuncParser afp;
            afp.parse({ pFileStu });
            if (afp.hasErr()) {
                for (const auto & e : afp.getErrList())
                    std::cerr << e.getMsg() << std::endl;
                return "";
            }
            ss << afp._asmk.getContextRef().str();
        }
        
        return ss.str();
    }

};

#endif
