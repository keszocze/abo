cmake_minimum_required(VERSION 3.10)
project(abo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(BUILD_TESTS "Build tests" ON)

option(BUILD_DOCUMENTATION "Build documentation" ON)

if(UNIX)
    # show quite some warnings (but remove some intentionally)
    include(CheckCXXCompilerFlag)
    add_compile_options(-W -Wall -Wextra)
    foreach (WARNING unknown-pragmas gnu-anonymous-struct nested-anon-types)
        check_cxx_compiler_flag("-Wno-${WARNING}" HAS_WNO_${WARNING})
        if (HAS_WNO_${WARNING})
            add_compile_options(-Wno-${WARNING})
        endif()
    endforeach()
endif()

include(BuildLorina)
include(BuildCatch2)
include(BuildBenchmark)
include(BuildCudd)

add_subdirectory(src)
add_subdirectory(benchmark)

if (BUILD_DOCUMENTATION)
    add_subdirectory(docs)
endif()

#IF (BUILD_TESTS)
    add_subdirectory(tests)
#endif(BUILD_TESTS)
