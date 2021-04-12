#ifndef __UNIPARSER_HEADER__
#define __UNIPARSER_HEADER__

#include <memory>
#include <fstream>
#include <vector>
#include <string>

#include "FrontParser.hpp"

class UniParser {
private:
    std::vector<std::string> _filenames;

public:
    UniParser(const std::vector<std::string> & filenames)
            : _filenames(filenames) { }
    UniParser() = default;
    ~UniParser() = default;
    
    std::string parse() {
        std::stringstream ss;
        if (!_filenames.empty()) {
            std::vector<FileStructurePtr> fileStus;
            for (const std::string & filename : _filenames) {
                FrontParser p(filename);
                FileStructurePtr pFileStu = p.parse();
                if (!pFileStu)
                    return "";
                fileStus.push_back(pFileStu);
            }
            
            SemParser sp;
            sp.parse(fileStus);
            if (sp.hasErr()) {
                for (const auto & e : sp.getErrList())
                    std::cerr << e.getMsg() << std::endl;
                return "";
            }
            ss << sp._asmk.getContextRef().str();
            
        } else {
            FrontParser p;
            FileStructurePtr pFileStu = p.parse();
            if (!pFileStu)
                return "";
            
            SemParser sp;
            sp.parse({ pFileStu });
            if (sp.hasErr()) {
                for (const auto & e : sp.getErrList())
                    std::cerr << e.getMsg() << std::endl;
                return "";
            }
            ss << sp._asmk.getContextRef().str();
        }
        
        return ss.str();
    }
};

#endif
