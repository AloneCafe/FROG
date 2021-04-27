#ifndef __FRONT_PARSER_HEADER__
#define __FRONT_PARSER_HEADER__

#include <memory>
#include <fstream>
#include <vector>
#include <string>

#include "UnitedTokenizer.hpp"
#include "SyntaxTreeParser.hpp"
#include "SemanticParser.hpp"

class FrontParser {
private:
    std::string _filename;
    UniTokenizer _ut;
    AstParser _ap;
    SemParser _sp;

public:
    FrontParser(const std::string & filename) :
        _filename(filename),
        _ap(_filename),
        _ut(_filename),
        _sp(_filename) { }
        
    FrontParser() :
        _filename("<stdin>"),
        _ap(),
        _ut(),
        _sp() { }
        
    ~FrontParser() = default;

public:
    FileStructurePtr parse();
};

#endif
