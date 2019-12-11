include(ExternalProject)

ExternalProject_Add(project_benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.5.0
    BUILD_BYPRODUCTS "<INSTALL_DIR>/lib"
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DCMAKE_CXX_COMPILER:PATH=${CMAKE_CXX_COMPILER}
        -DBENCHMARK_ENABLE_TESTING:BOOL=OFF
        -DBENCHMARK_ENABLE_INSTALL:BOOL=ON
        -DCMAKE_BUILD_TYPE=Release
        -DBENCHMARK_ENABLE_LTO=false
)

add_library(benchmark UNKNOWN IMPORTED)
add_dependencies(benchmark project_benchmark)

externalproject_get_property(project_benchmark install_dir)
# cmake enforces that the include directory exists
file(MAKE_DIRECTORY "${install_dir}/include/")
set_target_properties(benchmark PROPERTIES
    IMPORTED_LOCATION "${install_dir}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${install_dir}/include/"
)
find_package(Threads REQUIRED)
target_link_libraries(benchmark INTERFACE Threads::Threads)
