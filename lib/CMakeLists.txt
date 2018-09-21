if (NOT TARGET fmt)
    add_library(fmt INTERFACE)
    target_include_directories(fmt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/fmt)
    target_compile_definitions(fmt INTERFACE FMT_HEADER_ONLY)
endif()

if (NOT TARGET range)
    add_library(rang INTERFACE)
    target_include_directories(rang INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/rang)
endif()


if (NOT TARGET lorina)
    add_library(lorina INTERFACE) # v0.1
    target_include_directories(lorina INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lorina)
    target_link_libraries(lorina INTERFACE rang fmt)
endif()