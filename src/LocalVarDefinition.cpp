#include "LocalVarDefinition.hpp"

LocalVar::InitEntityPtr LocalVar::IInitEntity::newInitExpr(const ExprPtr & pExpr) {
    InitEntityPtr pEntity(new InitExpr(pExpr));
    return pEntity;
}

LocalVar::InitEntityPtr LocalVar::IInitEntity::newInitList(const std::vector<InitEntityPtr> & entities) {
    InitEntityPtr pEntity(new InitList(entities));
    return pEntity;
}

LocalVar::InitEntityType LocalVar::InitExpr::getEntityType() const {
    return InitEntityType::Expr;
}

LocalVar::InitExpr::InitExpr(const ExprPtr & pExpr) {
    _pExpr = pExpr;
}

LocalVar::InitEntityType LocalVar::InitList::getEntityType() const {
    return InitEntityType::List;
}

LocalVar::InitList::InitList(const std::vector<InitEntityPtr> & entities) {
    _entities = entities;
}

void LocalVar::setName(const LocatedUtfString & name) {
    _name = name;
}

void LocalVar::setType(const VarType & type) {
    _type = type;
}

bool LocalVar::hasInit() const {
    return _pEntity != nullptr;
}

const VarType & LocalVar::getType() const {
    return _type;
}

const LocatedUtfString & LocalVar::getName() const {
    return _name;
}

void LocalVar::copySameModifierAndType(const LocalVar & from, LocalVar & to) {
    to._type = from._type;
}

bool LocalVar::isListInit() const {
    assert(_pEntity != nullptr);
    return _pEntity->getEntityType() == InitEntityType::List;
}

bool LocalVar::isExprInit() const {
    assert(_pEntity != nullptr);
    return _pEntity->getEntityType() == InitEntityType::Expr;
}

void LocalVar::setInitList(const std::vector<InitEntityPtr> & entities) {
    assert(_pEntity == nullptr); // 必须为 nullptr
    _pEntity = IInitEntity::newInitList(entities);
}

void LocalVar::setInitExpr(const ExprPtr & pExpr) {
    assert(_pEntity == nullptr); // 必须为 nullptr
    _pEntity = IInitEntity::newInitExpr(pExpr);
}

LocalVar::InitList * LocalVar::getInitListNativePtr() const {
    assert(isListInit());
    return static_cast<InitList *>(_pEntity.get());
}

LocalVar::InitExpr * LocalVar::getInitExprNativePtr() const {
    assert(isExprInit());
    return static_cast<InitExpr *>(_pEntity.get());
}