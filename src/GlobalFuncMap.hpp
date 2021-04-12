#ifndef __GLOBAL_FUNC_MAP_HEADER__
#define __GLOBAL_FUNC_MAP_HEADER__

#include <unordered_map>

class GlobalFuncMap {
private:
	static std::vector<Function> _funcs;
	static std::unordered_map<std::string, size_t> _idxMap;

public:
	static Function * getFunc(const std::string & name) {
		decltype(_idxMap.cbegin()) it;
		if ((it = _idxMap.find(name)) == _idxMap.cend())
			return nullptr;
		return &_funcs[it->second];
	}

	static bool putFunc(const std::string & name, const Function & func) {
		auto i = _funcs.size();
		if (getFunc(name))
			return false;
		_idxMap.insert(std::make_pair(name, i));
		_funcs.push_back(func);
		return true;
	}
};

std::vector<Function> GlobalFuncMap::_funcs;
std::unordered_map<std::string, size_t> GlobalFuncMap::_idxMap;

#endif