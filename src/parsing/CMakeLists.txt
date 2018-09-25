add_library(aig_parser aig_parser.cpp aig_parser.hpp)
add_library(pla_parser pla_parser.cpp pla_parser.hpp)

target_link_libraries(pla_parser lorina)
target_link_libraries(aig_parser lorina)