#include "TokenScanner.hpp"
#include "UnitedILParser.hpp"

static const char * pLogoAS =
        " ______                                                 _     _           \n"
        "|  ____|                   /\\                          | |   | |          \n"
        "| |__ _ __ ___   __ _     /  \\   ___ ___  ___ _ __ ___ | |__ | | ___ _ __ \n"
        "|  __| '__/ _ \\ / _` |   / /\\ \\ / __/ __|/ _ \\ '_ ` _ \\| '_ \\| |/ _ \\ '__|\n"
        "| |  | | | (_) | (_| |  / ____ \\\\__ \\__ \\  __/ | | | | | |_) | |  __/ |   \n"
        "|_|  |_|  \\___/ \\__, | /_/    \\_\\___/___/\\___|_| |_| |_|_.__/|_|\\___|_|   \n"
        "                 __/ |                                                    \n"
        "                |___/                                                     \n"
;

void printUsage(const std::string path) {
    std::cout << pLogoAS << std::endl;
    std::cout << "========== 欢迎使用 FROG 汇编器 (FROG Assembler) ==========" << std::endl;
    std::cout << std::endl;
    std::cout << "- USAGE: " << path << " [OPTIONS]" << std::endl;
    std::cout << "- OPTIONS: " << std::endl;
    std::cout << "    infile                      : 汇编代码文件" << std::endl;
    std::cout << "    (-o | --output) outfile     : 生成字节码" << std::endl;
    std::cout << "    (-I | --stdin)              : 从标准输入读取汇编代码" << std::endl;
    std::cout << "    (-O | --stdout)             : 将结果打印至标准输出 (十六进制文本序列)" << std::endl;
    std::cout << "    -h                          : 显示帮助" << std::endl;
    std::cout << std::endl;
    std::cout << "+ EXAMPLE: " << path << " test.fas -o output.fvm" << std::endl;
    std::cout << "+ EXAMPLE: " << path << " foo.fas -I -O"  << std::endl;
}


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
                    if (inFileNames.size() > 1) {
                        std::cerr << "~ 汇编器只能接受一个汇编代码文件" << std::endl;
                        return 1;
                    }
                    
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
                        std::cerr << "~ 显式指定标准输入, 已忽略汇编代码文件" << std::endl;
                    }
                    
                } else if (s == "-O" || s == "--stdout") {
                    flagStdout = true;
                    if (!outFileName.empty()) {
                        std::cerr << "~ 显式指定标准输出, 将不会生成字节码文件" << std::endl;
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
    
        std::vector<char> outputByteCode;
        if (flagStdin) {
            UniILParser up;
            if (up.parse())
                outputByteCode = ByteCodeGenerator::make(
                        up.getBytesStatic(),
                        up.getBytesFuncs(),
                        up.runnable(),
                        up.getEntryPoint());
            
        } else {
            if (inFileNames.empty()) {
                std::cerr << "~ 输入文件为空" << std::endl;
                return 1;
            }
            UniILParser up(inFileNames[0]);
            if (up.parse())
                outputByteCode = ByteCodeGenerator::make(
                        up.getBytesStatic(),
                        up.getBytesFuncs(),
                        up.runnable(),
                        up.getEntryPoint());
        }
        
        if (flagStdout) {
            ByteCodeHexPrinter::print(outputByteCode);
        } else {
            if (outFileName.empty()) {
                std::cerr << "~ 未指定输出文件" << std::endl;
                return 1;
            }
            std::ofstream ofs(outFileName, std::ios::out | std::ios::binary);
            for (const char & b : outputByteCode)
                ofs << b;
        }
        
    } else {
        printUsage(FileSystem::path2FileName(argv[0]));
        return 1;
    }
    return 0;
}

