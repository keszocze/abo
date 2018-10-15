add_library(bdd_examples simple.cpp simple.hpp from_papers.cpp from_papers.hpp)

target_link_libraries(bdd_examples PUBLIC cudd)

target_include_directories(bdd_examples PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})