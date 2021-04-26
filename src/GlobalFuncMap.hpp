#ifndef __GLOBAL_FUNC_MAP_HEADER__
#define __GLOBAL_FUNC_MAP_HEADER__

#include <unordered_map>
#include <vector>

class GlobalFuncMap {
private:
	static std::vector<Function> _funcs;
	static std::unordered_map<std::string, size_t> _idxMap;

public:
	static Function * getFuncByMangling(const std::string & fname, const std::vector<FormalArg> & fargs) {
        std::string name{ nameMangling(fname, fargs) };
		decltype(_idxMap.cbegin()) it;
		if ((it = _idxMap.find(name)) == _idxMap.cend())
			return nullptr;
		return &_funcs[it->second];
	}
    
    static std::vector<Function> getFuncsNoMangling(const std::string & name) {
	    std::vector<Function> funcs;
	    for (const auto & func : _funcs) {
	        if (func.getName().toString() == name) {
	            funcs.push_back(func);
	        }
	    }
	    return funcs;
    }

	static bool putFunc(const std::string & fname, const std::vector<FormalArg> & fargs, const Function & func) {
		auto i = _funcs.size();
		std::string name{ nameMangling(fname, fargs) };
		if (getFuncByMangling(fname, fargs))
			return false;
		_idxMap.insert(std::make_pair(name, i));
		
		_funcs.push_back(func);
		return true;
	}
	
	static std::string nameMangling(const std::string & fname, const std::vector<FormalArg> & fargs) {
	    std::stringstream ss;
	    ss << fname;
	    for (const FormalArg & farg : fargs) {
	        ss << "__" << farg.getType().toString();
	    }
	    return ss.str();
	}
	
};

std::vector<Function> GlobalFuncMap::_funcs;
std::unordered_map<std::string, size_t> GlobalFuncMap::_idxMap;

#endif