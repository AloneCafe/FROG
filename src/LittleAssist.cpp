#include "LittleAssist.hpp"

extern "C" {
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
}

std::string StringOperator::format(const char * fmt, ...) {
    constexpr int len = 4096;
    char * buff = new char[len];
    va_list va;
    va_start(va, fmt);
    vsprintf(buff, fmt, va);
    va_end(va);
    std::string s(buff);
    delete[] buff;
    return s;
}

std::string StringOperator::W2A(const std::wstring & ws) {
    setlocale(LC_CTYPE, "");
    const long memLen = ws.length() + 1;
    char * buff = new char[memLen];
    wcstombs(buff, ws.c_str(), memLen);
    std::string s(buff);
    delete[] buff;
    return s;
}

std::wstring StringOperator::A2W(const std::string & as) {
    setlocale(LC_CTYPE, "");
    const long memLen = as.length() + 1;
    wchar_t * buff = new wchar_t[memLen];
    mbstowcs(buff, as.c_str(), memLen);
    std::wstring s(buff);
    delete[] buff;
    return s;
}

unsigned long SeqConverter::DigitSeq::dec2oct(uint64_t dec) {
    unsigned long oct = 0;
    while (dec) {
        oct = oct * 8 + dec % 10;
        dec /= 10;
    }
    return oct;
}

UtfString SeqConverter::toEscape(UtfString & native) {
    UtfString escape;
    int len = native.length();
    for (int i = 0; i < len; i ++) escape += toEscape(native[i]);
    return escape;
}

UtfString SeqConverter::toEscape(UtfChar & native) {
    UtfString escape;
    switch (native.getAsciChar()) {
    case '\'':
        escape = "\\\'";
        break;
    case '\"':
        escape = "\\\"";
        break;
    case '\?':
        escape = "\\\?";
        break;
    case '\\':
        escape = "\\\\";
        break;
    case '\a':
        escape = "\\a";
        break;
    case '\b':
        escape = "\\b";
        break;
    case '\f':
        escape = "\\f";
        break;
    case '\n':
        escape = "\\n";
        break;
    case '\r':
        escape = "\\r";
        break;
    case '\t':
        escape = "\\t";
        break;
    case '\v':
        escape = "\\v";
        break;
        //case CHARACTER_EOF: escape = "EOF"; break;
    default:
        escape = native;
    }
    return escape;
}

std::string FileSystem::getCWD() {
    std::string result;
#ifdef _WIN32
    CHAR buff[MAX_PATH + 1];
    GetCurrentDirectoryA(MAX_PATH + 1, buff);
    result = buff;
#else
    char buff[PATH_MAX + 1];
    getcwd(buff, PATH_MAX + 1);
    setlocale(LC_CTYPE, "");
    result = buff;
#endif
    return result;
}

bool FileSystem::fileExist(const std::string & filename) {
    std::ifstream i(filename);
    return (i ? (i.close(), true) : false);
}

std::string FileSystem::path2FileName(const std::string & filename) {
    std::stack<char> path;
    size_t len = filename.size();
    for (size_t i = len; i >= 1; --i) {
        char ch = filename[i - 1];
        if (ch == '/' || ch == '\\')
            break;
        path.push(ch);
    }
    std::stringstream ss;
    while (!path.empty()) {
        ss << path.top();
        path.pop();
    }
    return ss.str();
}
