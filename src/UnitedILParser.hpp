#ifndef __UNI_IL_PARSER_HEADER__
#define __UNI_IL_PARSER_HEADER__

#include "ILParser.hpp"
#include "ByteCodeGenerator.hpp"

class UniILParser {
private:
    std::vector<std::string> _filenames;
    std::vector<char> _bytesFuncs;
    std::vector<char> _bytesStatic;

public:
    const std::vector<char> & getBytesFuncs() const {
        return _bytesFuncs;
    }
    
    const std::vector<char> & getBytesStatic() const {
        return _bytesStatic;
    }

public:
    UniILParser(const std::vector<std::string> & filenames)
            : _filenames(filenames) { }
    UniILParser() = default;
    ~UniILParser() = default;
    
    bool parse() {
        if (!_filenames.empty()) {
            for (const std::string & filename : _filenames) {
                ILParser ap(filename);
                bool result = ap.parse();
                if (!result)
                    return false;
                
                for (auto b : ap.getBytesFuncs())
                    _bytesFuncs.push_back(b);
                for (auto b : ap.getBytesStatic())
                    _bytesStatic.push_back(b);
            }
            
        } else {
            ILParser ap;
            bool result = ap.parse();
            if (!result)
                return false;
            
            for (auto b : ap.getBytesFuncs())
                _bytesFuncs.push_back(b);
            for (auto b : ap.getBytesStatic())
                _bytesStatic.push_back(b);
        }
        
        return true;
    }
    
};

#endif
