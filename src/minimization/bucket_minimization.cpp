#include "bucket_minimization.hpp"

#include <array>
#include <exception>
#include <functional>
#include <numeric>
#include <set>
#include <tuple>
#include <vector>

#include <cudd/cplusplus/cuddObj.hh>

#include "approximation_operators.hpp"
#include "average_bit_flip_error.hpp"
#include "average_case_error.hpp"
#include "average_case_relative_error.hpp"
#include "cudd_helpers.hpp"
#include "error_rate.hpp"
#include "worst_case_bit_flip_error.hpp"
#include "worst_case_error.hpp"
#include "worst_case_relative_error.hpp"

namespace abo::minimization {

std::size_t reduce_multi_dim_index(const std::vector<std::size_t>& index,
                                   const std::vector<std::size_t>& bounds)
{
    std::size_t result = 0;
    for (std::size_t i = 0; i < bounds.size(); i++)
    {
        result *= bounds[i];
        result += index[i];
    }
    return result;
}

std::vector<std::size_t>
create_multi_dim_index(std::size_t index,
                        const std::vector<std::size_t>& bounds)
{
    std::vector<std::size_t> result(bounds.size());
    for (std::size_t i = bounds.size() - 1; i < bounds.size(); i--)
    {
        result[i] = index % bounds[i];
        index /= bounds[i];
    }
    return result;
}

std::vector<Bucket>
bucket_greedy_minimize(Cudd& mgr,
                       const std::vector<BDD>& function,
                       const std::vector<MetricDimension>& metrics,
                       const std::vector<OperatorFunction>& operators,
                       const bool populate_all_buckets)
{

    std::size_t num_metrics = metrics.size();
    if (num_metrics == 0)
    {
        throw std::invalid_argument("At least one metric must be specified");
    }

    std::vector<std::size_t> bucket_grid_size;
    for (const auto& metric : metrics)
    {
        bucket_grid_size.push_back(metric.grid_size);
    }

    std::size_t total_buckets = std::accumulate(bucket_grid_size.begin(),
                                                bucket_grid_size.end(),
                                                1UL,
                                                std::multiplies<std::size_t>());

    std::size_t node_count = static_cast<std::size_t>(mgr.nodeCount(function));
    std::vector<Bucket> buckets(total_buckets,
                                {function,
                                 node_count,
                                 std::vector<double>(metrics.size(), 0),
                                 std::vector<bool>(operators.size(), true)});

    std::set<std::vector<std::size_t>> test;
    test.insert(create_multi_dim_index(0, bucket_grid_size));

    while (test.size() > 0)
    {

        auto smallest_pos = *std::min_element(
            test.begin(), test.end(),
            [](const std::vector<std::size_t>& a, const std::vector<std::size_t>& b) {
                return std::accumulate(a.begin(), a.end(), 0.) <
                       std::accumulate(b.begin(), b.end(), 0.);
            });
        test.erase(smallest_pos);

        std::size_t current_index = reduce_multi_dim_index(smallest_pos, bucket_grid_size);

        // a copy is necessary as the current bucket might get overwritten
        const std::vector<BDD> bucket_function = buckets[current_index].function;
        std::vector<bool> bucket_possible_operators = buckets[current_index].possible_operators;
        std::map<std::size_t, std::size_t> replace_possible_operators;

        for (std::size_t opnum = 0; opnum < operators.size(); opnum++)
        {
            const auto op = operators[opnum];

            if (!bucket_possible_operators[opnum])
            {
                continue;
            }

            std::vector<BDD> modified = bucket_function;
            op(mgr, modified);

            std::size_t nodes = static_cast<std::size_t>(mgr.nodeCount(modified));
            if (nodes >= buckets[current_index].bdd_size)
            {
                if (nodes > buckets[current_index].bdd_size * 3 / 2)
                {
                    bucket_possible_operators[opnum] = false;
                }
                continue;
            }

            // compute metric values
            bool better = true;
            std::vector<std::size_t> new_bucket_index(num_metrics, 0);
            std::vector<double> metric_values(num_metrics);
            for (std::size_t i = 0; i < num_metrics; i++)
            {
                double error = metrics[i].metric(mgr, function, modified);
                std::size_t dimension_index =
                    std::size_t(bucket_grid_size[i] * error / metrics[i].bound);
                if (dimension_index >= bucket_grid_size[i])
                {
                    better = false;
                    bucket_possible_operators[opnum] = false;
                    break;
                }
                new_bucket_index[i] = dimension_index;
                auto partial_index = reduce_multi_dim_index(new_bucket_index, bucket_grid_size);
                // early stopping, no need to compute the other metrics, it can not get any better
                if (buckets[partial_index].bdd_size <= nodes)
                {
                    better = false;
                    break;
                }
                metric_values[i] = error;
            }
            if (!better)
            {
                continue;
            }

            // update buckets with new function
            std::size_t reduced_bucket_start =
                reduce_multi_dim_index(new_bucket_index, bucket_grid_size);
            std::size_t bucket_end =
                populate_all_buckets ? buckets.size() : reduced_bucket_start + 1;
            for (std::size_t i = reduced_bucket_start; i < bucket_end; i++)
            {
                if (buckets[i].bdd_size <= nodes)
                {
                    continue;
                }
                auto index = create_multi_dim_index(i, bucket_grid_size);
                bool invalid = false;
                for (std::size_t b = 0; b < num_metrics; b++)
                {
                    if (index[b] < new_bucket_index[b])
                    {
                        invalid = true;
                        break;
                    }
                }
                if (invalid)
                {
                    continue;
                }
                replace_possible_operators[i] = opnum;
                buckets[i].function = modified;
                buckets[i].bdd_size = nodes;
                buckets[i].metric_values = metric_values;
                test.insert(index);
            }
        }

        // only insert this after the operator loop to have the biggest set of operators removed
        for (auto [bucket, op] : replace_possible_operators)
        {
            buckets[bucket].possible_operators = bucket_possible_operators;
            buckets[bucket].possible_operators[op] = false;
        }
    }

    return buckets;
}

BDD apply_operator(const Cudd& mgr, BDD& b, Operator op, unsigned int level_start,
                   unsigned int level_end)
{
    switch (op)
    {
    case Operator::POSITIVE_COFACTOR:
        return b.Cofactor(mgr.bddVar(static_cast<int>(level_start)));
    case Operator::NEGATIVE_COFACTOR:
        return b.Cofactor(!mgr.bddVar(static_cast<int>(level_start)));
    case Operator::SUBSET_LIGHT:
        return abo::operators::subset_light_child(mgr, b, level_start, level_end);
    case Operator::SUPERSET_HEAVY:
        return abo::operators::superset_heavy_child(mgr, b, level_start, level_end);
    case Operator::SUBSET_HEAVY:
        return abo::operators::subset_heavy_child(mgr, b, level_start, level_end);
    case Operator::SUPERSET_LIGHT:
        return abo::operators::superset_light_child(mgr, b, level_start, level_end);
    case Operator::ROUND_BEST:
        return abo::operators::round_best(mgr, b, level_start, level_end);
    case Operator::ROUND:
        return abo::operators::round_bdd(mgr, b, level_start);
    default: return b;
    }
    return b;
}

void apply_operator(const Cudd& mgr, std::vector<BDD>& function, Operator op,
                    unsigned int level_start, unsigned int level_end)
{
    for (std::size_t i = 0; i < function.size(); i++)
    {
        function[i] = apply_operator(mgr, function[i], op, level_start, level_end);
    }
}

std::vector<OperatorFunction>
generate_single_bdd_operators(const std::vector<BDD>& function,
                                std::vector<Operator> operators)
{
    std::vector<OperatorFunction> result;
    unsigned int top_level = abo::util::terminal_level({function});
    for (unsigned int i = 0; i < function.size(); i++)
    {
        for (unsigned int j = 0; j < top_level; j++)
        {
            for (auto op : operators)
            {
                result.push_back([=](Cudd& manager, std::vector<BDD>& f) {
                    f[i] = apply_operator(manager, f[i], op, j, j);
                });
            }
        }
    }
    return result;
}

std::vector<OperatorFunction>
generate_multi_bdd_operators(const std::vector<BDD>& function,
                                std::vector<Operator> operators)
{
    std::vector<OperatorFunction> result;
    unsigned int top_level = abo::util::terminal_level({function});
    for (unsigned int j = 0; j < top_level; j++)
    {
        for (auto op : operators)
        {
            result.push_back([=](Cudd& manager, std::vector<BDD>& f) {
                for (unsigned int i = 0; i < f.size(); i++)
                {
                    f[i] = apply_operator(manager, f[i], op, j, j);
                }
            });
        }
    }
    return result;
}

std::vector<OperatorFunction>
generate_random_operators(const std::vector<BDD>& function,
                            std::vector<Operator> operators,
                            std::size_t count)
{
    std::vector<OperatorFunction> result;
    unsigned int top_level = abo::util::terminal_level({function});
    for (std::size_t i = 0; i < count; i++)
    {
        Operator op = operators[static_cast<std::size_t>(rand()) % operators.size()];
        unsigned int level = static_cast<unsigned int>(rand()) % top_level;
        result.push_back([=](Cudd& manager, std::vector<BDD>& f) {
            for (unsigned int i = 0; i < f.size(); i++)
            {
                f[i] = apply_operator(manager, f[i], op, level, level);
            }
        });
    }
    return result;
}

std::string metric_to_string(ErrorMetric metric)
{
    switch (metric)
    {
    case ErrorMetric::WORST_CASE: return "worst case";
    case ErrorMetric::WORST_CASE_PERCENT: return "worst case percent";
    case ErrorMetric::WORST_CASE_RELATIVE: return "worst case relative";
    case ErrorMetric::AVERAGE_CASE: return "average case";
    case ErrorMetric::AVERAGE_CASE_RELATIVE: return "average case relative";
    case ErrorMetric::AVERAGE_CASE_RELATIVE_ADD: return "acr - ADD";
    case ErrorMetric::MEAN_SQUARED: return "mean squared";
    case ErrorMetric::ERROR_RATE: return "error rate";
    case ErrorMetric::AVERAGE_BIT_FLIP: return "average bit flip";
    case ErrorMetric::WORST_CASE_BIT_FLIP: return "worst case bit flip";
    default: return "";
    }
    return "";
}

std::function<double(Cudd&, const std::vector<BDD>&, const std::vector<BDD>&)>
metric_function(ErrorMetric metric)
{

    switch (metric)
    {
    case ErrorMetric::WORST_CASE:
        return [](Cudd& mgr, const std::vector<BDD>& orig, const std::vector<BDD>& approx) {
            return static_cast<double>(abo::error_metrics::worst_case_error(mgr, orig, approx));
        };
    case ErrorMetric::WORST_CASE_PERCENT:
        return [](Cudd& mgr, const std::vector<BDD>& orig, const std::vector<BDD>& approx) {
            return static_cast<double>(
                abo::error_metrics::worst_case_error_percent(mgr, orig, approx));
        };
    case ErrorMetric::WORST_CASE_RELATIVE:
        return [](Cudd& mgr, const std::vector<BDD>& orig, const std::vector<BDD>& approx) {
            return static_cast<double>(
                abo::error_metrics::wcre_add(mgr, orig, approx));
        };
    case ErrorMetric::AVERAGE_CASE:
        return [](Cudd& mgr, const std::vector<BDD>& orig, const std::vector<BDD>& approx) {
            return static_cast<double>(abo::error_metrics::average_case_error(mgr, orig, approx));
        };
    case ErrorMetric::AVERAGE_CASE_RELATIVE:
        return [](Cudd& mgr, const std::vector<BDD>& orig, const std::vector<BDD>& approx) {
            return static_cast<double>(
                abo::error_metrics::acre_add(mgr, orig, approx));
        };
    case ErrorMetric::AVERAGE_CASE_RELATIVE_ADD:
        return [](Cudd& mgr, const std::vector<BDD>& orig, const std::vector<BDD>& approx) {
            return static_cast<double>(
                abo::error_metrics::acre_add(mgr, orig, approx));
        };
    case ErrorMetric::MEAN_SQUARED:
        return [](Cudd& mgr, const std::vector<BDD>& orig, const std::vector<BDD>& approx) {
            return static_cast<double>(abo::error_metrics::mean_squared_error(mgr, orig, approx));
        };
    case ErrorMetric::ERROR_RATE:
        return [](Cudd& mgr, const std::vector<BDD>& orig, const std::vector<BDD>& approx) {
            return static_cast<double>(abo::error_metrics::error_rate(mgr, orig, approx));
        };
    case ErrorMetric::AVERAGE_BIT_FLIP:
        return [](Cudd&, const std::vector<BDD>& orig, const std::vector<BDD>& approx) {
            return static_cast<double>(abo::error_metrics::average_bit_flip_error(orig, approx));
        };
    case ErrorMetric::WORST_CASE_BIT_FLIP:
        return [](Cudd& mgr, const std::vector<BDD>& orig, const std::vector<BDD>& approx) {
            return static_cast<double>(
                abo::error_metrics::worst_case_bit_flip_error(mgr, orig, approx));
        };
    default: throw std::logic_error("switch does not handle all cases");
    }
    throw std::logic_error("switch does not handle all cases");
}

std::string operator_to_string(Operator op)
{
    switch (op)
    {
    case Operator::ROUND: return "round";
    case Operator::ROUND_BEST: return "round best";
    case Operator::SUPERSET_HEAVY: return "superset heavy";
    case Operator::SUBSET_LIGHT: return "subset light";
    case Operator::SUPERSET_LIGHT: return "superset light";
    case Operator::SUBSET_HEAVY: return "subset heavy";
    case Operator::POSITIVE_COFACTOR: return "cof+";
    case Operator::NEGATIVE_COFACTOR: return "cof-";
    default: return "";
    }
    return "";
}

} // namespace abo::minimization
