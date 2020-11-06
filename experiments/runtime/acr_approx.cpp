#include <benchmark/benchmark.h>
#include <cudd/cplusplus/cuddObj.hh>

#include "approximate_adders.hpp"
#include "benchmark_util.hpp"

using namespace abo::benchmark;

// input: adder generator, parameters for that generator
static void acr_adders(benchmark::State& state)
{

    const ApproximateAdder adder = static_cast<ApproximateAdder>(state.range(0));

    std::string adder_configuration =
        approximate_adder_name(adder, state.range(1), state.range(2), state.range(3));

    state.SetLabel(adder_configuration);
    for (auto _ : state)
    {
        state.PauseTiming();
        Cudd mgr(state.range(1) + 1);
        auto correct = abo::example_bdds::regular_adder(mgr, state.range(1));
        std::vector<BDD> approximate_adder =
            get_approximate_adder(mgr, adder, state.range(1), state.range(2), state.range(3));
        state.ResumeTiming();

        abo::benchmark::compute_error_metric(mgr, correct, approximate_adder,
                                             ErrorMetric::AVERAGE_RELATIVE_APPROX);
    }
}

BENCHMARK(acr_adders)->Unit(benchmark::kMillisecond)->Apply([](auto* b) {
    b->Args({static_cast<int>(ApproximateAdder::ACA1), 8, 5, 0})
        ->Args({static_cast<int>(ApproximateAdder::ACA2), 8, 4, 0})
        ->Args({static_cast<int>(ApproximateAdder::GEAR), 8, 1, 5})
        ->Args({static_cast<int>(ApproximateAdder::GEAR), 8, 2, 2})
        ->Args({static_cast<int>(ApproximateAdder::ACA1), 16, 4, 0})
        ->Args({static_cast<int>(ApproximateAdder::ACA2), 16, 4, 0})
        ->Args({static_cast<int>(ApproximateAdder::ACA2), 16, 8, 0})
        ->Args({static_cast<int>(ApproximateAdder::ACA1), 32, 8, 0})
        ->Args({static_cast<int>(ApproximateAdder::ACA2), 32, 16, 0});
});

BENCHMARK_MAIN();
