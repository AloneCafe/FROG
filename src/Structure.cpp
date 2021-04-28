#include <vector>

#include "StructureBase.hpp"
#include "ExprDefinition.hpp"
#include "Statements.hpp"
#include "Structure.hpp"

const StmtsPtr & FunctionStructure::getStmtsPtr() const {
    return _pStmts;
}

void FunctionStructure::setStmtsPtr(const StmtsPtr & pStmts) {
    _pStmts = pStmts;
}

void Function::setName(const LocatedUtfString & name) {
    _name = name;
}

void Function::setRetType(const VarType & retType) {
    _retType = retType;
}

void Function::setFormalArgList(const FormalArgList & fal) {
    _fal = fal;
}

const LocatedUtfString & Function::getName() const {
    return _name;
}

const VarType & Function::getRetType() const {
    return _retType;
}

const FormalArgList & Function::getFormalArgList() const {
    return _fal;
}

const FunctionStructure & Function::getFuncStu() const {
    return _funStu;
}

FunctionStructure & Function::getFuncStuRef() {
    return _funStu;
}

const StmtsPtr & Function::getContextStmtsPtr() const {
    return _funStu.getStmtsPtr();
}

void FileStructure::addFunc(const Function & f) {
    _funs.push_back(f);
}

void FileStructure::addGlobalVar(const GlobalVar & gv) {
    _gvars.push_back(gv);
}

const std::vector<Function> & FileStructure::getFuncs() const {
    return _funs;
}

const std::vector<GlobalVar> & FileStructure::getGlobalVars() const {
    return _gvars;
}

FileStructurePtr FileStructure::newFileStu(const std::string & filename, const std::vector<GlobalVar> & gvars, const std::vector<Function> & funs) {
    FileStructurePtr pFileStu = std::make_shared<FileStructure>();
    pFileStu->_filename = filename;
    pFileStu->_gvars = gvars;
    pFileStu->_funs = funs;
    return pFileStu;
}

FileStructurePtr FileStructure::newFileStu(const std::string & filename) {
    FileStructurePtr pFileStu = std::make_shared<FileStructure>();
    pFileStu->_filename = filename;
    return pFileStu;
}