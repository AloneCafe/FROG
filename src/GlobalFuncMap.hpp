#ifndef __GLOBAL_FUNC_MAP_HEADER__
#define __GLOBAL_FUNC_MAP_HEADER__

#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>

#include "Structure.hpp"

class GlobalFuncMap {
private:
	static std::vector<Function> _funcs;
	static std::unordered_map<std::string, uint32_t> _idxMap;

public:
	static Function * getFuncByMangling(const std::string & fname, const std::vector<FormalArg> & fargs);
    
    static std::vector<Function> getFuncsNoMangling(const std::string & name);

	static bool putFunc(const std::string & fname, const std::vector<FormalArg> & fargs, const Function & func);
	
	static std::string nameMangling(const std::string & fname, const std::vector<FormalArg> & fargs);
	
};

#endif