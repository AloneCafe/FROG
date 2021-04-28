#ifndef __LOCAL_VAR_DEF_HEADER__
#define __LOCAL_VAR_DEF_HEADER__

#include "ExprDefinition.hpp"

struct LocalVar {
public:
    enum class InitEntityType : int {
        Expr = 0,
        List
    };
    
    struct IInitEntity;
    using InitEntityPtr = std::shared_ptr<IInitEntity>;
    
    struct IInitEntity {
        virtual InitEntityType getEntityType() const = 0;
        
        static InitEntityPtr newInitExpr(const ExprPtr & pExpr);
        
        static InitEntityPtr newInitList(const std::vector<InitEntityPtr> & entities);
    };
    
    struct InitExpr : public IInitEntity {
        ExprPtr _pExpr; // 一般表达式
        
        InitEntityType getEntityType() const override;
        
        explicit InitExpr(const ExprPtr & pExpr);
        
        InitExpr(const InitExpr &) = delete;
        InitExpr operator=(const InitExpr &) = delete;
    };
    
    struct InitList : public IInitEntity {
        // List 中可能包含的元素：List 或者 Expr，用智能指针的多态形式来表示
        std::vector<InitEntityPtr> _entities;
        
        InitEntityType getEntityType() const override;
        
        explicit InitList(const std::vector<InitEntityPtr> & entities);
        
        InitList(const InitList &) = delete;
        InitList operator=(const InitList &) = delete;
    };
    
protected:
    VarType _type;
    LocatedUtfString _name;
    InitEntityPtr _pEntity = nullptr; // 如果不给初始化表达式，此处应设为 nullptr

public:
    void setName(const LocatedUtfString & name);
    
    void setType(const VarType & type);
    
    bool hasInit() const;

    const VarType & getType() const;
    
    const LocatedUtfString & getName() const;
    
    static void copySameModifierAndType(const LocalVar & from, LocalVar & to);
    
    bool isListInit() const;
    
    bool isExprInit() const;
    
    void setInitList(const std::vector<InitEntityPtr> & entities);
    
    void setInitExpr(const ExprPtr & pExpr);

    InitList * getInitListNativePtr() const;

    InitExpr * getInitExprNativePtr() const;
    
    LocalVar() = default;
    //ScopeObject(const ScopeObject &) = delete;
    //ScopeObject & operator=(const ScopeObject &) = delete;
};



#endif
