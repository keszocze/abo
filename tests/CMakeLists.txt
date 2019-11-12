add_executable(parsing_test parsing_test.cpp)
add_executable(operations_test operations_test.cpp)
add_executable(error_metrics_test error_metrics_test.cpp)
add_executable(approximation_operations_test approximation_operations_test.cpp)
add_library(catch-main catch-main.cpp)

target_link_libraries(parsing_test PRIVATE pla_parser abo_util catch catch-main)
target_link_libraries(operations_test PRIVATE bdd_examples abo_util catch catch-main)
target_link_libraries(error_metrics_test PRIVATE  bdd_examples catch catch-main abo_util error_metrics)
target_link_libraries(approximation_operations_test PRIVATE bdd_examples catch catch-main abo_util approximation_operators)
target_link_libraries(catch-main catch)


# copies the .pla files used for testing into the build folder so that they are actually found
add_custom_command(
        TARGET parsing_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/tests/or.pla ${CMAKE_SOURCE_DIR}/tests/funs.pla ${CMAKE_SOURCE_DIR}/tests/dc.pla
        ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Copying .pla files for testing"
        VERBATIM)



## coverage

set(BUILD_COVERAGE_DATA TRUE)
if(CMAKE_COMPILER_IS_GNUCXX)
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
        if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 3)
            set(BUILD_COVERAGE_DATA FALSE)
        endif()
    endif()
else()
    set(BUILD_COVERAGE_DATA FALSE)
endif()

function(add_cov_stuff tgt)
    target_compile_options(${tgt} PRIVATE --coverage)
    target_link_options(${tgt} PRIVATE --coverage)
endfunction()

if(BUILD_COVERAGE_DATA)
    add_cov_stuff(parsing_test)
    add_cov_stuff(operations_test)
    add_cov_stuff(error_metrics_test)
    add_cov_stuff(approximation_operations_test)
endif()
