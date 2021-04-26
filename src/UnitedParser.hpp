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
    std::vector<std::string> _extfilenames;

public:
    UniParser(const std::vector<std::string> & filenames,
              const std::vector<std::string> & extfilenames)
            : _filenames(filenames), _extfilenames(extfilenames) { }
    UniParser() = default;
    ~UniParser() = default;
    
    std::string parse() {
        std::stringstream ss;
        std::vector<FileStructurePtr> fileStus;
        
        if (!_filenames.empty()) {
            for (const std::string & extfilename : _extfilenames) {
                FrontParser p(extfilename);
                FileStructurePtr pFileStu = p.parse();
                if (!pFileStu)
                    return "";
                fileStus.push_back(pFileStu);
            }
            
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
            
            for (const std::string & extfilename : _extfilenames) {
                FrontParser p(extfilename);
                FileStructurePtr pFileStu = p.parse();
                if (!pFileStu)
                    return "";
                fileStus.push_back(pFileStu);
            }
            
            FrontParser p;
            FileStructurePtr pFileStu = p.parse();
            if (!pFileStu)
                return "";
            fileStus.push_back(pFileStu);
            
            SemParser sp;
            sp.parse(fileStus);
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
