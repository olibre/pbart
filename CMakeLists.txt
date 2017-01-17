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
# requires v3.4 because use CMAKE_<LANG>_COMPILER_LAUNCHER

project(pbart)


#############
## OPTIONS ##
#############

# Static code analysis
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Generate file 'compile_commands.json' for clang-check
# Usage: awk -F: '/"file"/{print $2 }' build/compile_commands.json | xargs clang-check -fixit -p build
#        awk -F: '/"file"/{print $2 }' build/compile_commands.json | xargs clang-tidy  -fix   -p build

# Default: -march=corei7
set( MARCH "corei7"  CACHE STRING "Control flag -march" )
# Usage:  cmake .. -DMARCH=native (if native => convert to real cpu-type)
#         cmake .. -DMARCH=       (disable flag -march)

set( OPTIM "" CACHE STRING "Control flags -Ox" )
# Usage: cmake -DOPTIM=-Ofast

option (LTO "Link Time Optimization" OFF)
# Usage: cmake -DLTO=ON

set (PGO "" CACHE STRING "Profile Guided Optimization (can be empty, generate or use)" )
# Usage: cmake -DPGO=generate
#        ctest
#        cmake -DPGO=use


######################
## CMAKE_BUILD_TYPE ##
######################

# Release  = Debug info g3 + disable assert
# Coverage = Debug info g3 + disable assert + Coverage
# Debug    = Debug info g3 + enable  assert + -D_GLIBCXX_DEBUG_PEDANTIC
# Sanitize = Enable sanitize compiler flags

# Default = Release
if( NOT CMAKE_BUILD_TYPE )
    message(STATUS "** No build type => Default is 'Release'")
    set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE )
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug" "Coverage") # For 'cmake -i' and 'cmake-gui'
endif()

# Coverage: check tools gcov/lcov/... and set flag --coverage
# HTML report: gcovr --root . --html --html-details --output coverage.html --exclude-unreachable-branches --print-summary
if( CMAKE_BUILD_TYPE STREQUAL "Coverage" )
    add_definitions( -DNDEBUG )  # Disable assert to avoid code coverage bias
    if( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
        set (CoverageCmplFlags --coverage) #add_compile_options( --coverage )
        set (CoverageLinkFlags --coverage) #link_libraries(      --coverage )
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
                                 --exclude          ${CMAKE_SOURCE_DIR}/src/test
                                 --exclude          ${CMAKE_BINARY_DIR}
                                 --object-directory ${CMAKE_BINARY_DIR}
                                 --output           ${CMAKE_BINARY_DIR}/gcovr.html
                                 --html --html-details --sort-uncovered
                                 --print-summary --exclude-unreachable-branches
            #   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMAND echo "** Coverage report: firefox ${CMAKE_BINARY_DIR}/gcovr.html"
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
                                   --no-checksum --base-directory ${CMAKE_SOURCE_DIR}
                                   --rc    lcov_branch_coverage=1 --quiet
                COMMAND ${lcov}    --quiet --extract              ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-all.info
                                                                  ${CMAKE_SOURCE_DIR}/include/pbart/*
                                                                  ${CMAKE_SOURCE_DIR}/src/main/cpp/*/*
                                   --no-checksum --output-file    ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.info
                                   --rc    lcov_branch_coverage=1
                COMMAND ${genhtml} --rc genhtml_branch_coverage=1 ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.info
                                   --output-directory             ${CMAKE_BINARY_DIR}/lcov
                                   --highlight --legend --quiet
                COMMAND echo "** Coverage report: firefox ${CMAKE_BINARY_DIR}/lcov/index.html"
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
if( CMAKE_BUILD_TYPE STREQUAL "Sanitize" )
    add_compile_options(-fsanitize=address -fsanitize=leak -fsanitize=undefined
                        -fsanitize=signed-integer-overflow -fsanitize=shift
                        -fsanitize=integer-divide-by-zero -fsanitize=null)
else()

    # Full debug info for all other CMAKE_BUILD_TYPE
    add_compile_options( -g3 -ggdb3 )  # -g3 -> include also the MACRO definitions

    # Enable many wanings
    if (CMAKE_COMPILER_IS_GNUCXX)
        set (WarningFlags -Wall -Wextra -Wswitch-enum -Wno-ignored-qualifiers -pedantic -pedantic-errors -Wconversion -Wno-unused-but-set-variable -Wno-unused-variable -Wno-unused-function -Wno-conversion) #-Weffc++ -Wpadded)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set (WarningFlags -Weverything -Wno-c++98-compat)
        #et (WarningFlags -Wimplicit-fallthrough)
    endif()

endif()


# Speed: Use pipes rather than temporary files between the compiler stages
add_compile_options( -pipe )
link_libraries(      -pipe )

# Retrieve above compile definitions and options
get_directory_property (CompileDefinitions COMPILE_DEFINITIONS)
get_directory_property (CompileOptions     COMPILE_OPTIONS)
string (REPLACE ";" " " CompileFlags "${CompileDefinitions} ${CompileOptions}")
set (FlagsC   ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE}}   ${CompileFlags})
set (FlagsCxx ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}} ${CompileFlags})



