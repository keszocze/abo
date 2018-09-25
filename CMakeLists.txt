cmake_minimum_required(VERSION 3.12)
project(abo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ABO_TEST "Build tests" OFF)

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
add_subdirectory(tests)
add_subdirectory(src)


#target_link_libraries(tests abo cudd lorina)