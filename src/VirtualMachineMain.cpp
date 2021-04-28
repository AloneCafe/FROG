#include "TokenScanner.hpp"
#include "UnitedILParser.hpp"

static const char * pLogoVM =
        " ______                __      ____  __\n"
        "|  ____|               \\ \\    / /  \\/  |\n"
        "| |__ _ __ ___   __ _   \\ \\  / /| \\  / |\n"
        "|  __| '__/ _ \\ / _` |   \\ \\/ / | |\\/| |\n"
        "| |  | | | (_) | (_| |    \\  /  | |  | |\n"
        "|_|  |_|  \\___/ \\__, |     \\/   |_|  |_|\n"
        "                 __/ |\n"
        "                |___/\n"
;

void printUsage(const std::string & path) {
    std::cout << pLogoVM << std::endl;
    std::cout << "========== 欢迎使用 FROG 虚拟机 (FROG VM) ==========" << std::endl;
    std::cout << std::endl;
    std::cout << "- USAGE: " << path << " [OPTIONS]" << std::endl;
    std::cout << "- OPTIONS: " << std::endl;
    std::cout << "    bytecode_file           : 字节码文件" << std::endl;
    std::cout << "    (-v | --verbose)        : 输出字节码执行轨迹" << std::endl;
    std::cout << "    (-s | --step)           : 单步调试模式" << std::endl;
    std::cout << "    -h                      : 显示帮助" << std::endl;
    std::cout << std::endl;
    std::cout << "+ EXAMPLE: " << path << " -v test.fvm" << std::endl;
    std::cout << "+ EXAMPLE: " << path << " -s test.fvm"  << std::endl;
}


int main(int argc, const char * argv[]) {
    enum {
        NEED_INPUT_OR_OPTION,
        OTHER,
    } stat = NEED_INPUT_OR_OPTION;
    
    bool flagVerbose = false;
    bool flagStep = false;
    
    if (argc > 1) {
        std::string inFileName;
        
        for (int i = 1; i < argc; ++i) {
            std::string s(argv[i]);
            if (!s.empty() && s[0] != '-') {
                if (stat == NEED_INPUT_OR_OPTION) {
                    inFileName = s;
                    
                } else {
                    stat = NEED_INPUT_OR_OPTION;
                }
                
            } else {
                if (s == "-v" || s == "--verbose") {
                    flagVerbose = true;
                    
                } else if (s == "-s" || s == "--step") {
                    flagStep = true;
                    
                } else if (s == "-h" || s == "--help") {
                    printUsage(FileSystem::path2FileName(argv[0]));
                    return 0;
                    
                } else {
                    std::cerr << "~ 错误的参数用法, 可以附加 -h 或者 --help 参数以获取帮助" << std::endl;
                    return 1;
                }
            }
        }
    
        std::ifstream ifs(inFileName, std::ios::in | std::ios::binary);
        if (flagStep) {
        
        } else if (flagVerbose) {
        
        } else {
        
        }
        
        
    } else {
        printUsage(FileSystem::path2FileName(argv[0]));
        return 1;
    }
    
    return 0;
}

