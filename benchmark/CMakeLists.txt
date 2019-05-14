add_compile_options(-isystem -lpthread) # required by the benchmark library

add_executable(benchmark_approximate_adder approximate_adder_error_metrics.cpp)

target_link_libraries(benchmark_approximate_adder
	PRIVATE error_metrics
	PRIVATE bdd_examples
	PRIVATE benchmark
)

add_executable(benchmark_iscas_85 iscas85.cpp)

target_link_libraries(benchmark_iscas_85
        PRIVATE aig_parser
        PRIVATE error_metrics
        PRIVATE bdd_examples
        PRIVATE benchmark
        PRIVATE abo_util
)

# copies the iscas dataset used for benchmarking into the build folder so that they are actually found
add_custom_command(
        TARGET benchmark_iscas_85 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/iscas85
        ${CMAKE_CURRENT_BINARY_DIR}/iscas85
        COMMENT "Copying iscas dataset for benchmarking"
        VERBATIM)
