#  Copyright (c) 2014-2016 olibre (olibre@Lmap.org)
#
#  This file is part of the project pbart (Protocol Buffers At RunTime)
#
#  pbart is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Affero General Public License (AGPL)
#  version 3 or (at your option) any later version.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with pbart (maybe in file LICENSE).
#  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.4)
# requires v3.1 because use target_compile_features()
# requires v3.4 because use FindJava.cmake/UseJava/create_javah()

project(pbart)


#############
## OPTIONS ##
#############

# Usage: cmake -DSANITIZE=ON
option(SANITIZE "Sanity check" OFF)

# Static code analysis
# Generate file 'compile_commands.json' for clang-check
# Usage: awk -F: '/"file"/{print $2 }' build/compile_commands.json | xargs clang-check -fixit -p build
#option(CMAKE_EXPORT_COMPILE_COMMANDS "Produce compile_commands.json for clang-check" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default: -march=corei7
# Usages:  cmake .. -DMARCH=native (if native => convert to real cpu-type)
#          cmake .. -DMARCH=       (disable flag -march)
set( MARCH "corei7"  CACHE STRING "Control flag -march" )

# Usage: cmake -DOPTIM=-Ofast
set( OPTIM "" CACHE STRING "Control flags -Ox" )


######################
## CMAKE_BUILD_TYPE ##
######################

# Release  = Debug info g3 + disable assert
# Coverage = Debug info g3 + disable assert + Coverage
# Debug    = Debug info g3 + enable  assert + -D_GLIBCXX_DEBUG_PEDANTIC

# Default = Release
if( NOT CMAKE_BUILD_TYPE )
    message(STATUS "** Setting build type to 'Release' as none was specified.")
    set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE )
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "Coverage") # For 'cmake -i' and 'cmake-gui'
endif()

