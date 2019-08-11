#include <benchmark/benchmark.h>
#include <cudd/cplusplus/cuddObj.hh>

#include "aig_parser.hpp"
#include "approximation_operators.hpp"
#include "error_rate.hpp"
#include "average_case_error.hpp"
#include "worst_case_error.hpp"
#include "approximate_adders.hpp"
#include "average_bit_flip_error.hpp"
#include "worst_case_bit_flip_error.hpp"

enum ErrorMetric {
    WORST_CASE,
    AVERAGE_CASE,
    MEAN_SQUARED,
    ERROR_RATE,
    AVERAGE_BIT_FLIP,
    WORST_CASE_BIT_FLIP,
    APPROXIMATE_WORST_CASE_5
};

enum TestFile {
    C17 = 0, C432 = 1, C499 = 2, C880 = 3,
    C1355 = 4, C1908 = 5, C2670 = 6, C3540 = 7,
    C5315 = 8, C6288 = 9, C7552 = 10
};

enum RoundingOperation {
    ROUND_UP,
    ROUND_DOWN,
    ROUND_FULL
};

// input: error metric, test file, rounding operation, rounding level
static void benchmark_iscas85(benchmark::State& state) {

    const std::vector<std::string> files = {"c17.aig", "c432.aig", "c499.aig", "c880.aig", "c1355.aig", "c1908.aig",
                                            "c2670.aig", "c3540.aig", "c5315.aig", "c6288.aig", "c7552.aig"};

    std::string error_metric_name;
    switch (state.range(0)) {
    case WORST_CASE: error_metric_name = "worst case"; break;
    case AVERAGE_CASE: error_metric_name = "average case"; break;
    case MEAN_SQUARED: error_metric_name = "mean squared"; break;
    case ERROR_RATE: error_metric_name = "error rate"; break;
    case AVERAGE_BIT_FLIP: error_metric_name = "average bit flip"; break;
    case WORST_CASE_BIT_FLIP: error_metric_name = "worst case bit flip"; break;
    case APPROXIMATE_WORST_CASE_5: error_metric_name = "approximate worst case - 5 bit"; break;
    }
    std::string rounding_text;
    switch (state.range(2)) {
    case ROUND_UP: rounding_text = "round up"; break;
    case ROUND_DOWN: rounding_text = "round down"; break;
    case ROUND_FULL: rounding_text = "round"; break;
    }
    state.SetLabel(files[state.range(1)] + "(" + rounding_text + "->" + std::to_string(state.range(3)+1) + ") - " + error_metric_name);
    for (auto _ : state) {
        state.PauseTiming();
        Cudd mgr(64); // the exact number does not matter, it will be expanded if necessary

        abo::parser::aig_parser parser(mgr);
        lorina::read_aiger("iscas85/" + files[state.range(1)], parser);

        std::vector<BDD> original = parser.get_outputs();

        std::vector<BDD> rounded;
        rounded.reserve(original.size());
        for (const BDD &orig : original) {
            switch (state.range(2)) {
            case ROUND_UP:
                rounded.push_back(abo::operators::superset_heavy_child(mgr, orig, state.range(3), 10000));
                break;
            case ROUND_DOWN:
                rounded.push_back(abo::operators::subset_light_child(mgr, orig, state.range(3), 10000));
                break;
            case ROUND_FULL:
                rounded.push_back(abo::operators::round(mgr, orig, state.range(3)));
                break;
            }
        }
        state.ResumeTiming();

        switch (state.range(0)) {
        case WORST_CASE:
            abo::error_metrics::worst_case_error(mgr, original, rounded);
            break;
        case AVERAGE_CASE:
            abo::error_metrics::average_case_error(mgr, original, rounded);
            break;
        case MEAN_SQUARED:
            abo::error_metrics::mean_squared_error(mgr, original, rounded);
            break;
        case ERROR_RATE:
            abo::error_metrics::error_rate(mgr, original, rounded);
            break;
        case AVERAGE_BIT_FLIP:
            abo::error_metrics::average_bit_flip_error(original, rounded);
            break;
        case WORST_CASE_BIT_FLIP:
            abo::error_metrics::worst_case_bit_flip_error(mgr, original, rounded);
            break;
        case APPROXIMATE_WORST_CASE_5:
            abo::error_metrics::approximate_worst_case_error(mgr, original, rounded, 5);
            break;
        }
    }
}

// this re-creates the time table found in "BDD Minimization for Approximate Computing"
BENCHMARK(benchmark_iscas85)->Unit(benchmark::kMillisecond)->Apply([](auto *b) {
    std::vector<ErrorMetric> metrics = {ERROR_RATE, WORST_CASE, AVERAGE_CASE};
    for (auto metric : metrics) {
        b = b->Args({metric, C432, ROUND_DOWN, 34})
                ->Args({metric, C432, ROUND_UP, 34})
                ->Args({metric, C432, ROUND_FULL, 34})
                ->Args({metric, C499, ROUND_DOWN, 39})
                ->Args({metric, C499, ROUND_DOWN, 35})
                ->Args({metric, C499, ROUND_UP, 39})
                ->Args({metric, C499, ROUND_UP, 35})
                ->Args({metric, C499, ROUND_FULL, 39})
                ->Args({metric, C499, ROUND_FULL, 35})
                ->Args({metric, C1355, ROUND_DOWN, 39})
                ->Args({metric, C1355, ROUND_UP, 39})
                ->Args({metric, C1908, ROUND_DOWN, 31})
                ->Args({metric, C1908, ROUND_DOWN, 26})
                ->Args({metric, C1908, ROUND_DOWN, 22})
                ->Args({metric, C1908, ROUND_UP, 31})
                ->Args({metric, C1908, ROUND_UP, 26})
                ->Args({metric, C1908, ROUND_UP, 22})
                ->Args({metric, C1908, ROUND_FULL, 31})
                ->Args({metric, C1908, ROUND_FULL, 26})
                ->Args({metric, C1908, ROUND_FULL, 22});
    }
});

BENCHMARK_MAIN();
