#include "ErrorDriver.hpp"

#define EW_FORMAT(x) StringOperator::format("~ 源文件 %s (第 %ld 行, 第 %ld 列), %s", filename.c_str(), lineno, colno, x)

bool operator==(const std::tuple<ErrorType, int, int> & a,
        const std::tuple<ErrorType, int, int> & b) {
	return std::get<0>(a) == std::get<0>(b) &&
		std::get<1>(a) == std::get<1>(b) &&
		std::get<2>(a) == std::get<2>(b);
}

Error::Error(const ErrorType & type, const std::string & filename, long lineno,
        long colno) {
    _type = type;
    _lineno = lineno;
    _colno = colno;
    switch (_type) {
    
    case E_INVALID_OCTDIGIT_SEQ:
        _msg = EW_FORMAT("无效的八进制数字常量序列");
        break;
    
    case E_INVALID_EXP:
        _msg = EW_FORMAT("无效的指数序列");
        break;
    
    case E_INCMP_HEXDIGIT_SEQ:
        _msg = EW_FORMAT("不完整的十六进制数字常量序列");
        break;
    
    case E_INCMP_STRING_SEQ:
        _msg = EW_FORMAT("不完整的字符串常量序列");
        break;
    
    case E_INCMP_CHAR_SEQ:
        _msg = EW_FORMAT("不完整的字符常量序列");
        break;
    
    case E_INVALID_CHAR:
        _msg = EW_FORMAT("遭遇无效的字符");
        break;
    
    case E_ID_TOO_LONG:
        _msg = EW_FORMAT("标识符超过限定长度");
        break;
    
    case E_CHARSEQ_TOO_LONG:
        _msg = EW_FORMAT("字符序列超过限定长度");
        break;
    
    case E_STRING_TOO_LONG:
        _msg = EW_FORMAT("字符串超过限定长度");
        break;
    
    case E_ILLEGAL_TOKEN:
        _msg = EW_FORMAT("该token在此处非法");
        break;
    
    case E_UNEXPECTED_EOF:
        _msg = EW_FORMAT("意外的处理流结尾");
        break;
    
    case E_INCMP_ID_LIST:
        _msg = EW_FORMAT("不完整的标识符列表");
        break;
    
    case E_INCMP_IMPLEMENTS_LIST:
        _msg = EW_FORMAT("不完整的 implements 列表");
        break;
    
    case E_INCMP_EXTENDS_LIST:
        _msg = EW_FORMAT("不完整的 extends 列表");
        break;
    
    case E_ILLEGAL_PKGNAME:
        _msg = EW_FORMAT("非法的 package 名字");
        break;
    
    case E_LOSE_SEMICOLON:
        _msg = EW_FORMAT("此处可能缺少分号 ';'");
        break;
    
    case E_ILLEGAL_IMPORT_CLASSNAME:
        _msg = EW_FORMAT("非法的 import class 名字");
        break;
    
    case E_MULTIPLE_DEFINITION_PKGNAME:
        _msg =
                EW_FORMAT("多重 package 归属定义，每份源文件不可定义多个 package 归属");
        break;
    
    case E_DEFINE_STATIC_OBJ:
        _msg = EW_FORMAT("变量不可修饰为 static ");
        break;
    
    case E_ILLEGAL_CLASS_MODIFIER:
        _msg = EW_FORMAT("非法的修饰符: class 只能用 final、public、abstract 修饰");
        break;
    
    case E_ILLEGAL_INTERFACE_MODIFIER:
        _msg = EW_FORMAT("非法的修饰符: interface 只能用 public、abstract 修饰");
        break;
    
    case E_INTERFACE_EXPECT_EXTENDS:
        _msg = EW_FORMAT("非法的修饰符: interface 只能用 extends "
                         "关键字，继承自一个或多个 super-interface");
        break;
    
    case E_ILLEGAL_PACKAGE_POS:
        _msg = EW_FORMAT("package 包归属只能定义在源文件开头");
        break;
    
    case E_ILLEGAL_IMPORT_POS:
        _msg = EW_FORMAT("import 导入类只能定义在 package 之后、类和接口定义之前");
        break;
    
    case E_LOSE_RP:
        _msg = EW_FORMAT("此处可能缺少右括号 ')'");
        break;
    
    case E_LOSE_COLON:
        _msg = EW_FORMAT("此处可能缺少冒号 ':'");
        break;
    
    case E_LOSE_RBRACKET:
        _msg = EW_FORMAT("此处可能缺少右括号 ']'");
        break;
    
    case E_LOSE_RBRACE:
        _msg = EW_FORMAT("此处可能缺少右括号 '}'");
        break;
    
    case E_EXPECT_LP:
        _msg = EW_FORMAT("此处应该需要左括号 '('");
        break;
    
    case E_ILLEGAL_FORMAL_ARG_TYPE:
        _msg = EW_FORMAT("非法的形式参数类型");
        break;
    
    case E_ILLEGAL_FORMAL_ARG_NAME:
        _msg = EW_FORMAT("非法的形式参数名称");
        break;
    
    case E_INCMP_NEW_EXPRESSION:
        _msg = EW_FORMAT("不完整的 new 表达式");
        break;
    
    case E_DUPLICATED_FINAL_MODIFIER:
        _msg = EW_FORMAT("重复的 final 修饰符");
        break;
    
    case E_DUPLICATED_STATIC_MODIFIER:
        _msg = EW_FORMAT("重复的 static 修饰符");
        break;
    
    case E_DUPLICATED_ABSTRACT_MODIFIER:
        _msg = EW_FORMAT("重复的 abstract 修饰符");
        break;
    
    case E_DUPLICATED_ACCESS_MODIFIER:
        _msg = EW_FORMAT("重复的访问权限修饰符");
        break;
    
    case E_ILLEGAL_MODIFIER:
        _msg = EW_FORMAT("非法的定义修饰符");
        break;
    
    case E_INCMP_TYPE_CONVERT:
        _msg = EW_FORMAT("数据类型不兼容，且不可隐式类型转换");
        break;
    
    case E_PKG_NOT_EXIST:
        _msg = EW_FORMAT("该 package 不存在");
        break;
    
    case E_IMPORT_TYPE_NOT_EXIST:
        _msg = EW_FORMAT("import 的类型不存在");
        break;
    
    case E_TYPE_REDEFINED:
        _msg = EW_FORMAT("该类型名重定义");
        break;
    
    case E_UNRECOGNIZED_TYPE_NAME:
        _msg = EW_FORMAT("未识别的类型名");
        break;
    
    case E_AMBIGUOUS_TYPE_NAME:
        _msg =
                EW_FORMAT("类型名存在歧义，可能 import 的多个包都包含同名的类型定义");
        break;
    
    case E_INHERIT_I2I:
        _msg = EW_FORMAT("接口只允许 extends 自超接口 (super-interface)");
        break;
    
    case E_INHERIT_C2I:
        _msg = EW_FORMAT("类只允许 implements 自超接口 (super-interface)");
        break;
    
    case E_INHERIT_C2C:
        _msg = EW_FORMAT("类只允许 extends 自超类 (super-class)");
        break;
    
    case E_FUNC_REDEFINED:
        _msg = EW_FORMAT("成员函数重定义");
        break;
    
    case E_FIELD_REDEFINED:
        _msg = EW_FORMAT("成员字段重定义");
        break;
    
    case E_CYCLIC_INHERITANCE:
        _msg = EW_FORMAT("不允许出现回环继承以及实现");
        break;
    
    case E_BOTH_FINAL_AND_ABSTRACT:
        _msg = EW_FORMAT("不允许同时存在修饰符 final 和 abstract");
        break;
    
    case E_INHERIT_FINAL_SUPER_CLASS:
        _msg = EW_FORMAT("不允许继承 final 类");
        break;
    
    case E_OVERRIDE_FINAL_FUNCTION:
        _msg = EW_FORMAT("不允许覆盖 final 函数");
        break;
    
    case E_CLASS_HAS_UNIMPLEMENTED_ABSTRACT_FUNCTIONS:
        _msg = EW_FORMAT("类存在未实现的 abstract 函数，要么将类定义为 abstract 的，"
                          "要么请在类中实现这些 abstract 函数 (继承得到的)");
        break;
    
    case E_INCMP_OVERRIDE_FUNC_RET_TYPE:
        _msg = EW_FORMAT("互不兼容的 override 函数返回值类型");
        break;
    
    case E_REDUCE_OVERRIDE_FUNC_ACCESSIBLITY:
        _msg = EW_FORMAT("不允许削弱 override 函数的可访问性");
        break;
    
    case E_ILLEGAL_TYPE_FOR_OBJ_DEFINITION:
        _msg = EW_FORMAT("非法的类型定义");
        break;
    
    case E_VOID_FOR_OBJ_DEFINITION:
        _msg = EW_FORMAT("void 类型不可用于定义对象");
        break;
    
    case E_OBJECT_REDEFINED:
        _msg = EW_FORMAT("变量 (对象) 重定义");
        break;
    
    case E_INCMP_TYPE_BOOLEAN:
        _msg = EW_FORMAT("不兼容的类型 (boolean)");
        break;
        
    case E_INCMP_TYPE_BYTE:
        _msg = EW_FORMAT("不兼容的类型 (byte)");
        break;
    
    case E_INCMP_TYPE_GENERIC_INT:
        _msg = EW_FORMAT("不兼容的类型 (宽泛整数)");
        break;
    
    case E_INCMP_TYPE_GENERIC_NUMBER:
        _msg = EW_FORMAT("不兼容的类型 (宽泛数值)");
        break;
    
    case E_INCMP_TYPE_DW:
        _msg = EW_FORMAT("不兼容的类型 (int)");
        break;
    
    case E_INCMP_TYPE_QW:
        _msg = EW_FORMAT("不兼容的类型 (long)");
        break;
    
    case E_INCMP_TYPE_FLT:
        _msg = EW_FORMAT("不兼容的类型 (float)");
        break;
    
    case E_INCMP_TYPE_DBL:
        _msg = EW_FORMAT("不兼容的类型 (double)");
        break;
    
    case E_INCMP_TYPE_CHAR:
        _msg = EW_FORMAT("不兼容的类型 (char)");
        break;
        
    case E_INCMP_TYPE_SHORT:
        _msg = EW_FORMAT("不兼容的类型 (short)");
        break;
    
    case E_INCMP_TYPE_LITERAL_STRING:
        _msg = EW_FORMAT("不兼容的类型 (字符串字面量)");
        break;
    
    case E_INCMP_TYPE_REF:
        _msg = EW_FORMAT("不兼容的类型 (引用)");
        break;

	case E_INCMP_TYPE:
		_msg = EW_FORMAT("不兼容的类型");
		break;
        
    case E_ID_UNDEFINED:
        _msg = EW_FORMAT("未定义的标识符ID");
        break;
        
    case E_ILLEGAL_THIS:
        _msg = EW_FORMAT("此处使用 this 非法");
        break;
    
    case E_ILLEGAL_SUPER:
        _msg = EW_FORMAT("此处使用 super 非法");
        break;
        
    case E_ILLEGAL_MEMBER:
        _msg = EW_FORMAT("对象中不存在该成员");
        break;

    case E_ILLEGAL_EXPR:
        _msg = EW_FORMAT("非法的表达式");
        break;

    case E_ASSIGN_LEFT_VAL:
        _msg = EW_FORMAT("赋值表达式左侧必须是可以被改变的左值");
        break;

    case E_ILLEGAL_TYPE_CONVERT_TYPE:
        _msg = EW_FORMAT("非法的类型转换到某个类型");
        break;

    case E_ILLEGAL_FUNCTION_TO_CALL:
        _msg = EW_FORMAT("非法的函数调用");
        break;

    case E_FUNC_UNDEFINED:
        _msg = EW_FORMAT("函数未定义");
        break;

    case E_FUNC_ARG_NUM_MISMATCH:
        _msg = EW_FORMAT("函数调用参数数量与函数原型不匹配");
        break;

    case E_UNSUPPORTED_LIST_INIT:
        _msg = EW_FORMAT("暂不支持列表初始化");
        break;

    case E_VAR_REDEFINED:
        _msg = EW_FORMAT("变量重定义");
        break;

    case E_FUNC_HAS_NO_RET_VALUE:
        _msg = EW_FORMAT("此函数不具有返回值");
        break;

    case E_FUNC_NEED_RET_VALUE:
        _msg = EW_FORMAT("此函数需要返回值");
        break;

    case E_IILEGAL_BREAK:
        _msg = EW_FORMAT("此处不可使用 break 语句");
        break;

    case E_IILEGAL_CONTINUE:
        _msg = EW_FORMAT("此处不可使用 continue 语句");
        break;

    case E_ILLEGAL_RET_TYPE:
        _msg = EW_FORMAT("非法的返回值类型");
        break;
        
    case E_MISMATCHING_FUNC:
        _msg = EW_FORMAT("无法找到匹配的重载函数");
        break;
        
    case E_ILLEGAL_INIT_STMT:
        _msg = EW_FORMAT("非法的初始化语句");
        break;
    
    case E_UNKNOWN:
    default:
        _msg = EW_FORMAT("发生未知错误");
        break;
    }
}

void ErrorDriver::fatalErr(const Error & err) {
    std::cerr << "编译错误：" << err.getMsg() << std::endl;
    if (_pfuncBeforeExit)
        _pfuncBeforeExit(_errors);
    std::exit(err.getErrCode());
}

void ErrorDriver::postErr(const Error & err) { 
    std::tuple<ErrorType, int, int> tuple = std::make_tuple(err.getErrType(), err.lineno(), err.colno());
    if (_set.find(tuple) == _set.cend()) {
        _set.insert(tuple);
        _errors.push_back(err);
    }
}

void ErrorDriver::postErr(const ErrorType & type, const std::string & filename,
        long lineno, long colno) {
    postErr(Error(type, filename, lineno + 1, colno));
}

void ErrorDriver::fatalErr(const ErrorType & type, const std::string & filename,
        long lineno, long colno) {
    fatalErr(Error(type, filename, lineno + 1, colno));
}