include(ExternalProject)

ExternalProject_Add(project_pagmo2
    GIT_REPOSITORY https://github.com/esa/pagmo2.git
    GIT_TAG v2.12.0
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DCMAKE_CXX_COMPILER:PATH=${CMAKE_CXX_COMPILER}
)

add_library(pagmo2 UNKNOWN IMPORTED)
add_dependencies(pagmo2 project_pagmo2)

externalproject_get_property(project_pagmo2 install_dir)
# cmake enforces that the include directory exists
file(MAKE_DIRECTORY "${install_dir}/include/")
set_target_properties(pagmo2 PROPERTIES
    IMPORTED_LOCATION "${install_dir}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}pagmo${CMAKE_SHARED_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${install_dir}/include/"
)

# this should not be necessary, but there seems to be some weird bug somewhere
find_package(Boost 1.60.0 QUIET REQUIRED COMPONENTS serialization)
target_link_libraries(pagmo2 INTERFACE Boost::boost Boost::serialization)
