#ifndef __UNITOKENIZER_HEADER__
#define __UNITOKENIZER_HEADER__

#include <vector>
#include "TokenScanner.hpp"

class UniTokenizer {
private:
    std::string _filename;
    std::vector<Token> _tks;
    std::vector<Error> _errs;
    std::ifstream _ifs;
    LineScanner _ls;

public:
    UniTokenizer(const std::string & filename)
            : _filename(filename),
            _ifs(_filename, std::ios::in),
            _ls(_ifs) {
        if (!_ifs.is_open()) {
            std::cerr << "~ 无法打开文件 \'" << filename << "\'" << std::endl;
            exit(1);
        }
    }
    UniTokenizer()
            : _filename("<stdin>"),
              _ls(std::cin) { }
    UniTokenizer(const UniTokenizer &) = delete;
    
    const std::vector<Token> & tokenize();
    const std::vector<Error> & getErrList() { return _errs; }
    bool hasErr() const { return ! _errs.empty(); }
};

#endif