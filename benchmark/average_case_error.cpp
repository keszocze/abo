#include <benchmark/benchmark.h>
#include <cudd/cplusplus/cuddObj.hh>

#include "approximation_operators.hpp"
#include "benchmark_util.hpp"

using namespace abo::benchmark;

// input: test file, rounding level, round up (true) or down (false)
static void benchmark_ace(benchmark::State& state) {

    state.PauseTiming();

    const ISCAS85File file_id = static_cast<ISCAS85File>(state.range(0));
    const std::string file = iscas_85_filename_by_id(file_id);

    state.SetLabel(file + (state.range(2) ? "(round_up->" : "(round_down->") + std::to_string(state.range(1)+1) + ")");

    Cudd mgr(64);

    std::vector<BDD> original = load_iscas_85_file(mgr, file_id);

    std::vector<BDD> rounded;
    rounded.reserve(original.size());
    for (const BDD &orig : original) {
        if (state.range(2)) {
            rounded.push_back(abo::operators::superset_light_child(mgr, orig, state.range(1), 10000));
        } else {
            rounded.push_back(abo::operators::subset_light_child(mgr, orig, state.range(1), 10000));
        }

    }
    state.ResumeTiming();

    const int BATCH_SIZE = 1000;
    while (state.KeepRunningBatch(BATCH_SIZE)) {
        for (int i = 0;i<BATCH_SIZE;i++) {
            abo::benchmark::compute_error_metric(mgr, original, rounded, abo::benchmark::ErrorMetric::AVERAGE_CASE);

            state.PauseTiming();
            mgr.CollectGarbage();
            state.ResumeTiming();
        }
    }
}

BENCHMARK(benchmark_ace)->Unit(benchmark::kMillisecond)->Apply([](auto *b) {
    b->Args({static_cast<int>(ISCAS85File::C432), 35, true})
        ->Args({static_cast<int>(ISCAS85File::C499), 36, false})
        ->Args({static_cast<int>(ISCAS85File::C1908), 32, true});
});

BENCHMARK_MAIN();
