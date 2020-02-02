add_executable(minimize_adders minimize_adders.cpp)

target_link_libraries(minimize_adders
    PRIVATE bucket_minimization
)

add_executable(minimize_iscas minimize_iscas.cpp)

target_link_libraries(minimize_iscas
    PRIVATE bucket_minimization
)

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
)

target_include_directories(nsga2minimization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(nsga2_iscas
    nsga2_minimize_iscas.cpp
)

target_link_libraries(nsga2_iscas
    nsga2minimization
    bucket_minimization
    aig_parser
)

add_executable(nsga2_adders
    nsga2_minimize_adders.cpp
)

target_link_libraries(nsga2_adders
    nsga2minimization
    bucket_minimization
    bdd_examples
)

# copies the script to run all experiments into build folder
add_custom_command(
        TARGET minimize_iscas POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/run_experiments.sh
        ${CMAKE_BINARY_DIR}/run_experiments.sh
        COMMENT "Copying run experiments script"
        VERBATIM
)
