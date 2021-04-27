#ifndef __BYTE_CODE_GENERATOR_HEADER__
#define __BYTE_CODE_GENERATOR_HEADER__

struct ByteCodeHeader {
    uint8_t  _magics[4] = { 'F', 'R', 'O', 'G' };
    uint32_t _sizeScalarRAM;
    uint32_t _sizeVectorRAM;
    
};

class ByteCodeGenerator {
private:
    
    
    static std::vector<char> make(const std::vector<char> & bytesStatic, const std::vector<char> & bytesFuncs) {
    
    }
};

#endif
