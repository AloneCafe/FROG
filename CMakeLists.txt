cmake_minimum_required(VERSION 3.0)
project(frog)

set(CMAKE_CXX_STANDARD 11)

include_directories(${SRCDIR})

include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
    message(FATAL_ERROR "Detected big-endian, FAILED")
else()
    message(STATUS "Check little-endian, OK")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CXX_FLAGS -Wall -Wextra -O2)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CXX_FLAGS -Wall -Wextra -O2)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CXX_FLAGS /utf-8)
else ()
    message(FATAL_ERROR "Unsupported C++ compiler")
endif ()

set(EN_FROG_COMPILER fc)
set(EN_FROG_ASSEMBLER fas)
set(EN_FROG_VM fvm)

add_compile_options(${CXX_FLAGS})

set(SRCDIR src)

set(SRCS_FROG_COMPILER
        ${SRCDIR}/CompilerMain.cpp
        ${SRCDIR}/ILGenerator.cpp
        ${SRCDIR}/ILGenerator.hpp
        ${SRCDIR}/SyntaxTreeBasicParser.cpp
        ${SRCDIR}/SyntaxTreeBasicParser.hpp
        ${SRCDIR}/SyntaxTreeErrorDriver.cpp
        ${SRCDIR}/SyntaxTreeErrorDriver.hpp
        ${SRCDIR}/SyntaxTreeParser.cpp
        ${SRCDIR}/SyntaxTreeParser.hpp
        ${SRCDIR}/CharScanner.cpp
        ${SRCDIR}/CharScanner.hpp
        ${SRCDIR}/ErrorDriver.cpp
        ${SRCDIR}/ErrorDriver.hpp
        ${SRCDIR}/ExprDefinition.cpp
        ${SRCDIR}/ExprDefinition.hpp
        ${SRCDIR}/FrontParser.cpp
        ${SRCDIR}/FrontParser.hpp
        ${SRCDIR}/GlobalFuncMap.cpp
        ${SRCDIR}/GlobalFuncMap.hpp
        ${SRCDIR}/GUIDGenerator.cpp
        ${SRCDIR}/GUIDGenerator.hpp
        ${SRCDIR}/LineScanner.cpp
        ${SRCDIR}/LineScanner.hpp
        ${SRCDIR}/LittleAssist.cpp
        ${SRCDIR}/LittleAssist.hpp
        ${SRCDIR}/LocalCodeBinder.cpp
        ${SRCDIR}/LocalCodeBinder.hpp
        ${SRCDIR}/LocalVarDefinition.cpp
        ${SRCDIR}/LocalVarDefinition.hpp
        ${SRCDIR}/ParserConfigure.cpp
        ${SRCDIR}/ParserConfigure.hpp
        ${SRCDIR}/PreAllocatedEntry.cpp
        ${SRCDIR}/PreAllocatedEntry.hpp
        ${SRCDIR}/SemanticErrorDriver.cpp
        ${SRCDIR}/SemanticErrorDriver.hpp
        ${SRCDIR}/SemanticParser.cpp
        ${SRCDIR}/SemanticParser.hpp
        ${SRCDIR}/Statements.cpp
        ${SRCDIR}/Statements.hpp
        ${SRCDIR}/Structure.cpp
        ${SRCDIR}/Structure.hpp
        ${SRCDIR}/StructureBase.cpp
        ${SRCDIR}/StructureBase.hpp
        ${SRCDIR}/Token.cpp
        ${SRCDIR}/Token.hpp
        ${SRCDIR}/TokenDefinition.cpp
        ${SRCDIR}/TokenDefinition.hpp
        ${SRCDIR}/TokenScanner.cpp
        ${SRCDIR}/TokenScanner.hpp
        ${SRCDIR}/UnitedSemanticParser.cpp
        ${SRCDIR}/UnitedSemanticParser.hpp
        ${SRCDIR}/UnitedTokenizer.cpp
        ${SRCDIR}/UnitedTokenizer.hpp
        ${SRCDIR}/UTFSequence.cpp
        ${SRCDIR}/UTFSequence.hpp)

set(SRCS_FROG_ASSEMBLER
        ${SRCDIR}/AssemblerMain.cpp
        ${SRCDIR}/ILParser.cpp
        ${SRCDIR}/ILParser.hpp
        ${SRCDIR}/CharScanner.cpp
        ${SRCDIR}/CharScanner.hpp
        ${SRCDIR}/LineScanner.cpp
        ${SRCDIR}/LineScanner.hpp
        ${SRCDIR}/LittleAssist.cpp
        ${SRCDIR}/LittleAssist.hpp
        ${SRCDIR}/Token.cpp
        ${SRCDIR}/Token.hpp
        ${SRCDIR}/TokenDefinition.cpp
        ${SRCDIR}/TokenDefinition.hpp
        ${SRCDIR}/TokenScanner.cpp
        ${SRCDIR}/TokenScanner.hpp
        ${SRCDIR}/UnitedTokenizer.cpp
        ${SRCDIR}/UnitedTokenizer.hpp
        ${SRCDIR}/ErrorDriver.cpp
        ${SRCDIR}/ErrorDriver.hpp
        ${SRCDIR}/UTFSequence.cpp
        ${SRCDIR}/UTFSequence.hpp
        ${SRCDIR}/UnitedILParser.cpp
        ${SRCDIR}/UnitedILParser.hpp
        ${SRCDIR}/ByteCodeGenerator.cpp
        ${SRCDIR}/ByteCodeGenerator.hpp
        ${SRCDIR}/ByteCodeRelocator.cpp
        ${SRCDIR}/ByteCodeRelocator.hpp)

set(SRCS_FROG_VM
        ${SRCDIR}/VirtualMachineMain.cpp
        ${SRCDIR}/LittleAssist.cpp
        ${SRCDIR}/LittleAssist.hpp
        ${SRCDIR}/UTFSequence.cpp
        ${SRCDIR}/UTFSequence.hpp
        ${SRCDIR}/FakeCPU.cpp
        ${SRCDIR}/FakeCPU.hpp
        ${SRCDIR}/FakeOPROM.cpp
        ${SRCDIR}/FakeOPROM.hpp
        ${SRCDIR}/VMException.cpp
        ${SRCDIR}/VMException.hpp
        ${SRCDIR}/FakeOPStack.cpp
        ${SRCDIR}/FakeOPStack.hpp
        ${SRCDIR}/FakeFNStack.cpp
        ${SRCDIR}/FakeFNStack.hpp
        ${SRCDIR}/FakeScalarRAM.cpp
        ${SRCDIR}/FakeScalarRAM.hpp
        ${SRCDIR}/FakeVectorRAM.cpp
        ${SRCDIR}/FakeVectorRAM.hpp
        ${SRCDIR}/GarbageCollector.cpp
        ${SRCDIR}/GarbageCollector.hpp
        ${SRCDIR}/TSMapWarpper.cpp
        ${SRCDIR}/TSMapWarpper.hpp)

add_executable(${EN_FROG_COMPILER} ${SRCS_FROG_COMPILER})
add_executable(${EN_FROG_ASSEMBLER} ${SRCS_FROG_ASSEMBLER})
add_executable(${EN_FROG_VM} ${SRCS_FROG_VM})

target_compile_options(${EN_FROG_COMPILER} INTERFACE ${CXX_FLAGS})
target_compile_options(${EN_FROG_ASSEMBLER} INTERFACE ${CXX_FLAGS})
target_compile_options(${EN_FROG_VM} INTERFACE ${CXX_FLAGS})

file(COPY extra/SimpleIO.frog DESTINATION ${CMAKE_BINARY_DIR}/extra)