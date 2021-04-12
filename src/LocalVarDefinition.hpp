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
        static InitEntityPtr newInitExpr(const ExprPtr & pExpr) {
            InitEntityPtr pEntity(new InitExpr(pExpr));
            return pEntity;
        }
        static InitEntityPtr newInitList(const std::vector<InitEntityPtr> & entities) {
            InitEntityPtr pEntity(new InitList(entities));
            return pEntity;
        }
    };
    
    struct InitExpr : public IInitEntity {
        ExprPtr _pExpr; // 一般表达式
        InitEntityType getEntityType() const override {
            return InitEntityType::Expr;
        }
        
        explicit InitExpr(const ExprPtr & pExpr) { _pExpr = pExpr; }
        InitExpr(const InitExpr &) = delete;
        InitExpr operator=(const InitExpr &) = delete;
    };
    
    struct InitList : public IInitEntity {
        // List 中可能包含的元素：List 或者 Expr，用智能指针的多态形式来表示
        std::vector<InitEntityPtr> _entities;
        InitEntityType getEntityType() const override {
            return InitEntityType::List;
        }
        
        explicit InitList(const std::vector<InitEntityPtr> & entities) {
            _entities = entities;
        }
        InitList(const InitList &) = delete;
        InitList operator=(const InitList &) = delete;
    };
    
protected:
    VarType _type;
    LocatedUtfString _name;
    InitEntityPtr _pEntity = nullptr; // 如果不给初始化表达式，此处应设为 nullptr

public:
    void setName(const LocatedUtfString & name) { _name = name; }
    void setType(const VarType & type) { _type = type; }
    
    bool hasInit() const { return _pEntity != nullptr; }

    const auto & getType() const { return _type; }
    const auto & getName() const { return _name; }
    
    static void copySameModifierAndType(const LocalVar & from, LocalVar & to) {
        to._type = from._type;
    }
    
    bool isListInit() const {
        assert(_pEntity != nullptr);
        return _pEntity->getEntityType() == InitEntityType::List;
    }
    
    bool isExprInit() const {
        assert(_pEntity != nullptr);
        return _pEntity->getEntityType() == InitEntityType::Expr;
    }
    
    void setInitList(const std::vector<InitEntityPtr> & entities) {
        assert(_pEntity == nullptr); // 必须为 nullptr
        _pEntity = IInitEntity::newInitList(entities);
    }
    
    void setInitExpr(const ExprPtr & pExpr) {
        assert(_pEntity == nullptr); // 必须为 nullptr
        _pEntity = IInitEntity::newInitExpr(pExpr);
    }

    InitList * getInitListNativePtr() const {
        assert(isListInit());
        return static_cast<InitList *>(_pEntity.get());
    }

    InitExpr * getInitExprNativePtr() const {
        assert(isExprInit());
        return static_cast<InitExpr *>(_pEntity.get());
    }
    
    LocalVar() = default;
    //ScopeObject(const ScopeObject &) = delete;
    //ScopeObject & operator=(const ScopeObject &) = delete;
};

#endif
