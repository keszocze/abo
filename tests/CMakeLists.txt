add_executable(parsing_test parsing_test.cpp)

target_link_libraries(parsing_test pla_parser cudd abo_util)


add_custom_command(
        TARGET parsing_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/tests/or.pla ${CMAKE_SOURCE_DIR}/tests/andor.pla
        ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Copying .pla files for testing"
        VERBATIM)