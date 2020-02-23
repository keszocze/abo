#ifndef BUCKET_MINIMIZATION_H
#define BUCKET_MINIMIZATION_H

#include <vector>
#include <functional>
#include <string>

#include <cudd/cplusplus/cuddObj.hh>

namespace abo::minimization {

    enum class Operator {
        POSITIVE_COFACTOR = 0,
        NEGATIVE_COFACTOR = 1,
        SUBSET_LIGHT = 2,
        SUBSET_HEAVY = 3,
        SUPERSET_LIGHT = 4,
        SUPERSET_HEAVY = 5,
        ROUND_BEST = 6,
        ROUND = 7
    };

    typedef std::function<void(Cudd&, std::vector<BDD>&)> OperatorFunction;

    std::string operator_to_string(Operator op);

    BDD apply_operator(const Cudd &mgr, BDD &b, Operator op, unsigned int level_start, unsigned int level_end);

    std::vector<OperatorFunction> generate_single_bdd_operators(const std::vector<BDD> &function, std::vector<Operator> operators);

    std::vector<OperatorFunction> generate_multi_bdd_operators(const std::vector<BDD> &function, std::vector<Operator> operators);

    std::vector<OperatorFunction> generate_random_operators(const std::vector<BDD> &function, std::vector<Operator> operators,
                                                            std::size_t count);

    enum class ErrorMetric {
        WORST_CASE,
        WORST_CASE_PERCENT,
        WORST_CASE_RELATIVE,
        AVERAGE_CASE,
        AVERAGE_CASE_RELATIVE,
        AVERAGE_CASE_RELATIVE_ADD,
        MEAN_SQUARED,
        ERROR_RATE,
        AVERAGE_BIT_FLIP,
        WORST_CASE_BIT_FLIP,
    };

    typedef std::function<double(Cudd&, const std::vector<BDD>&, const std::vector<BDD>&)> MetricFunction;

    std::string metric_to_string(ErrorMetric metric);

    MetricFunction metric_function(ErrorMetric metric);


    struct MetricDimension {
        std::size_t grid_size;
        MetricFunction metric;
        double bound;
    };

    struct Bucket {
        std::vector<BDD> function;
        std::size_t bdd_size;
        std::vector<double> metric_values;
        std::vector<bool> possible_operators;
    };

    std::vector<Bucket> bucket_greedy_minimize(Cudd &mgr,
                           const std::vector<BDD> &function,
                           const std::vector<MetricDimension> &metrics,
                           const std::vector<OperatorFunction> &operators,
                           const bool populate_all_buckets);

}

#endif // BUCKET_MINIMIZATION_H
