#include "minimization_helper.hpp"

#include <chrono>

#include "aig_parser.hpp"
#include "approximate_adders.hpp"

namespace abo::minimization {

    std::pair<Cudd, std::vector<BDD>> load_adder(unsigned int bits) {
        Cudd mgr(2 * bits + 1);
        return {mgr, abo::example_bdds::regular_adder(mgr, bits)};
    }

    std::pair<Cudd, std::vector<BDD>> load_file(std::string file, bool reorder) {
        Cudd mgr(40);
        if (reorder) {
            mgr.AutodynEnable();
        }
        abo::parser::aig_parser parser(mgr);
        // TODO: although this path works, maybe make it more sensible?
        lorina::read_aiger("../benchmark/iscas85/" + file + ".aig", parser);
        mgr.AutodynDisable();

        return {mgr, parser.get_outputs()};
    }

    std::pair<Cudd, std::vector<BDD>> load_input(std::string input, bool sift) {
        if (input == "adder8") {
            return load_adder(8);
        } else if (input == "adder16") {
            return load_adder(16);
        } else if (input == "adder32") {
            return load_adder(32);
        } else {
            return load_file(input, sift);
        }
    }

    MinimizationResult bucket_minimize_helper(const MinimizationInputInfo &info) {
        auto [mgr, function] = load_input(info.input, info.sift);
        std::vector<MetricDimension> metrics;
        for (auto m : info.metrics) {
            MetricDimension dim;
            dim.bound = m.bound;
            dim.grid_size = m.grid_size;
            dim.metric = metric_function(m.metric);
            metrics.push_back(dim);
        }
        std::vector<OperatorFunction> operator_functions;
        switch (info.operator_mode) {
        case OperatorConstructionMode::SINGLE_BDD:
            operator_functions = generate_single_bdd_operators(function, info.operators);
            break;
        case OperatorConstructionMode::MULTI_BDD:
            operator_functions = generate_multi_bdd_operators(function, info.operators);
            break;
        case OperatorConstructionMode::RANDOM:
            operator_functions = generate_random_operators(function, info.operators, info.num_operator_functions);
            break;
        }

        auto before = std::chrono::high_resolution_clock::now();

        auto buckets = bucket_greedy_minimize(mgr, function, metrics, operator_functions, info.populate_all_buckets);

        auto after = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> minimization_time = std::chrono::duration_cast<std::chrono::duration<double>>(after - before);

        MinimizationResult result;
        result.mgr = mgr;
        result.original_size = static_cast<std::size_t>(mgr.nodeCount(function));
        result.minimization_time = minimization_time.count() * 1000.0;
        result.smallest_function = *std::min_element(buckets.begin(), buckets.end(), [](Bucket &a, Bucket &b) { return a.bdd_size < b.bdd_size; });
        result.all_buckets = buckets;

        return result;
    }

    std::string format_time(double milliseconds) {
        if (milliseconds < 1000.0) {
            return std::to_string(static_cast<int>(milliseconds)) + " ms";
        } else if (milliseconds < 300000.0) {
            return std::to_string(static_cast<int>(milliseconds / 1000.0)) + " s";
        } else {
            return std::to_string(static_cast<int>(milliseconds / 60000.0)) + " m";
        }
    }

}
