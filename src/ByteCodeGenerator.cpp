#include <cstdint>
#include <vector>
#include <iostream>

#include "ByteCodeGenerator.hpp"

std::vector<char> ByteCodeGenerator::make(const std::vector<char> & bytesStatic, const std::vector<char> & bytesFuncs) {
    std::vector<char> bytecodes;
    ByteCodeHeader bch;
    bch._offsetStatic = sizeof(bch);
    bch._sizeStatic = bytesStatic.size();
    bch._offsetFuncs = bch._offsetStatic + bch._sizeStatic;
    bch._sizeFuncs = bytesFuncs.size();
    for (uint32_t i = 0; i < sizeof(bch); ++i)
        bytecodes.push_back(((char *)&bch)[i]);
    for (uint32_t i = 0; i < bch._sizeStatic; ++i)
        bytecodes.push_back(bytesStatic[i]);
    for (uint32_t i = 0; i < bch._sizeFuncs; ++i)
        bytecodes.push_back(bytesFuncs[i]);
    return std::move(bytecodes);
}

void ByteCodeHexPrinter::b2hex(uint8_t b, char hex[2], bool upcase) {
    uint8_t hb[2] = { uint8_t(b / 0x10), uint8_t(b % 0x10) };
    for (uint32_t i = 0; i < 2; ++i) {
        switch (hb[i]) {
        case 0x0: hex[i] = '0'; break;
        case 0x1: hex[i] = '1'; break;
        case 0x2: hex[i] = '2'; break;
        case 0x3: hex[i] = '3'; break;
        case 0x4: hex[i] = '4'; break;
        case 0x5: hex[i] = '5'; break;
        case 0x6: hex[i] = '6'; break;
        case 0x7: hex[i] = '7'; break;
        case 0x8: hex[i] = '8'; break;
        case 0x9: hex[i] = '9'; break;
        case 0xa: hex[i] = upcase ? 'A' : 'a'; break;
        case 0xb: hex[i] = upcase ? 'B' : 'b'; break;
        case 0xc: hex[i] = upcase ? 'C' : 'c'; break;
        case 0xd: hex[i] = upcase ? 'D' : 'd'; break;
        case 0xe: hex[i] = upcase ? 'E' : 'e'; break;
        case 0xf: hex[i] = upcase ? 'F' : 'f'; break;
        }
    }
}

void ByteCodeHexPrinter::print(const std::vector<char> & bytes, uint32_t coln) {
    uint32_t sizeBytes = bytes.size();
    for (uint32_t i = 0; i < sizeBytes; ++i) {
        char h[2];
        b2hex(bytes[i], h);
        std::cout << h << ((i + 1) % 8 == 0 ? "\n" : " ");
    }
}