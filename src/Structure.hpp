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
    void setStmtsPtr(const StmtsPtr & pStmts);
    
    const StmtsPtr & getStmtsPtr() const;
    
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
    void setName(const LocatedUtfString & name);
    
    void setRetType(const VarType & retType);
    
    void setFormalArgList(const FormalArgList & fal);
    
    const LocatedUtfString & getName() const;
    
    const VarType & getRetType() const;
    
    const FormalArgList & getFormalArgList() const;
    
    const FunctionStructure & getFuncStu() const;
    
    Function() = default;
    //Function(const Function &) = delete;
    //Function & operator=(const Function &) = delete;
    
    FunctionStructure & getFuncStuRef();
    
    const StmtsPtr & getContextStmtsPtr() const;
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

    void addFunc(const Function & f);
    
    void addGlobalVar(const GlobalVar & gv);

    const std::vector<Function> & getFuncs() const;
    
    const std::vector<GlobalVar> & getGlobalVars() const;

    static FileStructurePtr newFileStu(
            const std::string & filename,
            const std::vector<GlobalVar> & gvars,
            const std::vector<Function> & funs);

    static FileStructurePtr newFileStu(const std::string & filename);
};

#endif