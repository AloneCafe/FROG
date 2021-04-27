#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>

#include "GlobalFuncMap.hpp"
#include "Structure.hpp"

std::vector<Function> GlobalFuncMap::_funcs;
std::unordered_map<std::string, size_t> GlobalFuncMap::_idxMap;

Function * GlobalFuncMap::getFuncByMangling(
        const std::string & fname,
        const std::vector<FormalArg> & fargs)
{
    std::string name{ nameMangling(fname, fargs) };
    decltype(_idxMap.cbegin()) it;
    if ((it = _idxMap.find(name)) == _idxMap.cend())
        return nullptr;
    return &_funcs[it->second];
}

std::vector<Function> GlobalFuncMap::getFuncsNoMangling(
        const std::string & name)
{
    std::vector<Function> funcs;
    for (const auto & func : _funcs) {
        if (func.getName().toString() == name) {
            funcs.push_back(func);
        }
    }
    return funcs;
}

bool GlobalFuncMap::putFunc(const std::string & fname,
        const std::vector<FormalArg> & fargs,
        const Function & func)
{
    auto i = _funcs.size();
    std::string name{ nameMangling(fname, fargs) };
    if (getFuncByMangling(fname, fargs))
        return false;
    _idxMap.insert(std::make_pair(name, i));
    
    _funcs.push_back(func);
    return true;
}

std::string GlobalFuncMap::nameMangling(
        const std::string & fname,
        const std::vector<FormalArg> & fargs)
{
    std::stringstream ss;
    ss << fname;
    for (const FormalArg & farg : fargs) {
        ss << "__" << farg.getType().toString();
    }
    return ss.str();
}