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


UtfString SeqConverter::toNative(UtfString & escape) {
    UtfString native;
    uint32_t len = escape.length();
    bool readingEscape = false;
    for (uint32_t i = 0; i < len; ++i) {
        UtfChar & uc = escape[i];
        
        if (!readingEscape) {
            if (uc == '\\') {
                readingEscape = true;
            } else {
                native.push(uc);
            }
        } else {
            switch (uc.getAsciChar()) {
            case '\'':
                native.push('\'');
                break;
            case '\"':
                native.push('\"');
                break;
            case '\?':
                native.push('\?');
                break;
            case '\\':
                native.push('\\');
                break;
            case 'a':
                native.push('\a');
                break;
            case 'b':
                native.push('\b');
                break;
            case 'f':
                native.push('\f');
                break;
            case 'n':
                native.push('\n');
                break;
            case 'r':
                native.push('\r');
                break;
            case 't':
                native.push('\t');
                break;
            case 'v':
                native.push('\v');
                break;
            default:
                native.push(uc);
            }
            readingEscape = false;
        }
        
    }
    return native;
}
std::string SeqConverter::toEscape(const std::string & native) {
    std::string escape;
    int len = native.length();
    for (int i = 0; i < len; i ++) escape += toEscape(native[i]);
    return escape;
}

std::string SeqConverter::toEscape(char native) {
    std::string escape;
    switch (native) {
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

std::string SeqConverter::toNative(const std::string & escape) {
    std::string native;
    uint32_t len = escape.length();
    bool readingEscape = false;
    for (uint32_t i = 0; i < len; ++i) {
        char uc = escape[i];
        
        if (!readingEscape) {
            if (uc == '\\') {
                readingEscape = true;
            } else {
                native.push_back(uc);
            }
        } else {
            switch (uc) {
            case '\'':
                native.push_back('\'');
                break;
            case '\"':
                native.push_back('\"');
                break;
            case '\?':
                native.push_back('\?');
                break;
            case '\\':
                native.push_back('\\');
                break;
            case 'a':
                native.push_back('\a');
                break;
            case 'b':
                native.push_back('\b');
                break;
            case 'f':
                native.push_back('\f');
                break;
            case 'n':
                native.push_back('\n');
                break;
            case 'r':
                native.push_back('\r');
                break;
            case 't':
                native.push_back('\t');
                break;
            case 'v':
                native.push_back('\v');
                break;
            default:
                native.push_back(uc);
            }
            readingEscape = false;
        }
        
    }
    return native;
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

bool FileSystem::fileExist(const char * filename) {
    return fileExist(std::string(filename));
}

std::string FileSystem::path2FileName(const std::string & filename) {
    std::stack<char> path;
    uint32_t len = filename.size();
    for (uint32_t i = len; i >= 1; --i) {
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

std::string FileSystem::path2FileName(const char * filename) {
    return path2FileName(std::string(filename));
}
