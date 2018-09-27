add_library(aig_parser aig_parser.cpp aig_parser.hpp)
add_library(pla_parser pla_parser.cpp pla_parser.hpp)

target_link_libraries(pla_parser lorina cudd)
target_include_directories(pla_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)



target_link_libraries(aig_parser lorina cudd)