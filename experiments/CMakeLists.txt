add_subdirectory(util)
add_subdirectory(minimization)
add_subdirectory(runtime)

add_executable(testing testing.cpp)

target_link_libraries(testing PRIVATE approximation_operators bdd_examples abo_util)