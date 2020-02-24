#include <benchmark/benchmark.h>
#include <cudd/cplusplus/cuddObj.hh>

#include "benchmark_util.hpp"
#include "approximate_adders.hpp"

using namespace abo::benchmark;

// input: error metric, adder generator, parameters for that generator
static void generate_adders(benchmark::State& state) {

    const ErrorMetric metric = static_cast<ErrorMetric>(state.range(0));
    const ApproximateAdder adder = static_cast<ApproximateAdder>(state.range(1));

    std::string error_metric_name = abo::benchmark::error_metric_name(metric);
    std::string adder_configuration = approximate_adder_name(adder, state.range(2), state.range(3), state.range(4));

    state.SetLabel(adder_configuration + " - " + error_metric_name);
    for (auto _ : state) {
        state.PauseTiming();
        Cudd mgr(state.range(2) + 1);
        auto correct = abo::example_bdds::regular_adder(mgr, state.range(2));
        std::vector<BDD> approximate_adder = get_approximate_adder(mgr, adder, state.range(2), state.range(3), state.range(4));
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
        b = b->Args({metric, static_cast<int>(ApproximateAdder::ACA1), 8, 5, 0})
                ->Args({metric, static_cast<int>(ApproximateAdder::ACA2), 8, 4, 0})
                ->Args({metric, static_cast<int>(ApproximateAdder::GDA), 8, 4, 2})
                ->Args({metric, static_cast<int>(ApproximateAdder::GDA), 8, 4, 4})
                ->Args({metric, static_cast<int>(ApproximateAdder::GDA), 8, 8, 1})
                ->Args({metric, static_cast<int>(ApproximateAdder::GDA), 8, 8, 2})
                ->Args({metric, static_cast<int>(ApproximateAdder::GDA), 8, 8, 3})
                ->Args({metric, static_cast<int>(ApproximateAdder::GDA), 8, 8, 4})
                ->Args({metric, static_cast<int>(ApproximateAdder::GDA), 8, 8, 5})
                ->Args({metric, static_cast<int>(ApproximateAdder::GEAR), 8, 1, 1})
                ->Args({metric, static_cast<int>(ApproximateAdder::GEAR), 8, 1, 2})
                ->Args({metric, static_cast<int>(ApproximateAdder::GEAR), 8, 1, 3})
                ->Args({metric, static_cast<int>(ApproximateAdder::GEAR), 8, 1, 4})
                ->Args({metric, static_cast<int>(ApproximateAdder::GEAR), 8, 1, 5})
                ->Args({metric, static_cast<int>(ApproximateAdder::GEAR), 8, 2, 2})
                ->Args({metric, static_cast<int>(ApproximateAdder::GEAR), 8, 2, 4});
    }
    // 16 bit adders
    for (auto metric_ : metrics) {
        int metric = static_cast<int>(metric_);
        // TODO: ETA2 is missing
        b = b->Args({metric, static_cast<int>(ApproximateAdder::ACA1), 16, 4, 0})
                ->Args({metric, static_cast<int>(ApproximateAdder::ACA2), 16, 4, 0})
                ->Args({metric, static_cast<int>(ApproximateAdder::ACA2), 16, 8, 0})
                ->Args({metric, static_cast<int>(ApproximateAdder::GDA), 16, 4, 4})
                ->Args({metric, static_cast<int>(ApproximateAdder::GDA), 16, 4, 8})
                ->Args({metric, static_cast<int>(ApproximateAdder::GEAR), 16, 2, 4})
                ->Args({metric, static_cast<int>(ApproximateAdder::GEAR), 16, 4, 4})
                ->Args({metric, static_cast<int>(ApproximateAdder::GEAR), 16, 4, 8})
                ->Args({metric, static_cast<int>(ApproximateAdder::GEAR), 16, 6, 4});
    }
    // 32 bit adders
    for (auto metric_ : metrics) {
        int metric = static_cast<int>(metric_);
        b = b->Args({metric, static_cast<int>(ApproximateAdder::ACA1), 32, 8, 0})
                -> Args({metric, static_cast<int>(ApproximateAdder::ACA1), 32, 16, 0});
    }
    // TODO: add 64 bit adders as well (not present in the paper)
});

BENCHMARK_MAIN();
