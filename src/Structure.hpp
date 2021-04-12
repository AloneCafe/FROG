#ifndef __STRUCTURE_CLASS_HEADER__
#define __STRUCTURE_CLASS_HEADER__

#include <vector>
#include "StructureBase.hpp"
#include "ExprDefinition.hpp"
#include "Statements.hpp"

struct FunctionStructure {
private:
    StmtsPtr _pStmts = nullptr; //必须具备某种行为，能够放入表达式

public:
    void setStmtsPtr(const StmtsPtr & pStmts) {
        _pStmts = pStmts;
    }
    
    const auto & getStmtsPtr() const { return _pStmts; }
    
    FunctionStructure() = default;
    //FunctionStructure(const FunctionStructure &) = delete;
    //FunctionStructure & operator=(const FunctionStructure &) = delete;
};

struct Function {
    VarType _retType;
    LocatedUtfString _name;
    FormalArgList _fal;
    FunctionStructure _funStu;

public:
    void setName(const LocatedUtfString & name) { _name = name; }
    void setRetType(const VarType & retType) { _retType = retType; }
    void setFormalArgList(const FormalArgList & fal) { _fal = fal; }
    
    const auto & getName() const { return _name; }
    const auto & getRetType() const { return _retType; }
    const auto & getFormalArgList() const { return _fal; }
    const auto & getFuncStu() const { return _funStu; }
    
    Function() = default;
    //Function(const Function &) = delete;
    //Function & operator=(const Function &) = delete;
    
    FunctionStructure & getFuncStuRef() {
        return _funStu;
    }
    
    const auto & getContextStmtsPtr() const {
        return _funStu.getStmtsPtr();
    }
};

using GlobalVar = LocalVar;

struct FileStructure;
using FileStructurePtr = std::shared_ptr<FileStructure>;

struct FileStructure {
    std::string _filename;
    std::vector<GlobalVar> _gvars;
    std::vector<Function> _funs;
    
    FileStructure() = default;
    FileStructure(const FileStructure &) = delete;
    FileStructure & operator=(const FileStructure &) = delete;

    void addFunc(const Function & f) { _funs.push_back(f); }
    void addGlobalVar(const GlobalVar & gv) { _gvars.push_back(gv); }

    const auto & getFuncs() const { return _funs; }
    const auto & getGlobalVars() const { return _gvars; }

    static FileStructurePtr newFileStu(const std::string & filename, const std::vector<GlobalVar> & gvars, const std::vector<Function> & funs) {
        FileStructurePtr pFileStu = std::make_shared<FileStructure>();
        pFileStu->_filename = filename;
        pFileStu->_gvars = gvars;
        pFileStu->_funs = funs;
        return pFileStu;
    }

    static FileStructurePtr newFileStu(const std::string & filename) {
        FileStructurePtr pFileStu = std::make_shared<FileStructure>();
        pFileStu->_filename = filename;
        return pFileStu;
    }
    
    static FileStructurePtr makeAll2One(const std::vector<FileStructurePtr> & fileStus) {
        return nullptr; // TODO
    }
};

#endif