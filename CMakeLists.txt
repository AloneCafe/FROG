cmake_minimum_required(VERSION 3.19)
project(frog)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

set(EN_FROG_COMPILER fgc)
set(EN_FROG_ASSEMBLER fgas)
set(EN_FROG_VM fgvm)

set(SRCS_FROG_COMPILER
        src/AssemblyMaker.hpp
        src/ASTBasicParser.hpp
        src/ASTErrorDriver.hpp
        src/ASTParser.hpp
        src/CharScanner.cpp
        src/CharScanner.hpp
        src/CompilerMain.cpp
        src/ErrorDriver.cpp
        src/ErrorDriver.hpp
        src/ExprDefinition.hpp
        src/FrontParser.hpp
        src/GlobalFuncMap.hpp
        src/GUIDGenerator.hpp
        src/LineScanner.cpp
        src/LineScanner.hpp
        src/LittleAssist.cpp
        src/LittleAssist.hpp
        src/LocalCodeBinder.hpp
        src/LocalVarDefinition.hpp
        src/ParserConfigure.hpp
        src/PreAllocatedEntry.hpp
        src/SemanticErrorDriver.hpp
        src/SemanticParser.hpp
        src/Statements.cpp
        src/Statements.hpp
        src/Structure.hpp
        src/StructureBase.hpp
        src/Token.hpp
        src/TokenDefinition.hpp
        src/TokenScanner.cpp
        src/TokenScanner.hpp
        src/UnitedParser.hpp
        src/UnitedTokenizer.cpp
        src/UnitedTokenizer.hpp
        src/UTFSequence.cpp
        src/UTFSequence.hpp
)

set(SRCS_FROG_ASSEMBLER
        src/AssemblyParser.hpp
        src/AssemblerMain.cpp
        src/AssemblyMaker.hpp
        src/ASTBasicParser.hpp
        src/ASTErrorDriver.hpp
        src/ASTParser.hpp
        src/CharScanner.cpp
        src/CharScanner.hpp
        src/ErrorDriver.cpp
        src/ErrorDriver.hpp
        src/ExprDefinition.hpp
        src/FrontParser.hpp
        src/GlobalFuncMap.hpp
        src/GUIDGenerator.hpp
        src/LineScanner.cpp
        src/LineScanner.hpp
        src/LittleAssist.cpp
        src/LittleAssist.hpp
        src/LocalCodeBinder.hpp
        src/LocalVarDefinition.hpp
        src/ParserConfigure.hpp
        src/PreAllocatedEntry.hpp
        src/SemanticErrorDriver.hpp
        src/SemanticParser.hpp
        src/Statements.cpp
        src/Statements.hpp
        src/Structure.hpp
        src/StructureBase.hpp
        src/Token.hpp
        src/TokenDefinition.hpp
        src/TokenScanner.cpp
        src/TokenScanner.hpp
        src/UnitedParser.hpp
        src/UnitedTokenizer.cpp
        src/UnitedTokenizer.hpp
        src/UTFSequence.cpp
        src/UTFSequence.hpp
)

add_executable(${EN_FROG_COMPILER} ${SRCS_FROG_COMPILER})
add_executable(${EN_FROG_ASSEMBLER} ${SRCS_FROG_ASSEMBLER})