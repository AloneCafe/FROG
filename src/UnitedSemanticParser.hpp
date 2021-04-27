#ifndef __UNI_SEM_PARSER_HEADER__
#define __UNI_SEM_PARSER_HEADER__

#include <memory>
#include <fstream>
#include <vector>
#include <string>

#include "FrontParser.hpp"

class UniSemParser {
private:
    std::vector<std::string> _filenames;
    std::vector<std::string> _extfilenames;

public:
    UniSemParser(const std::vector<std::string> & filenames,
              const std::vector<std::string> & extfilenames)
            : _filenames(filenames), _extfilenames(extfilenames) { }
            
    UniSemParser() = default;
    
    ~UniSemParser() = default;
    
    std::string parse();
};

#endif
