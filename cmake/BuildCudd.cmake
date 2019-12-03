include(ExternalProject)

set(CUDD_PATCH_FILE ${CMAKE_CURRENT_LIST_DIR}/public_gc.patch)

ExternalProject_Add(project_cudd
    GIT_REPOSITORY https://github.com/keszocze/cudd.git
    GIT_TAG cmake
    INSTALL_COMMAND cmake -E echo "Skipping install step."
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/project_cudd-prefix/src/cudd"
    PATCH_COMMAND cat ${CUDD_PATCH_FILE} | patch -p1
    CMAKE_ARGS
        -DCUDD_BUILD_TESTS:BOOL=FALSE
        -DCMAKE_CXX_COMPILER:PATH=${CMAKE_CXX_COMPILER}
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
