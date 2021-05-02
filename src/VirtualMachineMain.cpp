#include "TokenScanner.hpp"
#include "UnitedILParser.hpp"
#include "FakeCPU.hpp"

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
        
        ByteCodeHeader bch;
        char ch;
        for (uint32_t i = 0; i < sizeof(bch); ++i) {
            ch = ifs.get();
            if (ch == EOF) {
                std::cerr << "~ 错误的字节码文件格式" << std::endl;
                return 1;
            }
            reinterpret_cast<char *>(&bch)[i] = ch;
        }
        
        if (!bch._magics[3]) {
            std::cerr << "~ 该字节码文件不可运行 (无入口点)" << std::endl;
            return 1;
        }
        uint32_t startAddr = bch._raEntryPoint;
        
        std::vector<char> staticBytes;
        std::vector<char> funcsBytes;
        
        ifs.seekg(bch._offsetStatic, std::ios::beg);
        for (uint32_t i = 0; i < bch._sizeStatic; ++i) {
            ch = ifs.get();
            staticBytes.push_back(ch);
        }
    
        ifs.seekg(bch._offsetFuncs, std::ios::beg);
        for (uint32_t i = 0; i < bch._sizeFuncs; ++i) {
            ch = ifs.get();
            funcsBytes.push_back(ch);
        }
        
        
        FakeFNStack fnStack;
        FakeOPStack opStack;
        FakeOPROM opROM(staticBytes, funcsBytes);
        FakeScalarRAM sRAM;
        FakeVectorRAM vRAM;
        
        FakeCPU cpu;
        cpu.attachFNStack(&fnStack);
        cpu.attachOPStack(&opStack);
        cpu.attachOPROM(&opROM);
        cpu.attachScalarRAM(&sRAM);
        cpu.attachVectorRAM(&vRAM);
        
        if (!staticBytes.empty()) {
            int32_t retStatic = cpu.runStatic(flagVerbose, flagStep);
            if (!retStatic) {
                std::cerr << "~ 该字节码文件静态初始化失败" << std::endl;
                return retStatic;
            }
        }
        
        if (!funcsBytes.empty()) {
            bool retFuncs = cpu.runFuncs(flagVerbose, flagStep, startAddr);
            if (!retFuncs) {
                return retFuncs;
            }
        }
        
        return 0;
        
    } else {
        printUsage(FileSystem::path2FileName(argv[0]));
        return 1;
    }
    
    return 0;
}

