#include <benchmark/benchmark.h>
#include <cudd/cplusplus/cuddObj.hh>

#include "error_rate.hpp"
#include "benchmark_util.hpp"

using namespace abo::benchmark;

enum SamplingMethod {
    REGULAR,
    IMPROVED
};

// input: sampling method, number of samples, test file
static void benchmark_sampling_methods(benchmark::State& state) {

    const ISCAS85File file_id = static_cast<ISCAS85File>(state.range(2));
    const std::string file = iscas_85_filename_by_id(file_id);

    std::string sampling_name;
    switch (state.range(0)) {
    case REGULAR: sampling_name = "regular sampling"; break;
    case IMPROVED: sampling_name = "improved sampling"; break;
    }
    state.SetLabel(file + " - " + sampling_name + ": " + std::to_string(state.range(1)));

    Cudd mgr(0);
    std::vector<BDD> original = load_iscas_85_file(mgr, file_id);

    std::vector<BDD> rounded;
    rounded.reserve(original.size());
    for (std::size_t i = 0;i<original.size();i++) {
        rounded.push_back(mgr.bddZero());
    }

    for (auto _ : state) {
        switch (state.range(0)) {
        case REGULAR:
            abo::error_metrics::error_rate_sampling(mgr, original, rounded, state.range(1));
            break;
        case IMPROVED:
            abo::error_metrics::error_rate_efficient_sampling(mgr, original, rounded, state.range(1));
            break;
        }

        state.PauseTiming();
        mgr.CollectGarbage();
        state.ResumeTiming();
    }
}

BENCHMARK(benchmark_sampling_methods)->Unit(benchmark::kMillisecond)->Apply([](auto *b) {
    std::vector<int> samples = {1, 10, 100, 1000, 10000, 100000, 1000000};
    for (auto file_ : {ISCAS85File::C432, ISCAS85File::C499, ISCAS85File::C1908}) {
        int file = static_cast<int>(file_);
        for (auto x : {REGULAR, IMPROVED}) {
            for (auto n : samples) {
                b = b->Args({x, n, file});
            }
        }
    }

});

BENCHMARK_MAIN();
