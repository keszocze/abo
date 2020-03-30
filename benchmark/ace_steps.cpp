#include <benchmark/benchmark.h>
#include <cudd/cplusplus/cuddObj.hh>

#include "approximation_operators.hpp"
#include "average_case_error.hpp"
#include "benchmark_util.hpp"
#include "cudd_helpers.hpp"

using namespace abo::benchmark;

// input: test file, rounding level, difference only (false) or average case error (true)
static void benchmark_ace_steps(benchmark::State& state)
{

    const ISCAS85File file_id = static_cast<ISCAS85File>(state.range(0));
    const std::string file = iscas_85_filename_by_id(file_id);
    const bool difference_only = state.range(2) == 0;

    state.SetLabel(file + "(round_up->" + std::to_string(state.range(1)) + ")," +
                   (difference_only ? "difference" : "ace"));

    const int BATCH_SIZE = 10;
    while (state.KeepRunningBatch(BATCH_SIZE))
    {
        for (int i = 0; i < BATCH_SIZE; i++)
        {
            state.PauseTiming();
            Cudd mgr(64);

            std::vector<BDD> original = load_iscas_85_file(mgr, file_id);

            std::vector<BDD> rounded;
            rounded.reserve(original.size());
            for (const BDD& orig : original)
            {
                rounded.push_back(abo::operators::superset_heavy_child(mgr, orig, state.range(1),
                                                                       10000)); // round all the way
            }
            state.ResumeTiming();

            original.push_back(mgr.bddZero());
            rounded.push_back(mgr.bddZero());

            std::vector<BDD> difference = abo::util::bdd_subtract(mgr, original, rounded);
            std::vector<BDD> absdiff =
                abo::util::abs(mgr, difference, abo::util::NumberRepresentation::TwosComplement);

            if (!difference_only)
            {
                abo::error_metrics::average_value(absdiff);
            }
        }
    }
}

BENCHMARK(benchmark_ace_steps)->Unit(benchmark::kMillisecond)->Apply([](auto* b) {
    for (bool difference_only : {false, true})
    {
        b = b->Args({static_cast<int>(ISCAS85File::C432), 20, difference_only})
                ->Args({static_cast<int>(ISCAS85File::C432), 30, difference_only})
                ->Args({static_cast<int>(ISCAS85File::C499), 35, difference_only})
                ->Args({static_cast<int>(ISCAS85File::C1355), 10, difference_only});
    }
});

BENCHMARK_MAIN();
