#include <benchmark/benchmark.h>
#include <cudd/cplusplus/cuddObj.hh>

#include "approximation_operators.hpp"
#include "benchmark_util.hpp"

using namespace abo::benchmark;

enum RoundingOperation {
    SUPERSET_HEAVY,
    SUBSET_LIGHT,
    SUPERSET_LIGHT,
    SUBSET_HEAVY,
    ROUND_FULL,
    ROUND_BEST,
    COFACTOR_POSITIVE,
    COFACTOR_NEGATIVE
};

// input: test file, rounding operation, rounding level
static void benchmark_operators(benchmark::State& state) {

    const ISCAS85File file_id = static_cast<ISCAS85File>(state.range(0));
    const std::string file = iscas_85_filename_by_id(file_id);

    std::string rounding_text;
    switch (state.range(1)) {
    case SUPERSET_HEAVY: rounding_text = "superset-heavy"; break;
    case SUBSET_LIGHT: rounding_text = "subset-light"; break;
    case SUPERSET_LIGHT: rounding_text = "superset-light"; break;
    case SUBSET_HEAVY: rounding_text = "subset-heavy"; break;
    case ROUND_FULL: rounding_text = "round"; break;
    case ROUND_BEST: rounding_text = "round_best"; break;
    case COFACTOR_POSITIVE: rounding_text = "cof+"; break;
    case COFACTOR_NEGATIVE: rounding_text = "cof-"; break;
    }
    state.SetLabel(file + "(" + rounding_text + "->" + std::to_string(state.range(2)+1) + ")");
    for (auto _ : state) {
        state.PauseTiming();
        Cudd mgr(64);
        std::vector<BDD> original = load_iscas_85_file(mgr, file_id);

        std::vector<BDD> rounded;
        rounded.reserve(original.size());
        state.ResumeTiming();
        for (const BDD &orig : original) {
            switch (state.range(1)) {
            case SUPERSET_HEAVY:
                rounded.push_back(abo::operators::superset_heavy_child(mgr, orig, state.range(2), state.range(2)));
                break;
            case SUBSET_LIGHT:
                rounded.push_back(abo::operators::subset_light_child(mgr, orig, state.range(2), state.range(2)));
                break;
            case SUPERSET_LIGHT:
                rounded.push_back(abo::operators::superset_light_child(mgr, orig, state.range(2), state.range(2)));
                break;
            case SUBSET_HEAVY:
                rounded.push_back(abo::operators::subset_heavy_child(mgr, orig, state.range(2), state.range(2)));
                break;
            case ROUND_BEST:
                rounded.push_back(abo::operators::round_best(mgr, orig, state.range(2), state.range(2)));
                break;
            case ROUND_FULL:
                rounded.push_back(abo::operators::round(mgr, orig, state.range(2)));
                break;
            case COFACTOR_POSITIVE:
                rounded.push_back(orig.Cofactor(mgr.bddVar(state.range(2))));
                break;
            case COFACTOR_NEGATIVE:
                rounded.push_back(orig.Cofactor(!mgr.bddVar(state.range(2))));
                break;
            }
        }
    }
}

BENCHMARK(benchmark_operators)->Unit(benchmark::kMillisecond)->Apply([](auto *b) {
    for (auto op : {SUPERSET_HEAVY, SUBSET_LIGHT, SUPERSET_LIGHT, SUBSET_HEAVY, ROUND_FULL, ROUND_BEST, COFACTOR_POSITIVE, COFACTOR_NEGATIVE}) {
        for (int i = 1;i<41;i++) {
            b = b->Args({static_cast<int>(ISCAS85File::C432), op, i});
        }
    }
});

BENCHMARK_MAIN();
