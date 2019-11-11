#include <benchmark/benchmark.h>
#include <cudd/cplusplus/cuddObj.hh>

#include "aig_parser.hpp"
#include "approximation_operators.hpp"

enum TestFile {
    C17 = 0, C432 = 1, C499 = 2, C880 = 3,
    C1355 = 4, C1908 = 5, C2670 = 6, C3540 = 7,
    C5315 = 8, C6288 = 9, C7552 = 10
};

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

    const std::vector<std::string> files = {"c17.aig", "c432.aig", "c499.aig", "c880.aig", "c1355.aig", "c1908.aig",
                                            "c2670.aig", "c3540.aig", "c5315.aig", "c6288.aig", "c7552.aig"};

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
    state.SetLabel(files[state.range(0)] + "(" + rounding_text + "->" + std::to_string(state.range(2)+1) + ")");
    for (auto _ : state) {
        state.PauseTiming();
        Cudd mgr(64); // the exact number does not matter, it will be expanded if necessary

        abo::parser::aig_parser parser(mgr);
        lorina::read_aiger("iscas85/" + files[state.range(0)], parser);

        std::vector<BDD> original = parser.get_outputs();

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
            b = b->Args({C432, op, i});
        }
    }
});

BENCHMARK_MAIN();
