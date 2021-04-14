#ifndef __ASSEMBLY_PARSER_HEADER__
#define __ASSEMBLY_PARSER_HEADER__

#include <unordered_map>
#include <vector>

#include "UnitedTokenizer.hpp"

#define ASMBR_E(x) raiseFatalErrAt(tks[i], (x))

struct AddrRelocEntry {
    std::string _symbolName;
    size_t _codeOffset;
};

class AddrRelocTable {
private:
    std::unordered_map<std::string, size_t> _map;
    
public:
    std::pair<bool, size_t> getOffset(const std::string & symbolName) const {
        auto it = _map.cbegin();
        if ((it = _map.find(symbolName)) != _map.cend()) {
            return std::make_pair(true, it->second);
        }
        return std::make_pair(false, 0);
    }
    
    bool setOffset(const std::string & symbolName, size_t offset) {
        auto it = _map.insert(std::make_pair(symbolName, offset));
        return it.second;
    }
    
};

class AsmStaticParser {

};

class AsmFuncParser {

};

class AsmParser {
private:
    std::string _filename;
    std::vector<char> _bytesStatic;
    AddrRelocTable _artStatic;
    std::vector<char> _bytesFunc;
    AddrRelocTable _artFunc;
    
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
                    // TODO
                
                } else if (tks[i].isId("STATIC") || tks[i].isId("static")) {
                    ++i;
                    if (tks[i].isPunc<';'>()) {
                        ++i;
                        stat = BUILDING_STATIC;
                    } else {
                        ASMBR_E("此处可能缺少分号");
                    }
                    
                } else if (tks[i].isId("END") || tks[i].isId("end")) {
                    ++i;
                    if (stat != BUILDING_STATIC && stat != BUILDING_FUNC)
                        ASMBR_E("非法的结束标记");
                    
                    if (tks[i].isPunc<';'>()) {
                        ++i;
                        stat = INITIAL;
                    } else {
                        ASMBR_E("此处可能缺少分号");
                    }
                }
                
            } else if (tks[i].isId()) {
                if (stat == BUILDING_FUNC) {
                    ++i;
                    // TODO
                    
                } else if (stat == BUILDING_STATIC) {
                    ++i;
                    // TODO
                    
                } else {
                    ASMBR_E("非法的语法元素");
                }
            
            } else if (tks[i].isPunc<'#'>()) {
                size_t offset;
                ++i;
                if (stat == BUILDING_FUNC) {
                    offset = _bytesFunc.size();
                    if (tks[i].isId()) {
                        _artFunc.setOffset(tks[i].getId(), offset);
                        ++i;
                        if (!tks[i].isPunc<':'>())
                            ASMBR_E("缺失标签 ':' 冒号");
                        
                    } else {
                        ASMBR_E("非法的标签名");
                    }
                    
                } else if (stat == BUILDING_STATIC) {
                    offset = _bytesStatic.size();
                    if (tks[i].isId()) {
                        _artStatic.setOffset(tks[i].getId(), offset);
                        ++i;
                        if (!tks[i].isPunc<':'>())
                            ASMBR_E("缺失标签 ':' 冒号");
                        
                    } else {
                        ASMBR_E("非法的标签名");
                    }
                    
                } else {
                    ASMBR_E("非法的语法元素");
                }
            
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
