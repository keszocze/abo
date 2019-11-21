#include <benchmark/benchmark.h>
#include <cudd/cplusplus/cuddObj.hh>

#include "error_rate.hpp"
#include "average_case_error.hpp"
#include "worst_case_error.hpp"
#include "approximate_adders.hpp"
#include "average_bit_flip_error.hpp"
#include "worst_case_bit_flip_error.hpp"
#include "average_case_relative_error.hpp"
#include "worst_case_relative_error.hpp"

enum ErrorMetric {
    WORST_CASE,
    WORST_CASE_RELATIVE,
    AVERAGE_CASE,
    AVERAGE_CASE_RELATIVE,
    MEAN_SQUARED,
    ERROR_RATE,
    AVERAGE_BIT_FLIP,
    WORST_CASE_BIT_FLIP,
};

enum AdderGenerators {
    ACA1,
    ACA2,
    GDA,
    GEAR
};

// input: error metric, adder generator, parameters for that generator
static void generate_adders(benchmark::State& state) {

    std::string error_metric_name;
    switch (state.range(0)) {
    case WORST_CASE: error_metric_name = "worst case"; break;
    case WORST_CASE_RELATIVE: error_metric_name = "worst case relative"; break;
    case AVERAGE_CASE: error_metric_name = "average case"; break;
    case AVERAGE_CASE_RELATIVE: error_metric_name = "average case relative"; break;
    case MEAN_SQUARED: error_metric_name = "mean squared"; break;
    case ERROR_RATE: error_metric_name = "error rate"; break;
    case AVERAGE_BIT_FLIP: error_metric_name = "average bit flip"; break;
    case WORST_CASE_BIT_FLIP: error_metric_name = "worst case bit flip"; break;
    }
    std::string adder_configuration;
    switch(state.range(1)) {
    case ACA1:
        adder_configuration = "ACA_1_N" + std::to_string(state.range(2)) + "_Q" + std::to_string(state.range(3));
        break;
    case ACA2:
        adder_configuration = "ACA_2_N" + std::to_string(state.range(2)) + "_Q" + std::to_string(state.range(3));
        break;
    case GDA:
        adder_configuration = "GDA_St_N" + std::to_string(state.range(2)) + "_M" + std::to_string(state.range(3)) + "_P" + std::to_string(state.range(4));
        break;
    case GEAR:
        adder_configuration = "GeAr_N" + std::to_string(state.range(2)) + "_R" + std::to_string(state.range(3)) + "_P" + std::to_string(state.range(4));
        break;
    }
    state.SetLabel(adder_configuration + " - " + error_metric_name);
    for (auto _ : state) {
        state.PauseTiming();
        Cudd mgr(state.range(2) + 1);
        auto correct = abo::example_bdds::regular_adder(mgr, state.range(2));
        std::vector<BDD> approximate_adder;
        switch(state.range(1)) {
        case ACA1:
            approximate_adder = abo::example_bdds::almost_correct_adder_1(mgr, state.range(2), state.range(3));
            break;
        case ACA2:
            approximate_adder = abo::example_bdds::almost_correct_adder_2(mgr, state.range(2), state.range(3));
            break;
        case GDA:
            approximate_adder = abo::example_bdds::gracefully_degrading_adder(mgr, state.range(2), state.range(3), state.range(4));
            break;
        case GEAR:
            approximate_adder = abo::example_bdds::generic_accuracy_reconfigurable_adder(mgr, state.range(2), state.range(3), state.range(4));
            break;
        }
        state.ResumeTiming();

        switch (state.range(0)) {
        case WORST_CASE:
            abo::error_metrics::worst_case_error(mgr, correct, approximate_adder);
            break;
        case WORST_CASE_RELATIVE:
            abo::error_metrics::worst_case_relative_error(mgr, correct, approximate_adder);
            break;
        case AVERAGE_CASE:
            abo::error_metrics::average_case_error(mgr, correct, approximate_adder);
            break;
        case AVERAGE_CASE_RELATIVE:
            abo::error_metrics::average_relative_error_bounds(mgr, correct, approximate_adder);
            break;
        case MEAN_SQUARED:
            abo::error_metrics::mean_squared_error(mgr, correct, approximate_adder);
            break;
        case ERROR_RATE:
            abo::error_metrics::error_rate(mgr, correct, approximate_adder);
            break;
        case AVERAGE_BIT_FLIP:
            abo::error_metrics::average_bit_flip_error(correct, approximate_adder);
            break;
        case WORST_CASE_BIT_FLIP:
            abo::error_metrics::worst_case_bit_flip_error(mgr, correct, approximate_adder);
            break;
        }
    }
}

// this re-creates the time table found in "One Method - All Error-Metrics [...]"
BENCHMARK(generate_adders)->Unit(benchmark::kMillisecond)->Apply([](auto *b) {
    std::vector<ErrorMetric> metrics = {WORST_CASE, WORST_CASE_RELATIVE, AVERAGE_CASE, AVERAGE_CASE_RELATIVE, MEAN_SQUARED, ERROR_RATE, AVERAGE_BIT_FLIP, WORST_CASE_BIT_FLIP};
    // 8 bit adders
    for (auto metric : metrics) {
        b = b->Args({metric, ACA1, 8, 5, 0})
                ->Args({metric, ACA2, 8, 4, 0})
                ->Args({metric, GDA, 8, 4, 2})
                ->Args({metric, GDA, 8, 4, 4})
                ->Args({metric, GDA, 8, 8, 1})
                ->Args({metric, GDA, 8, 8, 2})
                ->Args({metric, GDA, 8, 8, 3})
                ->Args({metric, GDA, 8, 8, 4})
                ->Args({metric, GDA, 8, 8, 5})
                ->Args({metric, GEAR, 8, 1, 1})
                ->Args({metric, GEAR, 8, 1, 2})
                ->Args({metric, GEAR, 8, 1, 3})
                ->Args({metric, GEAR, 8, 1, 4})
                ->Args({metric, GEAR, 8, 1, 4})
                ->Args({metric, GEAR, 8, 2, 2})
                ->Args({metric, GEAR, 8, 2, 4});
    }
    // 16 bit adders
    for (auto metric : metrics) {
        // TODO: ETA2 is missing
        b = b->Args({metric, ACA1, 16, 4, 0})
                ->Args({metric, ACA2, 16, 4, 0})
                ->Args({metric, ACA2, 16, 8, 0})
                ->Args({metric, GDA, 16, 4, 4})
                ->Args({metric, GDA, 16, 4, 8})
                ->Args({metric, GEAR, 16, 2, 4})
                ->Args({metric, GEAR, 16, 4, 4})
                ->Args({metric, GEAR, 16, 4, 8})
                ->Args({metric, GEAR, 16, 6, 4});
    }
    // 32 bit adders
    for (auto metric : metrics) {
        b = b->Args({metric, ACA1, 32, 8, 0})
                -> Args({metric, ACA1, 32, 16, 0});
    }
    // TODO: add 64 bit adders as well (not present in the paper)
});

BENCHMARK_MAIN();
