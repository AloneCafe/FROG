#include <vector>
#include <string>
#include <iostream>

#include "FrontParser.hpp"

FileStructurePtr FrontParser::parse() {
    const std::vector<Token> & tokens = _ut.tokenize();
    if (_ut.hasErr()) {
        for (const auto & e : _ut.getErrList())
            std::cerr << e.getMsg() << std::endl;
        return nullptr;
    }
    
    FileStructurePtr pFileStu = _ap.parse(tokens);
    if (_ap.hasErr()) {
        for (const auto & e : _ap.getErrList())
            std::cerr << e.getMsg() << std::endl;
        return nullptr;
    }
    
    return pFileStu;
}