#ifndef __UNI_ASM_PARSER_HEADER__
#define __UNI_ASM_PARSER_HEADER__

#include "AssemblyParser.hpp"
#include "ByteCodeGenerator.hpp"

class UniAsmParser {
private:
    std::vector<std::string> _filenames;
    std::vector<char> _bytesFuncs;
    std::vector<char> _bytesStatic;

public:
    UniAsmParser(const std::vector<std::string> & filenames)
            : _filenames(filenames) { }
    UniAsmParser() = default;
    ~UniAsmParser() = default;
    
    bool parse() {
        if (!_filenames.empty()) {
            for (const std::string & filename : _filenames) {
                AsmParser ap(filename);
                bool result = ap.parse();
                if (!result)
                    return false;
                
                for (auto b : ap.getBytesFuncs()) {
                    _bytesFuncs.push_back(b);
                }
                for (auto b : ap.getBytesStatic()) {
                    _bytesStatic.push_back(b);
                }
            }
            
        } else {
            AsmParser ap;
            bool result = ap.parse();
            if (!result)
                return false;
            
            for (auto b : ap.getBytesFuncs()) {
                _bytesFuncs.push_back(b);
            }
            for (auto b : ap.getBytesStatic()) {
                _bytesStatic.push_back(b);
            }
        }
        
        return true;
    }
    
};

#endif
