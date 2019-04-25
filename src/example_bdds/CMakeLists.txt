add_library(bdd_examples simple.cpp simple.hpp from_papers.cpp from_papers.hpp approximate_adders.hpp approximate_adders.cpp)

target_link_libraries(bdd_examples PUBLIC cudd PRIVATE abo_util)

target_include_directories(bdd_examples PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
