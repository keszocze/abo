add_library(nsga2minimization
        bddminimizationproblem.cpp
        bddminimizationproblem.hpp
        nsga2_minimization_helper.cpp
        nsga2_minimization_helper.hpp
        )

target_link_libraries(nsga2minimization
        PUBLIC pagmo2
        PUBLIC error_metrics
        PUBLIC approximation_operators
        PRIVATE benchmark_util
        )

target_include_directories(nsga2minimization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(nsga2_iscas
        nsga2_minimize_iscas.cpp
        )

target_link_libraries(nsga2_iscas
        nsga2minimization
        bucket_minimization
        benchmark_util
        aig_parser
        benchmark_util
        )

add_executable(nsga2_epfl
        nsga2_minimize_epfl.cpp
        )

target_link_libraries(nsga2_epfl
        nsga2minimization
        bucket_minimization
        benchmark_util
        aig_parser
        benchmark_util
        )

add_executable(nsga2_adders
        nsga2_minimize_adders.cpp
        )

target_link_libraries(nsga2_adders
        nsga2minimization
        bucket_minimization
        bdd_examples
        )

# copies the iscas dataset used for benchmarking into the build folder so that they are actually found
add_custom_command(
        TARGET nsga2_iscas POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/benchmarks/iscas85
        ${CMAKE_CURRENT_BINARY_DIR}/iscas85
        COMMENT "Copying iscas dataset for the minimization benchmarks"
        VERBATIM)

# copies the epfl dataset used for benchmarking into the build folder so that they are actually found
add_custom_command(
        TARGET nsga2_epfl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/benchmarks/epfl_combinational/arithmetic/
        ${CMAKE_CURRENT_BINARY_DIR}/epfl
        COMMENT "Copying EPFL dataset for the minimization benchmarks"
        VERBATIM)