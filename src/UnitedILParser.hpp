#ifndef __UNI_IL_PARSER_HEADER__
#define __UNI_IL_PARSER_HEADER__

#include "ILParser.hpp"
#include "ByteCodeGenerator.hpp"

class UniILParser {
private:
    std::string _filename;
    std::vector<char> _bytesFuncs;
    std::vector<char> _bytesStatic;
    
    bool _runnable = false;
    uint32_t _dwEntryPoint = 0;

public:
    const std::vector<char> & getBytesFuncs() const;
    
    const std::vector<char> & getBytesStatic() const;
    
    bool runnable() const;
    
    uint32_t getEntryPoint() const;

public:
    UniILParser(const std::string & filename)
            : _filename(filename) { }
            
    UniILParser() = default;
    
    ~UniILParser() = default;
    
    bool parse();
    
};

#endif
