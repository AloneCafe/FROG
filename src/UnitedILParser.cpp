#include "UnitedILParser.hpp"

const std::vector<char> &
UniILParser::getBytesFuncs() const {
    return _bytesFuncs;
}

const std::vector<char> &
UniILParser::getBytesStatic() const {
    return _bytesStatic;
}

bool UniILParser::parse() {
    if (!_filename.empty()) {
        ILParser ap(_filename);
        bool result = ap.parse();
        if (!result)
            return false;
        
        for (auto b : ap.getBytesFuncs())
            _bytesFuncs.push_back(b);
        for (auto b : ap.getBytesStatic())
            _bytesStatic.push_back(b);
        
        if (ap.runnable()) {
            _runnable = true;
            _dwEntryPoint = ap._dwEntryPoint;
        }
        
    } else {
        ILParser ap;
        bool result = ap.parse();
        if (!result)
            return false;
        
        for (auto b : ap.getBytesFuncs())
            _bytesFuncs.push_back(b);
        for (auto b : ap.getBytesStatic())
            _bytesStatic.push_back(b);
    
        if (ap.runnable()) {
            _runnable = true;
            _dwEntryPoint = ap._dwEntryPoint;
        }
    }
    
    return true;
}