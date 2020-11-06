add_library(benchmark_util
    benchmark_util.cpp
    benchmark_util.hpp
)

target_link_libraries(benchmark_util
    PUBLIC cudd
    PRIVATE error_metrics
    PRIVATE aig_parser
    PRIVATE bdd_examples
)

target_include_directories(benchmark_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
