# create reasonable cmake includes
add_subdirectory(cudd)
target_include_directories(cudd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
