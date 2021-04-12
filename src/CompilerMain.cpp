#include "TokenScanner.hpp"
#include "UnitedTokenizer.hpp"
#include "UnitedParser.hpp"

static const char * pLogoC =
"  _____                      ____                          _  _             \n"
" |  ___|_ __  ___    __ _   / ___| ___   _ __ ___   _ __  (_)| |  ___  _ __ \n"
" | |_  | '__|/ _ \\  / _` | | |    / _ \\ | '_ ` _ \\ | '_ \\ | || | / _ \\| '__|\n"
" |  _| | |  | (_) || (_| | | |___| (_) || | | | | || |_) || || ||  __/| |   \n"
" |_|   |_|   \\___/  \\__, |  \\____|\\___/ |_| |_| |_|| .__/ |_||_| \\___||_|   \n"
"                    |___/                          |_|                      \n"
;

void printUsage(const std::string & path) {
	std::cout << pLogoC << std::endl;
    std::cout << "========== 欢迎使用 FROG 编译器 (FROG Compiler) ==========" << std::endl;
    std::cout << std::endl;
    std::cout << "- USAGE: " << path << " [OPTIONS]" << std::endl;
    std::cout << "- OPTIONS: " << std::endl;
    std::cout << "    infile1 infile2 ... infileN : 编译源文件列表" << std::endl;
    std::cout << "    (-o | --output) outfile     : 生成汇编文件" << std::endl;
    std::cout << "    (-I | --stdin)              : 从标准输入读取源代码" << std::endl;
    std::cout << "    (-O | --stdout)             : 将结果打印至标准输出" << std::endl;
    std::cout << "    -h                          : 显示帮助" << std::endl;
    std::cout << std::endl;
    std::cout << "+ EXAMPLE: " << path << " test.frog -o output.fas" << std::endl;
    std::cout << "+ EXAMPLE: " << path << " foo.frog -I -O"  << std::endl;
}

#if defined(_DEBUG)
int main() {
    FrontParser p("../test1.txt");
    DBGPRINT;
    return 0;
}
#else 
int main(int argc, const char * argv[]) {
    enum {
        NEED_INPUT_OR_OPTION,
        NEED_OUTPUT,
    } stat = NEED_INPUT_OR_OPTION;
    
    
    if (argc > 1) {
        
        bool flagStdin = false;
        std::vector<std::string> inFileNames;
        bool flagStdout = false;
        std::string outFileName;
        
        for (int i = 1; i < argc; ++i) {
            std::string s(argv[i]);
            if (!s.empty() && s[0] != '-') {
                if (stat == NEED_INPUT_OR_OPTION) {
                    if (!flagStdin)
                        inFileNames.push_back(s);
            
                } else if (stat == NEED_OUTPUT) {
                    if (!flagStdout)
                        outFileName = s;
                    stat = NEED_INPUT_OR_OPTION;
                }
            } else if (stat != NEED_OUTPUT) {
                if (s == "-o" || s == "--output") {
                    stat = NEED_OUTPUT;
            
                } else if (s == "-I" || s == "--stdin") {
                    flagStdin = true;
                    if (!inFileNames.empty()) {
                        std::cerr << "~ 显式指定标准输入, 已忽略源文件列表" << std::endl;
                    }
                
                } else if (s == "-O" || s == "--stdout") {
                    flagStdout = true;
                    if (!outFileName.empty()) {
                        std::cerr << "~ 显式指定标准输出, 将不会生成目标汇编文件" << std::endl;
                    }
            
                } else if (s == "-h" || s == "--help") {
                    std::stack<char> path;
                    size_t len = strlen(argv[0]);
                    for (size_t i = len; i >= 1; --i) {
                        char ch = argv[0][i - 1];
                        if (ch == '/' || ch == '\\')
                            break;
                        path.push(ch);
                    }
                    std::stringstream ss;
                    while (!path.empty()) {
                        ss << path.top();
                        path.pop();
                    }
                    printUsage(ss.str());
                    return 0;
                }
            } else {
                std::cerr << "~ 错误的参数用法, 可以附加 -h 或者 --help 参数以获取帮助" << std::endl;
                return 1;
            }
        }
    
        std::string outputContext;
        if (flagStdin) {
            UniParser up;
            outputContext = std::move(up.parse());
            
        } else {
            if (inFileNames.empty()) {
                std::cerr << "~ 输入文件列表为空" << std::endl;
                return 1;
            }
            UniParser up(inFileNames);
            outputContext = std::move(up.parse());
        }
        
        if (flagStdout) {
            std::cout << outputContext << std::endl;
        } else {
            if (outFileName.empty()) {
                std::cerr << "~ 未指定输出文件" << std::endl;
                return 1;
            }
            std::ofstream ofs(outFileName, std::ios::out);
            ofs << outputContext;
        }
        
    } else {
        std::stack<char> path;
        size_t len = strlen(argv[0]);
        for (size_t i = len; i >= 1; --i) {
            char ch = argv[0][i - 1];
            if (ch == '/' || ch == '\\')
                break;
            path.push(ch);
        }
        std::stringstream ss;
        while (!path.empty()) {
            ss << path.top();
            path.pop();
        }
        printUsage(ss.str());
        return 1;
    }
    return 0;
}


#endif