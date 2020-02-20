include(ExternalProject)

ExternalProject_Add(project_cudd
    GIT_REPOSITORY https://github.com/keszocze/cudd.git
    GIT_TAG c71b5fe1147d036bc37e584c99bd3ba616c22064
    INSTALL_COMMAND cmake -E echo "Skipping install step."
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/project_cudd-prefix/src/cudd"
    CMAKE_ARGS
        -DCUDD_BUILD_TESTS:BOOL=FALSE
        -DCMAKE_CXX_COMPILER:PATH=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_FLAGS="-Wno-implicit-fallthrough"
        -DCMAKE_CXX_FLAGS="-Wno-implicit-fallthrough"
)

add_library(cudd UNKNOWN IMPORTED)
add_dependencies(cudd project_cudd)

externalproject_get_property(project_cudd binary_dir)
externalproject_get_property(project_cudd source_dir)

set_target_properties(cudd PROPERTIES
    IMPORTED_LOCATION "${binary_dir}/cplusplus/${CMAKE_STATIC_LIBRARY_PREFIX}obj${CMAKE_STATIC_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${source_dir}/.."
)
set_property(TARGET cudd PROPERTY INTERFACE_LINK_LIBRARIES
    "${binary_dir}/cudd/${CMAKE_STATIC_LIBRARY_PREFIX}cudd${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "${binary_dir}/epd/${CMAKE_STATIC_LIBRARY_PREFIX}epd${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "${binary_dir}/mtr/${CMAKE_STATIC_LIBRARY_PREFIX}mtr${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "${binary_dir}/st/${CMAKE_STATIC_LIBRARY_PREFIX}st${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "${binary_dir}/util/${CMAKE_STATIC_LIBRARY_PREFIX}util${CMAKE_STATIC_LIBRARY_SUFFIX}"
)
