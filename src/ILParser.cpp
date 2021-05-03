#include "ILParser.hpp"

std::pair<bool, std::pair<bool, int>>
UniVarAllocTable::getOffset(const std::string & varName) const {
    auto it = _map.cbegin();
    if ((it = _map.find(varName)) != _map.cend()) {
        return std::make_pair(true, it->second);
    }
    return std::make_pair(false, std::make_pair(0, 0));
}

bool UniVarAllocTable::setOffset(const std::string & varName, uint32_t varSiz, bool isStatic) {
    if (isStatic) {
        auto it = _map.insert(std::make_pair(varName, std::make_pair(isStatic, _currStaticOffset)));
        if (it.second)
            _currStaticOffset += varSiz;
        return it.second;
    } else {
        auto it = _map.insert(std::make_pair(varName, std::make_pair(isStatic, _currFuncsOffset)));
        if (it.second)
            _currFuncsOffset += varSiz;
        return it.second;
    }
}

void UniVarAllocTable::resetNonStatic() {
    _currFuncsOffset = 0;
    for (auto it = _map.cbegin(); it != _map.cend(); ) {
        if (!it->second.first) {
            it = _map.erase(it);
        } else {
            ++it;
        }
    }
}

void ILParser::raiseFatalErrAt(const Token & tk, const std::string & msg) const {
    std::cerr << "~ 汇编文件 " << _filename << " (第 " << tk.lineno() + 1 << " 行, 第 " << tk.colno() << " 列), " << msg << std::endl;
}

const std::vector<char> & ILParser::getBytesFuncs() const {
    return _bytesFuncs;
}

const std::vector<char> & ILParser::getBytesStatic() const {
    return _bytesStatic;
}

