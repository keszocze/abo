add_library(bucket_minimization
        bucket_minimization.cpp
        bucket_minimization.hpp
        minimization_helper.cpp
        minimization_helper.hpp
)
target_link_libraries(bucket_minimization
    PUBLIC cudd
    PRIVATE abo_util
    PRIVATE approximation_operators
    PRIVATE error_metrics
    PRIVATE bdd_examples
    PRIVATE aig_parser
)
target_include_directories(bucket_minimization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
