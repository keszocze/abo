#ifndef MINIMIZATION_HELPER_HPP
#define MINIMIZATION_HELPER_HPP

#include "bucket_minimization.hpp"

#include <string>
#include <vector>

namespace abo::minimization {

enum class OperatorConstructionMode
{
    //! the operator is applied to a single BDD at a time
    SINGLE_BDD,
    //! the operator is applied to the whole BDD forest at the same time
    MULTI_BDD,
    //! the operator is applied to a random (single) BDD at a random level
    RANDOM
};

//! A simplified version of the struct MetricDimension with an enum for the metric instead of a
//! lambda
struct SimpleMetricDimension
{
    //! The number of buckets to create for this metric in the given dimension
    std::size_t grid_size;
    //! The error metric used for this dimension
    ErrorMetric metric;
    //! The maximum value of the error metric that is represented by a bucket
    double bound;
};

//! Stores a configuration for easily running simple tests with the bucket based minimization
//! algorithm
struct MinimizationInputInfo
{
    //! can be either an adder (adder8, adder16 or adder32) or an iscas 85 file
    std::string input;
    //! whether to sift the input while reading (or generating) it
    bool sift = true;
    //! the metrics to target during the minimzation
    std::vector<SimpleMetricDimension> metrics;
    //! the operators used for the approximation
    std::vector<Operator> operators = {Operator::NEGATIVE_COFACTOR, Operator::POSITIVE_COFACTOR};
    //! the mode used for constructing the operator application functions
    OperatorConstructionMode operator_mode = OperatorConstructionMode::SINGLE_BDD;
    //! only used when operator_mode == RANDOM: the number of operator functions to construct
    std::size_t num_operator_functions;
    //! whether or not the algorithm should populate all buckets with a higher node count (if the
    //! error metrics match) or only the exactly matching one the algorithm generally performs
    //! slightly better when only the exact bucket is populated
    bool populate_all_buckets = false;
};

//! Stores the result of a BDD minimization by the bucket based algorithm
struct MinimizationResult
{
    //! in here to keep all the BDDs valid
    Cudd mgr;
    //! the node count of the non-approximated original function
    std::size_t original_size;
    //! the time the algorithm took, in milliseconds
    double minimization_time;
    //! the result with the smallest BDD found by the algorithm (also present in all_buckets)
    Bucket smallest_function;
    //! all buckets created by the algorithm
    std::vector<Bucket> all_buckets;
};

MinimizationResult bucket_minimize_helper(const MinimizationInputInfo& info);

std::string format_time(double milliseconds);

} // namespace abo::minimization

#endif // MINIMIZATION_HELPER_HPP
