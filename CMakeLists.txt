cmake_minimum_required(VERSION 3.5)
project(FlexibleComputerLanguage)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS -pthread)

include_directories(Core)
include_directories(Core/TestCases)
include_directories(FlexibleComputerLanguage)

find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)
include_directories(${LIBMONGOCXX_INCLUDE_DIR})
include_directories(${LIBBSONCXX_INCLUDE_DIR})
include_directories("/usr/local/include/mongocxx/v_noabi")
include_directories("/usr/local/include/bsoncxx/v_noabi")
include_directories("/usr/share/doc/libsasl2-dev")
include_directories("/usr/share/doc/libssl-dev")

add_executable(FlexibleComputerLanguage1
        Core/TestCases/TestCaseBase.cpp
        Core/TestCases/TestCaseBase.h
        Core/TestCases/TestCaseExecuter.cpp
        Core/TestCases/TestCaseExecuter.h
        Core/TestCases/TestCaseIncludes.h
        Core/TestCases/TestCompound1.cpp
        Core/TestCases/TestCompound1.h
        Core/TestCases/TestCustomStrFilterShorthand.cpp
        Core/TestCases/TestCustomStrFilterShorthand.h
        Core/TestCases/TestFilterSubtree.cpp
        Core/TestCases/TestFilterSubtree.h
        Core/TestCases/TestListFilter.cpp
        Core/TestCases/TestListFilter.h
        Core/TestCases/TestListGroupBy.cpp
        Core/TestCases/TestListGroupBy.h
        Core/TestCases/TestListInnerCount.cpp
        Core/TestCases/TestListInnerCount.h
        Core/TestCases/TestListInnerCountWithScript.cpp
        Core/TestCases/TestListInnerCountWithScript.h
        Core/TestCases/TestListSeek.h
        Core/TestCases/TestListSeek.cpp
        Core/TestCases/TestLoadFromCodeLibrary.cpp
        Core/TestCases/TestLoadFromCodeLibrary.h
        Core/TestCases/TestStringEnclosure.cpp
        Core/TestCases/TestStringEnclosure.h
        Core/TestCases/TestTestCase.cpp
        Core/TestCases/TestTestCase.h
        Core/AdditionalFunctions.h
        Core/Bool.cpp
        Core/Bool.h
        Core/Command.cpp
        Core/Command.h
        Core/CommonIncludes.h
        Core/CPlusPlusEntry.cpp
        Core/CPlusPlusEntry.h
        Core/DefFileReader.cpp
        Core/DefFileReader.h
        Core/DateTimeOperations.cpp
        Core/DateTimeOperations.h
        Core/DateTime.h
        Core/Defs.h
        Core/Entity.h
        Core/EntityList.cpp
        Core/EntityList.h
        Core/ExecutionContext.h
        Core/ExecutionTemplate.cpp
        Core/ExecutionTemplate.h
        Core/ExecutionTemplateList.cpp
        Core/ExecutionTemplateList.h
        Core/Int.h
        Core/ISpecialCommand.h
        Core/MemMan.cpp
        Core/MemMan.h
        Core/MetaData.h
        Core/Node.cpp
        Core/Node.h
        Core/NodeCustomValueShorthand.cpp
        Core/NodeCustomValueShorthand.h
        Core/Null.h
        Core/ProcessLog.h
        Core/ScriptReader.cpp
        Core/ScriptReader.h
        Core/SpecialCommandExecuter.cpp
        Core/SpecialCommandExecuter.h
        Core/Strings.h
        Core/StringOperations.cpp
        Core/StringOperations.h
        Core/Tests.cpp
        Core/Tests.h
        Core/TypeDefs.h
        Core/Utils.cpp
        Core/Utils.h
        Core/Value.h
        FlexibleComputerLanguage/json.hpp
        FlexibleComputerLanguage/main.cpp
        FlexibleComputerLanguage/ResultGenerator.h
        FlexibleComputerLanguage/ResultGenerator.cpp
        FlexibleComputerLanguage/NamedPipeOperations.h
        FlexibleComputerLanguage/NamedPipeOperations.cpp
        FlexibleComputerLanguage/OTPParser.h
        FlexibleComputerLanguage/OTPParser.cpp
        FlexibleComputerLanguage/easylogging++.h
        FlexibleComputerLanguage/easylogging++.cc
        FlexibleComputerLanguage/Logger.h)

target_link_libraries(FlexibleComputerLanguage1 ${LIBMONGOCXX_LIBRARIES})
target_link_libraries(FlexibleComputerLanguage1 ${LIBBSONCXX_LIBRARIES})


