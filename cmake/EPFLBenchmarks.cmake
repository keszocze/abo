include(ExternalProject)

ExternalProject_Add(epfl_benchmarks
	GIT_REPOSITORY https://github.com/lsils/benchmarks.git
	GIT_SHALLOW 1
	CONFIGURE_COMMAND ""
	BUILD_COMMAND file(REMOVE ${PROJECT_SOURCE_DIR}/benchmarks/epfl_combinational/.git)
	INSTALL_COMMAND ""
	SOURCE_DIR ${PROJECT_SOURCE_DIR}/benchmarks/epfl_combinational
)
