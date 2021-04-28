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
    if (!_filenames.empty()) {
        for (const std::string & filename : _filenames) {
            ILParser ap(filename);
            bool result = ap.parse();
            if (!result)
                return false;
            
            for (auto b : ap.getBytesFuncs())
                _bytesFuncs.push_back(b);
            for (auto b : ap.getBytesStatic())
                _bytesStatic.push_back(b);
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
    }
    
    return true;
}