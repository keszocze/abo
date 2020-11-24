add_executable(test_improved_sampling improved_sampling.cpp)

target_link_libraries(test_improved_sampling
        PRIVATE error_metrics
        PRIVATE benchmark
        PRIVATE approximation_operators
        PRIVATE benchmark_util
        )


# copies the iscas dataset used for benchmarking into the build folder so that they are actually found
add_custom_command(
        TARGET test_improved_sampling POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/benchmarks/iscas85
        ${CMAKE_CURRENT_BINARY_DIR}/iscas85
        COMMENT "Copying iscas dataset for the timing benchmarks"
        VERBATIM)

