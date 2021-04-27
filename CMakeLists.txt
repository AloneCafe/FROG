cmake_minimum_required(VERSION 3.19)
project(frog)

set(CMAKE_CXX_STANDARD 14)

include_directories(${SRCDIR})

#add_compile_definitions(_DEBUG)

set(EN_FROG_COMPILER fgc)
set(EN_FROG_ASSEMBLER fgas)
set(EN_FROG_VM fgvm)

set(SRCDIR src)

set(SRCS_FROG_COMPILER
        ${SRCDIR}/ILGenerator.hpp
        ${SRCDIR}/SyntaxTreeBasicParser.hpp
        ${SRCDIR}/SyntaxTreeErrorDriver.hpp
        ${SRCDIR}/SyntaxTreeParser.hpp
        ${SRCDIR}/CharScanner.cpp
        ${SRCDIR}/CharScanner.hpp
        ${SRCDIR}/CompilerMain.cpp
        ${SRCDIR}/ErrorDriver.cpp
        ${SRCDIR}/ErrorDriver.hpp
        ${SRCDIR}/ExprDefinition.hpp
        ${SRCDIR}/FrontParser.hpp
        ${SRCDIR}/GlobalFuncMap.hpp
        ${SRCDIR}/GUIDGenerator.hpp
        ${SRCDIR}/LineScanner.cpp
        ${SRCDIR}/LineScanner.hpp
        ${SRCDIR}/LittleAssist.cpp
        ${SRCDIR}/LittleAssist.hpp
        ${SRCDIR}/LocalCodeBinder.hpp
        ${SRCDIR}/LocalVarDefinition.hpp
        ${SRCDIR}/ParserConfigure.hpp
        ${SRCDIR}/PreAllocatedEntry.hpp
        ${SRCDIR}/SemanticErrorDriver.hpp
        ${SRCDIR}/SemanticParser.hpp
        ${SRCDIR}/Statements.cpp
        ${SRCDIR}/Statements.hpp
        ${SRCDIR}/Structure.hpp
        ${SRCDIR}/StructureBase.hpp
        ${SRCDIR}/Token.hpp
        ${SRCDIR}/TokenDefinition.hpp
        ${SRCDIR}/TokenScanner.cpp
        ${SRCDIR}/TokenScanner.hpp
        ${SRCDIR}/UnitedSemanticParser.hpp
        ${SRCDIR}/UnitedTokenizer.cpp
        ${SRCDIR}/UnitedTokenizer.hpp
        ${SRCDIR}/UTFSequence.cpp
        ${SRCDIR}/UTFSequence.hpp
        ${SRCDIR}/ILGenerator.cpp
        ${SRCDIR}/ExprDefinition.cpp src/FrontParser.cpp src/GlobalFuncMap.cpp src/GUIDGenerator.cpp)

set(SRCS_FROG_ASSEMBLER
        ${SRCDIR}/ILParser.hpp
        ${SRCDIR}/AssemblerMain.cpp
        ${SRCDIR}/ILGenerator.hpp
        ${SRCDIR}/CharScanner.cpp
        ${SRCDIR}/CharScanner.hpp
        ${SRCDIR}/LineScanner.cpp
        ${SRCDIR}/LineScanner.hpp
        ${SRCDIR}/LittleAssist.cpp
        ${SRCDIR}/LittleAssist.hpp
        ${SRCDIR}/Token.hpp
        ${SRCDIR}/TokenDefinition.hpp
        ${SRCDIR}/TokenScanner.cpp
        ${SRCDIR}/TokenScanner.hpp
        ${SRCDIR}/UnitedTokenizer.cpp
        ${SRCDIR}/UnitedTokenizer.hpp
        ${SRCDIR}/ErrorDriver.cpp
        ${SRCDIR}/ErrorDriver.hpp
        ${SRCDIR}/UTFSequence.cpp
        ${SRCDIR}/UTFSequence.hpp
        ${SRCDIR}/UnitedILParser.hpp
        ${SRCDIR}/ByteCodeGenerator.hpp
        ${SRCDIR}/ByteCodeRelocator.hpp
        ${SRCDIR}/ILParser.cpp
        ${SRCDIR}/ByteCodeGenerator.cpp
        ${SRCDIR}/ByteCodeRelocator.cpp)

add_executable(${EN_FROG_COMPILER} ${SRCS_FROG_COMPILER})
add_executable(${EN_FROG_ASSEMBLER} ${SRCS_FROG_ASSEMBLER})

file(COPY extra/SimpleIO.frog DESTINATION ${CMAKE_BINARY_DIR}/extra)