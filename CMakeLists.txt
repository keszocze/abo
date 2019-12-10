cmake_minimum_required(VERSION 3.10)
project(abo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" ON)

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


add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(benchmark)
add_subdirectory(experiments)

#IF (BUILD_TESTS)
    add_subdirectory(tests)
#endif(BUILD_TESTS)
