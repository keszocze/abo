# create reasonable cmake includes
add_subdirectory(cudd)
target_include_directories(cudd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})




add_library(catch INTERFACE)
target_include_directories(catch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(catch INTERFACE cxx_std_11)

SET(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Benchmark library enable testing")
SET(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Benchmark library enable installing")
SET(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Benchmark library enable gtest testing")
add_subdirectory(benchmark INTERFACE)
