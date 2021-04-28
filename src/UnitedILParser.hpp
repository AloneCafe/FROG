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
    const std::vector<char> & getBytesFuncs() const;
    
    const std::vector<char> & getBytesStatic() const;

public:
    UniILParser(const std::vector<std::string> & filenames)
            : _filenames(filenames) { }
            
    UniILParser() = default;
    
    ~UniILParser() = default;
    
    bool parse();
    
};

#endif