###########
## TOOLS ##
###########

# Speed up build if ccache installed
find_program (ccache ccache)
if (ccache)
    message(STATUS "** Command 'ccache' detected => Will use 'ccache' to speed up compilation and link" )
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK    ccache)
    #et_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache) # Before cmake-3.4
    set (CMAKE_C_COMPILER_LAUNCHER   ccache)
    set (CMAKE_CXX_COMPILER_LAUNCHER ccache)
    set (wrapper_name ccache)
else()
    set (wrapper_name default)
endif()
set (wrapper_sh_in ${CMAKE_SOURCE_DIR}/CMake/wrapper_${wrapper_name}.sh.in)
set (wrapper_sh    ${CMAKE_BINARY_DIR}/CMake/wrapper_${wrapper_name}.sh)
configure_file(${wrapper_sh_in} ${wrapper_sh} @ONLY)

# Use tool "include-what-you-use" if available
find_program (iwyu NAMES iwyu include-what-you-use)
if (iwyu)
    set (CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${iwyu})
    set (CMAKE_C_INCLUDE_WHAT_YOU_USE   ${iwyu})
endif()


##########################
## EXTENAL DEPENDENCIES ##
##########################

find_package(Protobuf REQUIRED) # sudo apt install libprotobuf-dev
add_library(               pbart_Protobuf        INTERFACE)
target_include_directories(pbart_Protobuf SYSTEM INTERFACE ${PROTOBUF_INCLUDE_DIRS})
target_link_libraries(     pbart_Protobuf        INTERFACE ${PROTOBUF_LIBRARIES})