bool ILParser::parse() {
    const std::vector<Token> tks = _ut.tokenize();
    uint32_t sizTks = tks.size();
    
    enum {
        INITIAL,
        BUILDING_STATIC,
        BUILDING_FUNC,
    } stat = INITIAL;
    
    uint32_t i;
    for (i = 0; i < sizTks; ++i) {
        if (tks[i].isPunc<'.'>()) {
            ++i;
            if (tks[i].isId("FUNC", false)) {
                ++i;
                if (stat == INITIAL) {
                    if (tks[i].isId()) {
                        const std::string & funcName = tks[i].getId();
                        if (funcName == "main") { // 设置虚拟机运行入口点
                            _runnable = true;
                            _dwEntryPoint = _bytesFuncs.size();
                        }
                        _altFuncs.setOffset(funcName, _bytesFuncs.size());
                        ++i;
                        
                        if (tks[i].isPunc<';'>()) {
                            //++i;
                            stat = BUILDING_FUNC;
                            
                        } else if (tks[i].isEnd()) {
                            IL_PARSER_E("意外的文件结尾");
                            
                        } else {
                            IL_PARSER_E("此处可能缺少分号");
                        }
                        
                    } else if (tks[i].isEnd()) {
                        IL_PARSER_E("意外的文件结尾");
                        
                    } else {
                        IL_PARSER_E("非法的语法元素");
                    }
                    
                } else {
                    IL_PARSER_E("非法的区块嵌套");
                }
                
            } else if (tks[i].isId("STATIC", false)) {
                ++i;
                if (stat == INITIAL) {
                    if (tks[i].isPunc<';'>()) {
                        //++i;
                        stat = BUILDING_STATIC;
                        
                    } else if (tks[i].isEnd()) {
                        IL_PARSER_E("意外的文件结尾");
                        
                    } else {
                        IL_PARSER_E("此处可能缺少分号");
                    }
                    
                } else {
                    IL_PARSER_E("非法的区块嵌套");
                }
                
            } else if (tks[i].isId("END", false)) {
                ++i;
                if (stat != BUILDING_STATIC && stat != BUILDING_FUNC)
                    IL_PARSER_E("非法的结束标记");
                
                if (tks[i].isPunc<';'>()) {
                    //++i;
                    stat = INITIAL;
                    _uvat.resetNonStatic(); // reset function variable allocation
                    
                } else if (tks[i].isEnd()) {
                    IL_PARSER_E("意外的文件结尾");
                    
                } else {
                    IL_PARSER_E("此处可能缺少分号");
                }
                
            } else if (tks[i].isEnd()) {
                IL_PARSER_E("意外的文件结尾");
                
            } else {
                IL_PARSER_E("非法的语法元素");
            }
            
        } else if (tks[i].isId()) {
            
            std::vector<char> * pBytes;
            AddrLocateTable * pALT;
            AddrRelocateTable * pART;
            UniVarAllocTable *pUVAT;
            bool isStatic;
            
            if (stat == BUILDING_FUNC) {
                pBytes = &_bytesFuncs;
                pALT = &_altFuncs;
                pART = &_artFuncs;
                pUVAT = &_uvat;
                isStatic = false;
            } else if (stat == BUILDING_STATIC) {
                pBytes = &_bytesStatic;
                pALT = &_altStatic;
                pART = &_artStatic;
                pUVAT = &_uvat;
                isStatic = true;
            } else {
                IL_PARSER_E("非法的语法元素");
            }
            
            unsigned char gh = 0;
            //const std::string & s0 = tks[i].getId();
            if (tks[i].isId("NOP", false)) {
                ++i;
                pBytes->push_back(0x00);
                
            } else if (tks[i].isId("HALT", false)) {
                ++i;
                pBytes->push_back(0x01);
                
            } else if (tks[i].isId("EFCALL", false)) {
                ++i;
                if (tks[i].getType() == TokenType::TOKEN_LITERAL_STRING) {
                    pBytes->push_back(0x02);
                    for (char c : tks[i].getVal()._str)
                        pBytes->push_back(c);
                    pBytes->push_back(0x00);
                    ++i;
                    
                } else {
                    IL_PARSER_E("非法的 EFCALL 参数");
                }
                
                
            } else if (tks[i].isId("CALL", false)) {
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
                    IL_PARSER_E("非法的 EFCALL 参数");
                }
                
            } else if (tks[i].isId("NRET", false)) {
                ++i;
                pBytes->push_back(0x04);
                
            } else if (tks[i].isId("RET", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x05);
                
            } else if (tks[i].isId("RSZ", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else if (tks[i].isId("VOID", false)) {
                    gh = 0B00000000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                if (tks[i].isId("B", false)) {
                    gh |= 0B00000001;
                } else if (tks[i].isId("W", false)) {
                    gh |= 0B00000010;
                } else if (tks[i].isId("DW", false)) {
                    gh |= 0B00000100;
                } else if (tks[i].isId("QW", false)) {
                    gh |= 0B00001000;
                } else if (tks[i].isId("FLT", false)) {
                    gh |= 0B00001011;
                } else if (tks[i].isId("DBL", false)) {
                    gh |= 0B00001111;
                } else if (tks[i].isId("VOID", false)) {
                    gh |= 0B00000000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x06);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("BAND", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x08);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("BOR", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x09);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("BXOR", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x0A);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("BNOT", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x0B);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("SHL", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x0C);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("SHR", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x0D);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("SHRZ", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x0E);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("LT", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x10);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("LE", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x11);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("EQ", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x12);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("NE", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x13);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("GE", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x14);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("GT", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x15);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("LNOT", false)) {
                ++i;
                pBytes->push_back(0x18);
                
            } else if (tks[i].isId("LOR", false)) {
                ++i;
                pBytes->push_back(0x19);
                
            } else if (tks[i].isId("LAND", false)) {
                ++i;
                pBytes->push_back(0x1A);
                
            } else if (tks[i].isId("ADD", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x20);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("SUB", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x21);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("MUL", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x22);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("DIV", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x23);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("MOD", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x24);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("NEG", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x25);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("OFFSET", false)) {
                ++i;
                pBytes->push_back(0x30);
                
            } else if (tks[i].isId("HPUSH", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x31);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("HPOP", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x32);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("LEN", false)) {
                ++i;
                pBytes->push_back(0x33);
                
            } else if (tks[i].isId("MKVEC", false)) {
                ++i;
                
                if (tks[i].getType() == TOKEN_LITERAL_INT) {
                    int d = tks[i].getVal().u._d;
                    if (d > 0B00001111) {
                        IL_PARSER_E("矢量维度过大");
                    } else {
                        gh = d;
                    }
                } else {
                    IL_PARSER_E("非法的维度");
                }
                ++i;
                
                if (tks[i].isId("B", false)) {
                    gh |= 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh |= 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh |= 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh |= 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh |= 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh |= 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x34);
                pBytes->push_back(gh);
                
            } else if (tks[i].isId("IPUSH", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                    pBytes->push_back(0x40);
                    pBytes->push_back(gh);
                    ++i;
                    if (tks[i].getType() == TokenType::TOKEN_LITERAL_INT || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_INT
                            || tks[i].getType() == TokenType::TOKEN_LITERAL_LONG || tks[i].getType() == TokenType::TOKEN_LITERAL_UNSIGNED_LONG
                            || tks[i].getType() == TokenType::TOKEN_LITERAL_CHARSEQ) {
                        pBytes->push_back(tks[i].getVal().u._c);
                    } else {
                        IL_PARSER_E("非法的字面量");
                    }
                    
                } else if (tks[i].isId("W", false)) {
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
                        IL_PARSER_E("非法的字面量");
                    }
                    
                } else if (tks[i].isId("DW", false)) {
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
                        IL_PARSER_E("非法的字面量");
                    }
                    
                } else if (tks[i].isId("QW", false)) {
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
                        IL_PARSER_E("非法的字面量");
                    }
                    
                } else if (tks[i].isId("FLT", false)) {
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
                        IL_PARSER_E("非法的字面量");
                    }
                    
                    pBytes->push_back(((char *)&f)[0]);
                    pBytes->push_back(((char *)&f)[1]);
                    pBytes->push_back(((char *)&f)[2]);
                    pBytes->push_back(((char *)&f)[3]);
                    
                } else if (tks[i].isId("DBL", false)) {
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
                        IL_PARSER_E("非法的字面量");
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
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
            } else if (tks[i].isId("DEF", false)) {
                ++i;
                int siz = 0;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                    siz = 1;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                    siz = 2;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                    siz = 4;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                    siz = 8;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                    siz = 4;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                    siz = 8;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x50);
                pBytes->push_back(gh);
                
                if (tks[i].isId()) {
                    if (!pUVAT->setOffset(tks[i].getId(), siz, isStatic)) {
                        IL_PARSER_E("DEF 指令对该标识符重定义");
                    }
                    auto pair = pUVAT->getOffset(tks[i].getId());
                    assert(pair.first);
                    uint32_t offset = pair.second.second;
                    pBytes->push_back(((char *)&offset)[0]);
                    pBytes->push_back(((char *)&offset)[1]);
                    pBytes->push_back(((char *)&offset)[2]);
                    pBytes->push_back(((char *)&offset)[3]);
                    ++i;
                    
                } else {
                    IL_PARSER_E("非法的 DEF 变量名");
                }
                
            } else if (tks[i].isId("PUSH", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x51);
                pBytes->push_back(gh);
                
                if (tks[i].isId()) {
                    auto pair = pUVAT->getOffset(tks[i].getId());
                    if (!pair.first) {
                        IL_PARSER_E("该变量未用 DEF 指令定义");
                    }
                    uint32_t offset = pair.second.second;
                    pBytes->push_back(((char *)&offset)[0]);
                    pBytes->push_back(((char *)&offset)[1]);
                    pBytes->push_back(((char *)&offset)[2]);
                    pBytes->push_back(((char *)&offset)[3]);
                    ++i;
                    
                } else {
                    IL_PARSER_E("非法的 DEF 变量名");
                }
                
            } else if (tks[i].isId("POP", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x52);
                pBytes->push_back(gh);
                
                if (tks[i].isId()) {
                    auto pair = pUVAT->getOffset(tks[i].getId());
                    if (!pair.first) {
                        IL_PARSER_E("该变量未用 DEF 指令定义");
                    }
                    uint32_t offset = pair.second.second;
                    pBytes->push_back(((char *)&offset)[0]);
                    pBytes->push_back(((char *)&offset)[1]);
                    pBytes->push_back(((char *)&offset)[2]);
                    pBytes->push_back(((char *)&offset)[3]);
                    ++i;
                    
                } else {
                    IL_PARSER_E("非法的 DEF 变量名");
                }
                
            } else if (tks[i].isId("TOP", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
                
                pBytes->push_back(0x53);
                pBytes->push_back(gh);
                
                if (tks[i].isId()) {
                    auto pair = pUVAT->getOffset(tks[i].getId());
                    if (!pair.first) {
                        IL_PARSER_E("该变量未用 DEF 指令定义");
                    }
                    uint32_t offset = pair.second.second;
                    pBytes->push_back(((char *)&offset)[0]);
                    pBytes->push_back(((char *)&offset)[1]);
                    pBytes->push_back(((char *)&offset)[2]);
                    pBytes->push_back(((char *)&offset)[3]);
                    ++i;
                    
                } else {
                    IL_PARSER_E("非法的 DEF 变量名");
                }
                
            } else if (tks[i].isId("J", false)) {
                ++i;
                if (tks[i].isPunc<'#'>()) {
                    uint32_t offset;
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
                        IL_PARSER_E("非法的标签名");
                    }
                } else {
                    IL_PARSER_E("非法的标签标记");
                }
                
            } else if (tks[i].isId("JT", false)) {
                ++i;
                if (tks[i].isPunc<'#'>()) {
                    uint32_t offset;
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
                        IL_PARSER_E("非法的标签名");
                    }
                } else {
                    IL_PARSER_E("非法的标签标记");
                }
                
            } else if (tks[i].isId("JF", false)) {
                ++i;
                if (tks[i].isPunc<'#'>()) {
                    uint32_t offset;
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
                        IL_PARSER_E("非法的标签名");
                    }
                } else {
                    IL_PARSER_E("非法的标签标记");
                }
                
            } else if (tks[i].isId("DUP", false)) {
                ++i;
                if (tks[i].isId("B", false)) {
                    gh = 0B00010000;
                } else if (tks[i].isId("W", false)) {
                    gh = 0B00100000;
                } else if (tks[i].isId("DW", false)) {
                    gh = 0B01000000;
                } else if (tks[i].isId("QW", false)) {
                    gh = 0B10000000;
                } else if (tks[i].isId("FLT", false)) {
                    gh = 0B10110000;
                } else if (tks[i].isId("DBL", false)) {
                    gh = 0B11110000;
                } else {
                    IL_PARSER_E("非法的粒度");
                }
                ++i;
    
                pBytes->push_back(0x41);
                pBytes->push_back(gh);
            }
            
            if (tks[i].isPunc<';'>()) {
            
            } else if (tks[i].isEnd()) {
                IL_PARSER_E("意外的文件结尾");
                
            } else {
                IL_PARSER_E("此处可能缺少分号");
            }
            
        } else if (tks[i].isPunc<'#'>()) {
            uint32_t offset;
            ++i;
            if (stat == BUILDING_FUNC) {
                offset = _bytesFuncs.size();
                if (tks[i].isId()) {
                    _altFuncs.setOffset(std::string{"#"} + tks[i].getId(), offset);
                    ++i;
                    if (!tks[i].isPunc<':'>())
                        IL_PARSER_E("缺失标签 ':' 冒号");
                    
                } else {
                    IL_PARSER_E("非法的标签名");
                }
                
            } else if (stat == BUILDING_STATIC) {
                offset = _bytesStatic.size();
                if (tks[i].isId()) {
                    _altStatic.setOffset(std::string{"#"} + tks[i].getId(), offset);
                    ++i;
                    if (!tks[i].isPunc<':'>())
                        IL_PARSER_E("缺失标签 ':' 冒号");
                    
                } else if (tks[i].isEnd()) {
                    IL_PARSER_E("意外的文件结尾");
                    
                } else {
                    IL_PARSER_E("非法的标签名");
                }
                
            } else {
                IL_PARSER_E("非法的语法元素");
            }
            
        } else if (tks[i].isEnd()) {
            break; // 跳出分析循环
            
        } else {
            IL_PARSER_E("非法的语法元素");
        }
    }
    
    if (stat != INITIAL) {
        IL_PARSER_E("意外的结尾且未闭合区块");
    }
    
    // 进行重定位
    if (!ByteCodeRelocator::relocateStatic(_bytesStatic, _altStatic, _artStatic))
        return false;
    if (!ByteCodeRelocator::relocateFuncs(_bytesFuncs, _altFuncs, _artFuncs))
        return false;
    
    return true;
}

bool ILParser::runnable() const {
    return _runnable;
}

uint32_t ILParser::getEntryPoint() const {
    return _dwEntryPoint;
}
