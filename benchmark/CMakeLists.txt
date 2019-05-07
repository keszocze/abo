add_compile_options(-isystem -lpthread) # required by the benchmark library

add_executable(benchmark_approximate_adder approximate_adder_error_metrics.cpp)

target_link_libraries(benchmark_approximate_adder
	PRIVATE error_metrics
	PRIVATE bdd_examples
	PRIVATE benchmark
)
