include(ExternalProject)

ExternalProject_Add(project_catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.4.0
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DCATCH_BUILD_TESTING:BOOL=OFF
        -DCMAKE_CXX_COMPILER:PATH=${CMAKE_CXX_COMPILER}
)

add_library(catch INTERFACE)
add_dependencies(catch project_catch2)
target_compile_features(catch INTERFACE cxx_std_11)

externalproject_get_property(project_catch2 install_dir)
# cmake enforces that the include directory exists
file(MAKE_DIRECTORY "${install_dir}/include/")
set_property(TARGET catch PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${install_dir}/include/")
