include(ExternalProject)

ExternalProject_Add(project_lorina
    GIT_REPOSITORY https://github.com/hriener/lorina.git
    GIT_TAG v0.2
    # the project sadly has no install step, which makes some of the following necessary
    INSTALL_COMMAND cmake -E echo "Skipping install step."
)

externalproject_get_property(project_lorina source_dir)

add_library(lorina INTERFACE)
add_dependencies(lorina project_lorina)
target_include_directories(lorina INTERFACE
    "${source_dir}/include"
    "${source_dir}/lib/fmt"
    "${source_dir}/lib/rang"
)
target_compile_definitions(lorina INTERFACE FMT_HEADER_ONLY)
