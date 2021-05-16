#include "UnitedTokenizer.hpp"

const std::vector<Token> & UniTokenizer::tokenize() {
    _tks.clear();   // 清空token列表
    _errs.clear();  // 清空错误列表
    
    CombinedLine cl;
    static Token tkEof;
    while ((cl = _ls.nextLine()), cl.getType() != CombinedLine::Terminator) {
        TokenScanner ts(cl, _filename);
        Token tk;
        while ((tk = ts.nextToken()), tk.getType() != TOKEN_TERMINATOR)
            _tks.push_back(tk);
        tkEof = tk;
        const auto & errs = ts.getErrList();
        for (const auto & err : errs)
            _errs.push_back(err);
    }
    _tks.push_back(tkEof);

#if 0
    for (const auto & e : _tks) {
        std::cerr << e.toDebugString() << std::endl;
    }
#endif
    
    return _tks;
}
