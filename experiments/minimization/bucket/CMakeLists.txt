add_executable(minimize_adders minimize_adders.cpp)

target_link_libraries(minimize_adders
    PRIVATE bucket_minimization
)

add_executable(minimize_iscas minimize_iscas.cpp)

target_link_libraries(minimize_iscas
    PRIVATE bucket_minimization
    PRIVATE benchmark_util
)

add_executable(minimize_epfl minimize_epfl.cpp)

target_link_libraries(minimize_epfl
        PRIVATE bucket_minimization
        PRIVATE benchmark_util
        )

# copies the iscas dataset used for benchmarking into the build folder so that they are actually found
add_custom_command(
        TARGET minimize_iscas POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/benchmarks/iscas85
        ${CMAKE_CURRENT_BINARY_DIR}/iscas85
        COMMENT "Copying iscas dataset for the minimization benchmarks"
        VERBATIM)

# copies the epfl dataset used for benchmarking into the build folder so that they are actually found
add_custom_command(
        TARGET minimize_epfl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/benchmarks/epfl_combinational/arithmetic/
        ${CMAKE_CURRENT_BINARY_DIR}/epfl
        COMMENT "Copying EPFL dataset for the minimization benchmarks"
        VERBATIM)