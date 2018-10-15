add_executable(parsing_test parsing_test.cpp)
add_executable(operations_test operations_test.cpp)
add_executable(error_metrics_test error_metrics_test.cpp)
add_library(catch-main catch-main.cpp)

target_link_libraries(parsing_test PRIVATE pla_parser abo_util catch catch-main)
target_link_libraries(operations_test PRIVATE abo_util catch catch-main)
target_link_libraries(error_metrics_test PRIVATE  catch catch-main abo_util error_metrics)
target_link_libraries(catch-main catch)


# copies the .pla files used for testing into the build folder so that they are actually found
add_custom_command(
        TARGET parsing_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/tests/or.pla ${CMAKE_SOURCE_DIR}/tests/funs.pla
        ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Copying .pla files for testing"
        VERBATIM)
