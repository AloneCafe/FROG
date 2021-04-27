#include "TokenScanner.hpp"
#include "UnitedTokenizer.hpp"
#include "UnitedSemanticParser.hpp"

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
    std::cout << "    (-e | --extra) extralib     : 指定与源程序一同编译的扩展库" << std::endl;
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
        NEED_EXTRA_LIB,
    } stat = NEED_INPUT_OR_OPTION;
    
    
    if (argc > 1) {
        
        bool flagStdin = false;
        std::vector<std::string> inFileNames;
        std::vector<std::string> extraFileNames;
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
                    
                } else if (stat == NEED_EXTRA_LIB) {
                    std::string extraFileName = "extra/"; extraFileName += s; extraFileName += ".frog";
                    bool result = FileSystem::fileExist(extraFileName);
                    if (!result) {
                        std::cerr << "~ 扩展库 "  << s  << " 不存在" << std::endl;
                        return 1;
                    }
                    extraFileNames.push_back(extraFileName);
                    stat = NEED_INPUT_OR_OPTION;
                }
                
                
            } else if (stat != NEED_OUTPUT) {
                if (s == "-o" || s == "--output") {
                    stat = NEED_OUTPUT;
            
                } else if (s == "-e" || s == "--extra") {
                    stat = NEED_EXTRA_LIB;
    
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
                    printUsage(FileSystem::path2FileName(argv[0]));
                    return 0;
                }
            } else {
                std::cerr << "~ 错误的参数用法, 可以附加 -h 或者 --help 参数以获取帮助" << std::endl;
                return 1;
            }
        }
    
        std::string outputContext;
        if (flagStdin) {
            UniSemParser up;
            outputContext = std::move(up.parse());
            
        } else {
            if (inFileNames.empty()) {
                std::cerr << "~ 输入文件列表为空" << std::endl;
                return 1;
            }
            UniSemParser up(inFileNames, extraFileNames);
            outputContext = std::move(up.parse());
        }
        
        if (flagStdout) {
            std::cout << outputContext << std::endl;
        } else {
            if (outFileName.empty()) {
                std::cerr << "~ 未指定输出文件" << std::endl;
                return 1;
            }
            if (!outputContext.empty()) {
                std::ofstream ofs(outFileName, std::ios::out);
                ofs << outputContext;
            }
        }
        
    } else {
        printUsage(FileSystem::path2FileName(argv[0]));
        return 1;
    }
    return 0;
}


#endif