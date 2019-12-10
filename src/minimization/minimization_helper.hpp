#ifndef MINIMIZATION_HELPER_HPP
#define MINIMIZATION_HELPER_HPP

#include "bucket_minimization.hpp"

#include <vector>

namespace abo::minimization {

    enum class OperatorConstructionMode {
        // the operator is applied to a single BDD at a time
        SINGLE_BDD,
        // the operator is applied to the whole BDD forest at the same time
        MULTI_BDD,
        // the operator is applied to a random (single) BDD at a random level
        RANDOM
    };

    struct SimpleMetricDimension {
        std::size_t grid_size;
        ErrorMetric metric;
        double bound;
    };

    struct MinimizationInputInfo {
        // can be either an adder (adder8, adder16 or adder32) or an iscas 85 file
        std::string input;
        // whether to sift the input while reading (or generating) it
        bool sift = true;
        // the metrics to target during the minimzation
        std::vector<SimpleMetricDimension> metrics;
        // the operators used for the approximation
        std::vector<Operator> operators = {Operator::NEGATIVE_COFACTOR, Operator::POSITIVE_COFACTOR};
        // the mode used for constructing the operator application functions
        OperatorConstructionMode operator_mode = OperatorConstructionMode::SINGLE_BDD;
        // only used when operator_mode == RANDOM: the number of operator functions to construct
        std::size_t num_operator_functions;
    };

    struct MinimizationResult {
        // in here to keep all the BDDs valid
        Cudd mgr;
        // the node count of the non-approximated original function
        std::size_t original_size;
        // the time the algorithm took, in milliseconds
        double minimization_time;
        // the result with the smallest BDD found by the algorithm (also present in all_buckets)
        Bucket smallest_function;
        // all buckets created by the algorithm
        std::vector<Bucket> all_buckets;
    };

    MinimizationResult bucket_minimize_helper(const MinimizationInputInfo &info);

}

#endif // MINIMIZATION_HELPER_HPP
