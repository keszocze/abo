add_library(error_metrics error_rate.cpp error_rate.hpp)

target_link_libraries(error_metrics PRIVATE cudd)

target_include_directories(error_metrics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})