# Coverage: check tools gcov/lcov/... and set flag --coverage
# HTML report: gcovr --root . --html --html-details --output coverage.html --exclude-unreachable-branches --print-summary
if( CMAKE_BUILD_TYPE STREQUAL "Coverage" )
    add_definitions( -DNDEBUG )  # Disable assert to avoid code coverage bias
    if( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
        set (CoverageCmplFlag --coverage) #add_compile_options( --coverage )
        set (CoverageLinkFlag --coverage) #link_libraries(      --coverage )
        find_program (gcov gcov)
        if (NOT gcov)
            message(WARNING "** Cannot find gcov => Build may fail...")
        endif()
        find_program (gcovr gcovr)
        if (gcovr)
            message (STATUS "** Found command 'gcovr' => Use target 'gcovr' to generate code coverage report (HTML)")
            add_custom_target (gcovr
                COMMAND ${gcovr} --root             ${CMAKE_SOURCE_DIR}
                                 --exclude          ${CMAKE_SOURCE_DIR}/ext
                                 --exclude          ${CMAKE_SOURCE_DIR}/cpp/unit-test
                                 --exclude          ${CMAKE_BINARY_DIR}
                                 --object-directory ${CMAKE_BINARY_DIR}
                                 --output           ${CMAKE_BINARY_DIR}/gcovr.html
                                 --html --html-details --sort-uncovered
                                 --print-summary --exclude-unreachable-branches
            #   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMAND echo "Coverage report: firefox ${CMAKE_BINARY_DIR}/gcovr.html"
            )
        else()
            message(WARNING "** Cannot find command 'gcovr' => 'apt install gcovr' for code coverage report (HTML)")
        endif()
        find_program (lcov    lcov   )
        find_program (genhtml genhtml)
        if (lcov AND genhtml)
            message (STATUS "** Found commands 'lcov' and 'genhtml' => Use target 'lcov' to generate code coverage report (HTML)")
            add_custom_target (lcov
                COMMAND ${lcov}    --capture      --directory     ${CMAKE_BINARY_DIR}
                                   --no-external  --output-file   ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-all.info
                                   --no-checksum --base-directory ${CMAKE_SOURCE_DIR}/cpp
                                   --rc    lcov_branch_coverage=1 --quiet
                COMMAND ${lcov}    --quiet --extract              ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-all.info
                                                                  ${CMAKE_SOURCE_DIR}/cpp/[isj]*/*/*
                                   --no-checksum --output-file    ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.info
                                   --rc    lcov_branch_coverage=1
                COMMAND ${genhtml} --rc genhtml_branch_coverage=1 ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.info
                                   --output-directory             ${CMAKE_BINARY_DIR}/lcov
                                   --highlight --legend --quiet
                COMMAND echo "Coverage report: firefox ${CMAKE_BINARY_DIR}/lcov/index.html"
            )
        else()
            message(WARNING "** Cannot find both commands 'lcov' and 'genhtml' => 'apt install lcov' for code coverage report (HTML)")
        endif()
    else()
        message(WARNING "** Coverage not yet implemented for your compiler '${CMAKE_CXX_COMPILER_ID}' (only support GNU and Clang)")
    endif()


endif()

# Debug: Enable std::debug
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    add_definitions(-D_GLIBCXX_DEBUG_PEDANTIC)
endif()


#######################
## COMPILATION FLAGS ##
#######################

# Flag -march
if( MARCH STREQUAL "native" )
    if( CMAKE_COMPILER_IS_GNUCXX )
        EXECUTE_PROCESS( COMMAND ${CMAKE_CXX_COMPILER} -march=native -Q --help=target COMMAND awk "/-march=/{ printf $2}" OUTPUT_VARIABLE march_native )
        message(STATUS "** MARCH is native and compiler is GNU => Detected processor '${march_native}' => -march=${march_native}")
        add_compile_options( -march=${march_native} )
    else()
        message(STATUS "** MARCH is native and compiler is *not* GNU => -march=native")
        add_compile_options( -march=native )
    endif()
elseif( MARCH )
    message(STATUS "** MARCH is not native => -march=${MARCH}")
    add_compile_options( -march=${MARCH} )
else()
    message(STATUS "** MARCH is empty => Do not set flag -march")
endif()

# Flags -O0/-Og/-O1/-O2/-O3/-Ofast
if( OPTIM )
    message(STATUS "** OPTIM is set to ${OPTIM} => Add it as compilation flag")
    add_compile_options( ${OPTIM} )
endif()

# Instrument code for run-time analysis
if(SANITIZE)
    add_compile_options(-fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=signed-integer-overflow -fsanitize=shift -fsanitize=integer-divide-by-zero -fsanitize=null)
endif()

# Full debug info for all CMAKE_BUILD_TYPE
add_compile_options( -g3 -ggdb3 )  # -g3 -> include also the MACRO definitions

# Enable many wanings
if( CMAKE_COMPILER_IS_GNUCXX )
    set( ManyWarns -Wall -Wextra -Wswitch-enum -Wno-ignored-qualifiers -pedantic -pedantic-errors -Wconversion -Wno-unused-but-set-variable -Wno-unused-variable -Wno-unused-function -Wno-conversion) #-Weffc++ -Wpadded)
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    set( ManyWarns -Weverything)
endif()

# Speed up build using pipes (rather than temporary files) for communication between the various GCC stages
add_compile_options( -pipe )
link_libraries(      -pipe )


###########
## TOOLS ##
###########

# Speed up build if ccache installed
find_program (ccache ccache)
if (ccache)
    message(STATUS "** Command 'ccache' detected => Will use 'ccache' to speed up compilation and link" )
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK    ccache)
    # TODO(olibre): Use "CMAKE_{C,CXX}_COMPILER_LAUNCHER=ccmake" with cmake-v3.4
endif()

# Use tool "include-what-you-use" if available
find_program (iwyu NAMES iwyu include-what-you-use)
if (iwyu)
    set(CXX_INCLUDE_WHAT_YOU_USE ${iwyu})
endif()


##########################
## EXTENAL DEPENDENCIES ##
##########################

find_package(Protobuf REQUIRED) # sudo apt install libprotobuf-dev
add_library(               pbart_Protobuf INTERFACE)
target_include_directories(pbart_Protobuf INTERFACE ${PROTOBUF_INCLUDE_DIRS})
target_link_libraries(     pbart_Protobuf INTERFACE ${PROTOBUF_LIBRARIES})

find_package(Boost REQUIRED) # sudo apt install libboost-dev
add_library(               pbart_Boost INTERFACE)
target_include_directories(pbart_Boost INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(     pbart_Boost INTERFACE ${Boost_LIBRARIES})

add_subdirectory(ext/tinyxml2)
add_library(               pbart_TinyXML INTERFACE)
target_include_directories(pbart_TinyXML INTERFACE ext/tinyxml2)
target_link_libraries(     pbart_TinyXML INTERFACE tinyxml2)

if( CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    set(make "$(MAKE)") # Allow passing arguments from parent make command
else()
    set(make make)      # Use 'make' because $(MAKE) fails using ninja
endif()

include(ExternalProject)
ExternalProject_Add(   cppunit
#   DEPENDS            Threads::Threads ZLIB::ZLIB
    SOURCE_DIR         ${CMAKE_CURRENT_LIST_DIR}/ext/cppunit
    BUILD_IN_SOURCE    1
    UPDATE_COMMAND     autoconf || autoreconf --install
    CONFIGURE_COMMAND  ${CMAKE_CURRENT_LIST_DIR}/ext/cppunit/configure
                       --prefix=${CMAKE_BINARY_DIR}
                       CC=${CMAKE_C_COMPILER}
                       CXX=${CMAKE_CXX_COMPILER}
    BUILD_COMMAND      ${make}
    INSTALL_COMMAND    ${make} install
    BUILD_BYPRODUCTS   ${CMAKE_BINARY_DIR}/lib/libcppunit.so  # Required for ninja
)

add_library(           pbart_CppUnit SHARED IMPORTED)
add_dependencies(      pbart_CppUnit cppunit)
set_target_properties( pbart_CppUnit PROPERTIES
                       IMPORTED_LOCATION             ${CMAKE_BINARY_DIR}/lib/libcppunit.so
                       INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/ext/cppunit/include)


######################
## C++ CORE LIBRARY ##
######################

file(GLOB pbart_header_files cpp/include/pbart/*.h)
file(GLOB pbart_source_files cpp/src/pbart/*.cpp)
add_library(               pbart ${pbart_header_files} ${pbart_source_files})
target_include_directories(pbart PUBLIC cpp/include)
target_link_libraries(     pbart PUBLIC pbart_Protobuf pbart_Boost PRIVATE pbart_TinyXML)
target_compile_features(   pbart PUBLIC cxx_long_long_type)
target_compile_options(    pbart PUBLIC ${CoverageCmplFlag} PRIVATE ${ManyWarns})
target_link_libraries(     pbart PUBLIC ${CoverageLinkFlag} PRIVATE ${ManyWarns})


######################
##  C++ UNIT TESTS  ##
######################

enable_testing()

add_library(TestMain cpp/unit-test/pbart/Main.cpp)
target_link_libraries(TestMain pbart pbart_CppUnit)

protobuf_generate_cpp(DicoBigSrc DicoBigHdr cpp/unit-test/data/dico-Big.proto)
add_executable(            TestBig cpp/unit-test/pbart/TestBig.cpp ${DicoBigSrc} ${DicoBigHdr})
target_compile_definitions(TestBig PRIVATE DICO_BIG_PROTOBUF_HEADER="${DicoBigHdr}")
target_link_libraries(     TestBig TestMain)
add_test(                  TestBig TestBig          ${CMAKE_SOURCE_DIR}/cpp/unit-test/data/ )

add_executable(       TestCodec cpp/unit-test/pbart/TestCodec.cpp)
target_link_libraries(TestCodec TestMain)
add_test(             TestCodec TestCodec           ${CMAKE_SOURCE_DIR}/cpp/unit-test/data/ )

add_executable(       TestCodecRepeated cpp/unit-test/pbart/TestCodecRepeated.cpp)
target_link_libraries(TestCodecRepeated TestMain)
add_test(             TestCodecRepeated TestCodec   ${CMAKE_SOURCE_DIR}/cpp/unit-test/data/ )

#add_executable(       TestDicoAsMsg cpp/unit-test/pbart/TestDicoAsMsg.cpp)
#target_link_libraries(TestDicoAsMsg TestMain)
#add_test(             TestDicoAsMsg TestDicoAsMsg  ${CMAKE_SOURCE_DIR}/cpp/unit-test/data/ )

add_executable(       TestDico          cpp/unit-test/pbart/TestDico.cpp)
target_link_libraries(TestDico          TestMain)
add_test(             TestDico          TestDico    ${CMAKE_SOURCE_DIR}/cpp/unit-test/data/ )

add_executable(       TestMessage       cpp/unit-test/pbart/TestMessage.cpp)
target_link_libraries(TestMessage       TestMain)
add_test(             TestMessage       TestMessage ${CMAKE_SOURCE_DIR}/cpp/unit-test/data/ )

add_executable(       TestPerf          cpp/unit-test/pbart/TestPerf.cpp)
target_link_libraries(TestPerf          TestMain)
add_test(             TestPerf          TestPerf    ${CMAKE_SOURCE_DIR}/cpp/unit-test/data/ )

add_executable(       TestVariant       cpp/unit-test/pbart/TestVariant.cpp)
target_link_libraries(TestVariant       TestMain)
add_test(             TestVariant       TestVariant ${CMAKE_SOURCE_DIR}/cpp/unit-test/data/ )


######################
##       JNI        ##
######################

find_package(Java COMPONENTS Development)
include (UseJava)
find_package(JNI)
create_javah(
  TARGET          pbart-jni-serializer-header
  GENERATED_FILES PbartJniSerializerHeader
  CLASSES         org.Lmap.pbart.Serializer
  CLASSPATH       ${PROJECT_SOURCE_DIR}/java/src/main/java
)
create_javah(
  TARGET          pbart-jni-message-header
  GENERATED_FILES PbartJniMessageHeader
  CLASSES         org.Lmap.pbart.Message
  CLASSPATH       ${PROJECT_SOURCE_DIR}/java/src/main/java
)

add_library(               pbart-jni cpp/src/jni/org_Lmap_pbart_Message.cpp
                                     cpp/src/jni/org_Lmap_pbart_Serializer.cpp
                                     cpp/src/jni/cpp-java-conversion-helpers.cpp
                                     cpp/src/jni/cpp-java-conversion-helpers.h
                                     ${PbartJniSerializerHeader}
                                     ${PbartJniMessageHeader}
)
add_dependencies(          pbart-jni pbart-jni-serializer-header pbart-jni-message-header)
target_include_directories(pbart-jni PUBLIC ${JNI_INCLUDE_DIRS})
target_compile_options(    pbart-jni PRIVATE ${ManyWarns} ${CoverageCmplFlag})
target_link_libraries(     pbart-jni PRIVATE ${ManyWarns} ${CoverageLinkFlag}
                                     PUBLIC  pbart)
target_compile_definitions(pbart-jni PRIVATE GENERATED_JNI_SERIALIZER_HEADER="${PbartJniSerializerHeader}"
                                             GENERATED_JNI_MESSAGE_HEADER="${PbartJniMessageHeader}")


######################
##       JAVA       ##
######################

file(GLOB PbartJavaFiles java/src/main/java/org/Lmap/pbart/*.java)
add_jar(pbart-java SOURCES ${PbartJavaFiles}
                   ENTRY_POINT java/src/main/java/org/Lmap/pbart
                   OUTPUT_NAME pbart
)