find_package(Boost REQUIRED) # sudo apt install libboost-dev
add_library(               pbart_Boost        INTERFACE)
target_include_directories(pbart_Boost SYSTEM INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(     pbart_Boost        INTERFACE ${Boost_LIBRARIES})
# TODO(olibre): Replace pbart_Boost by Boost::boost whith cmake-3.5

add_subdirectory(ext/tinyxml2)
add_library(               pbart_TinyXML        INTERFACE)
target_include_directories(pbart_TinyXML SYSTEM INTERFACE ext/tinyxml2)
target_link_libraries(     pbart_TinyXML        INTERFACE tinyxml2)

if( CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    set(make "$(MAKE)") # Allow passing arguments from parent make command
else()
    set(make make)      # Use 'make' because $(MAKE) fails using ninja
endif()


include(ExternalProject)
ExternalProject_Add(   cppunit
#   EXCLUDE_FROM_ALL
    PREFIX             ${CMAKE_BINARY_DIR}/cppunit
    SOURCE_DIR         ${CMAKE_SOURCE_DIR}/ext/cppunit
    UPDATE_COMMAND     autoconf || autoreconf --install #; make distclean
    CONFIGURE_COMMAND  ${wrapper_sh} ${CMAKE_SOURCE_DIR}/ext/cppunit/configure
                       --disable-dependency-tracking
                       --enable-silent-rules
                       --enable-static=no
                       --prefix=${CMAKE_BINARY_DIR}/cppunit
    BUILD_COMMAND      ${make}
    INSTALL_COMMAND    ${make} install
    BUILD_BYPRODUCTS   ${CMAKE_BINARY_DIR}/cppunit/lib/libcppunit.so # for ninja
                       ${CMAKE_BINARY_DIR}/cppunit/include
                       ${CMAKE_BINARY_DIR}/cppunit/include/cppunit/config-auto.h
    BUILD_IN_SOURCE    0 # TODO(olibre): Set zero and fix INTERFACE_INCLUDE_DIRECTORIES
)                        # TODO(olibre): and remove make_directory(), use? $<BUILD_INTERFACE:...>
make_directory( ${CMAKE_BINARY_DIR}/cppunit/include )

add_library(           pbart_cppunit SHARED IMPORTED)
add_dependencies(      pbart_cppunit cppunit)
set_target_properties( pbart_cppunit PROPERTIES
                       IMPORTED_LOCATION             ${CMAKE_BINARY_DIR}/cppunit/lib/libcppunit.so
                       INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/cppunit/include )


######################
## C++ CORE LIBRARY ##
######################

file (GLOB pbart_private_files src/main/cpp/pbart/*.cpp
                               src/main/cpp/pbart/*.h    )
file (GLOB pbart_public_files  include/pbart/*.h         )
add_library(               pbart ${pbart_private_files} ${pbart_public_files})
target_link_libraries(     pbart PUBLIC  pbart_Protobuf pbart_Boost PRIVATE pbart_TinyXML)
target_include_directories(pbart PUBLIC  include)
#arget_compile_definitions(pbart PRIVATE COMPILING_PBART=CMake)  # For MSWin export dll
target_compile_features(   pbart PUBLIC  cxx_long_long_type)
target_compile_options(    pbart PUBLIC  ${CoverageCmplFlags} PRIVATE ${WarningFlags})
target_link_libraries(     pbart PUBLIC  ${CoverageLinkFlags} PRIVATE ${WarningFlags})


######################
##  C++ UNIT TESTS  ##
######################

enable_testing()

add_library(          TestMain src/test/cpp/pbart/Main.cpp)
target_link_libraries(TestMain pbart pbart_cppunit)

set (DicoDir ${CMAKE_SOURCE_DIR}/src/test/dico/)

protobuf_generate_cpp(DicoBigSrc DicoBigHdr ${DicoDir}/dico-Big.proto)
add_executable(            TestBig src/test/cpp/pbart/TestBig.cpp ${DicoBigSrc} ${DicoBigHdr})
target_compile_definitions(TestBig PRIVATE DICO_BIG_PROTOBUF_HEADER="${DicoBigHdr}")
target_link_libraries(     TestBig TestMain)
add_test(                  TestBig TestBig          ${DicoDir} )

add_executable(       TestCodec src/test/cpp/pbart/TestCodec.cpp)
target_link_libraries(TestCodec TestMain)
add_test(             TestCodec TestCodec           ${DicoDir} )

add_executable(       TestCodecRepeated src/test/cpp/pbart/TestCodecRepeated.cpp)
target_link_libraries(TestCodecRepeated TestMain)
add_test(             TestCodecRepeated TestCodec   ${DicoDir} )

add_executable(       TestDicoAsMsg src/test/cpp/pbart/TestDicoAsMsg.cpp)
target_link_libraries(TestDicoAsMsg TestMain)
add_test(             TestDicoAsMsg TestDicoAsMsg   ${DicoDir} )

add_executable(       TestDico          src/test/cpp/pbart/TestDico.cpp)
target_link_libraries(TestDico          TestMain)
add_test(             TestDico          TestDico    ${DicoDir} )

add_executable(       TestMessage       src/test/cpp/pbart/TestMessage.cpp)
target_link_libraries(TestMessage       TestMain)
add_test(             TestMessage       TestMessage ${DicoDir} )

add_executable(       TestPerf          src/test/cpp/pbart/TestPerf.cpp)
target_link_libraries(TestPerf          TestMain)
add_test(             TestPerf          TestPerf    ${DicoDir} )

add_executable(       TestVariant       src/test/cpp/pbart/TestVariant.cpp)
target_link_libraries(TestVariant       TestMain)
add_test(             TestVariant       TestVariant ${DicoDir} )


######################
##       JNI        ##
######################

find_package(Java COMPONENTS Development)
include (UseJava)
find_package(JNI)

message (STATUS "** JAVA_AWT_LIBRARY=${JAVA_AWT_LIBRARY}")
message (STATUS "** JAVA_JVM_LIBRARY=${JAVA_JVM_LIBRARY}")

create_javah(
    TARGET          pbart-jni-serializer-header
    GENERATED_FILES PbartJniSerializerHeader
    CLASSES         org.Lmap.pbart.Serializer
    CLASSPATH       ${PROJECT_SOURCE_DIR}/src/main/java
)
create_javah(
    TARGET          pbart-jni-message-header
    GENERATED_FILES PbartJniMessageHeader
    CLASSES         org.Lmap.pbart.Message
    CLASSPATH       ${PROJECT_SOURCE_DIR}/src/main/java
)

add_library( pbart-jni src/main/cpp/jni/org_Lmap_pbart_Message.cpp
                       src/main/cpp/jni/org_Lmap_pbart_Serializer.cpp
                       src/main/cpp/jni/cpp-java-conversion-helpers.cpp
                       src/main/cpp/jni/cpp-java-conversion-helpers.h
                       ${PbartJniSerializerHeader}
                       ${PbartJniMessageHeader}
)
add_dependencies(          pbart-jni pbart-jni-serializer-header pbart-jni-message-header)
target_include_directories(pbart-jni PUBLIC ${JNI_INCLUDE_DIRS})
target_compile_options(    pbart-jni PRIVATE ${WarningFlags} ${CoverageCmplFlags})
target_link_libraries(     pbart-jni PRIVATE ${WarningFlags} ${CoverageLinkFlags}
                                     PUBLIC  pbart)
target_compile_definitions(pbart-jni PUBLIC
    GENERATED_JNI_SERIALIZER_HEADER="${PbartJniSerializerHeader}"
    GENERATED_JNI_MESSAGE_HEADER="${PbartJniMessageHeader}"
)

add_executable(       TestJNI src/test/cpp/jni/TestJNI.cpp)
target_link_libraries(TestJNI pbart-jni TestMain)
add_test(             TestJNI TestJNI ${DicoDir})


######################
##       JAVA       ##
######################

file(GLOB PbartJavaFiles       src/main/java/org/Lmap/pbart/*.java)
add_jar(pbart-java #ENTRY_POINT java/src/main/java/org/Lmap/pbart
                   SOURCES ${PbartJavaFiles}
                   OUTPUT_NAME pbart
                   VERSION     1.0.0  # TODO (olibre): Retreive from CMAKE_PROJECT_VERSION
)

find_jar (junit
          NAMES junit4-4.13 junit4-4.12 junit4-4.11 junit4-4.10 junit4-4.9 junit4-4.8 junit4-4.7 junit4-4.6 junit
          DOC "Search JUnit: first v4, then v3"
)
if( NOT junit)
    message(WARNING "** Cannot find JUnit jar in ${Java_LIBRARIES} => Skip Java Unit Tests")
else()
    message(STATUS "** Found ${junit} => Enable Java Unit Tests")
    add_jar(TestJava INCLUDE_JARS pbart-java ${junit}
                     ENTRY_POINT  org/junit/runner/JUnitCore
                     SOURCES      src/test/java/org/Lmap/pbart/TestSimple.java
                                  src/test/java/org/Lmap/pbart/TestPerf.java
    )
    add_test(TestJavaSimple java -Djava.library.path=${CMAKE_BINARY_DIR} -cp ${junit}:${CMAKE_BINARY_DIR}/pbart.jar:${CMAKE_BINARY_DIR}/TestJava.jar org.junit.runner.JUnitCore org.Lmap.pbart.TestSimple)
    add_test(TestJavaPerf   java -Djava.library.path=${CMAKE_BINARY_DIR} -cp ${junit}:${CMAKE_BINARY_DIR}/pbart.jar:${CMAKE_BINARY_DIR}/TestJava.jar org.junit.runner.JUnitCore org.Lmap.pbart.TestPerf  )
endif()
