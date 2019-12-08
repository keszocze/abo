#include <benchmark/benchmark.h>
#include <cudd/cplusplus/cuddObj.hh>

#include "benchmark_util.hpp"
#include "approximate_adders.hpp"

using namespace abo::benchmark;

enum AdderGenerators {
    ACA1,
    ACA2,
    GDA,
    GEAR
};

// input: error metric, adder generator, parameters for that generator
static void generate_adders(benchmark::State& state) {

    const ErrorMetric metric = static_cast<ErrorMetric>(state.range(0));

    std::string error_metric_name = abo::benchmark::error_metric_name(metric);

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

        abo::benchmark::compute_error_metric(mgr, correct, approximate_adder, metric);
    }
}

// this re-creates the time table found in "One Method - All Error-Metrics [...]"
BENCHMARK(generate_adders)->Unit(benchmark::kMillisecond)->Apply([](auto *b) {
    std::vector<ErrorMetric> metrics = {ErrorMetric::WORST_CASE, ErrorMetric::WORST_CASE_RELATIVE_APPROX, ErrorMetric::AVERAGE_CASE,
                                        ErrorMetric::AVERAGE_RELATIVE_APPROX, ErrorMetric::MEAN_SQUARED, ErrorMetric::ERROR_RATE,
                                        ErrorMetric::AVERAGE_BIT_FLIP, ErrorMetric::WORST_CASE_BIT_FLIP};
    // 8 bit adders
    for (auto metric_ : metrics) {
        int metric = static_cast<int>(metric_);
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
    for (auto metric_ : metrics) {
        int metric = static_cast<int>(metric_);
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
    for (auto metric_ : metrics) {
        int metric = static_cast<int>(metric_);
        b = b->Args({metric, ACA1, 32, 8, 0})
                -> Args({metric, ACA1, 32, 16, 0});
    }
    // TODO: add 64 bit adders as well (not present in the paper)
});

BENCHMARK_MAIN();
