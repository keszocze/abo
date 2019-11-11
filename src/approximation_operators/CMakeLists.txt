add_library(approximation_operators
        approximation_operators.cpp
        approximation_operators.hpp
)
target_link_libraries(approximation_operators
    PUBLIC cudd
    PRIVATE abo_util
)
target_include_directories(approximation_operators PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
