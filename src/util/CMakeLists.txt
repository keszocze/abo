add_library(abo_util
        string_helpers.cpp
        string_helpers.hpp
        cudd_helpers.cpp
        cudd_helpers.hpp
        cudd_helpers.hpp
        number_representation.hpp)
target_link_libraries(abo_util PUBLIC cudd)
target_include_directories(abo_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
