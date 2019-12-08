add_subdirectory(util)

add_compile_options(-isystem -lpthread) # required by the benchmark library

add_executable(benchmark_approximate_adder approximate_adder_error_metrics.cpp)

target_link_libraries(benchmark_approximate_adder
	PRIVATE bdd_examples
	PRIVATE benchmark
        PRIVATE benchmark_util
)

add_executable(benchmark_iscas_85 iscas85.cpp)

target_link_libraries(benchmark_iscas_85
        PRIVATE benchmark
        PRIVATE approximation_operators
        PRIVATE benchmark_util
)

add_executable(benchmark_approximation_operators operators.cpp)

target_link_libraries(benchmark_approximation_operators
        PRIVATE benchmark_util
        PRIVATE benchmark
        PRIVATE approximation_operators
)

add_executable(benchmark_ace_steps ace_steps.cpp)

target_link_libraries(benchmark_ace_steps
        PRIVATE error_metrics
        PRIVATE benchmark
        PRIVATE abo_util
        PRIVATE benchmark_util
        PRIVATE approximation_operators
)

add_executable(benchmark_average_case_error average_case_error.cpp)

target_link_libraries(benchmark_average_case_error
        PRIVATE approximation_operators
        PRIVATE benchmark
        PRIVATE benchmark_util
)

add_executable(benchmark_acr_approx acr_approx.cpp)

target_link_libraries(benchmark_acr_approx
        PRIVATE bdd_examples
        PRIVATE benchmark
        PRIVATE benchmark_util
)


add_executable(benchmark_sampling_methods sampling_methods.cpp)

target_link_libraries(benchmark_sampling_methods
        PRIVATE error_metrics
        PRIVATE benchmark
        PRIVATE benchmark_util
)

# copies the iscas dataset used for benchmarking into the build folder so that they are actually found
add_custom_command(
        TARGET benchmark_iscas_85 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/iscas85
        ${CMAKE_CURRENT_BINARY_DIR}/iscas85
        COMMENT "Copying iscas dataset for benchmarking"
        VERBATIM)
