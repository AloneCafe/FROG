#ifndef __BYTE_CODE_GENERATOR_HEADER__
#define __BYTE_CODE_GENERATOR_HEADER__

#include <cstdint>
#include <vector>

#pragma pack(push)
#pragma pack(1)
struct ByteCodeHeader {
    uint8_t  _magics[8] = { 'F', 'R', 'O', 'G', 'V', 'M', 0, 0 };
    uint32_t _offsetStatic;
    uint32_t _sizeStatic;
    uint32_t _offsetFuncs;
    uint32_t _sizeFuncs;
};
#pragma pack(pop)

class ByteCodeGenerator {
public:
    static std::vector<char> make(const std::vector<char> & bytesStatic,
                                  const std::vector<char> & bytesFuncs);
};

class ByteCodeHexPrinter {
    static void b2hex(uint8_t b, char hex[2], bool upcase = true);
    
public:
    static void print(const std::vector<char> & bytes, size_t coln = 8);
};

#endif
