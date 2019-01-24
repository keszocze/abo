add_library(error_metrics error_rate.cpp error_rate.hpp worst_case_error.cpp worst_case_error.hpp)

target_link_libraries(error_metrics PUBLIC cudd PRIVATE abo_util)

target_include_directories(error_metrics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})