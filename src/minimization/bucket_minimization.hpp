#ifndef BUCKET_MINIMIZATION_H
#define BUCKET_MINIMIZATION_H

#include <functional>
#include <string>
#include <vector>

#include <cudd/cplusplus/cuddObj.hh>

namespace abo::minimization {

enum class Operator
{
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

//! Returns a human readable string version of the enum value passed as argument
std::string operator_to_string(Operator op);

/**
 * @brief apply_operator Apply the given approximation operator to b
 * @param mgr The Cudd object manager b and the result are managed by
 * @param b The BDD to approximate
 * @param op The approximation operator to apply
 * @param level_start The variable level to start the approximation at. Is zero-indexed
 * @param level_end The last level the operator should be applied at. b is approximated for all
 * variable levels between level_start and level_end
 * @return The approximated BDD
 */
BDD apply_operator(const Cudd& mgr, BDD& b, Operator op, unsigned int level_start,
                   unsigned int level_end);

/**
 * @brief generate_single_bdd_operators Generate a set of operator application functions.
 * For each variable level in the function, each bit in the function and each operator,
 * one approximation function is created which applies the operator at exactly that level to the
 * given BDD.
 * @param function The original function that is later approximated. Only the support size and the
 * number of bits are used, the function to approximate is later passed to each OperatorFunction
 * individually
 * @param operators The set approximation operators to use
 * @return The list of approximation operator functions
 */
std::vector<OperatorFunction> generate_single_bdd_operators(const std::vector<BDD>& function,
                                                            std::vector<Operator> operators);

/**
 * @brief generate_multi_bdd_operators Generate a set of operator application functions.
 * For each variable level in the function and each operator,
 * one approximation function is created which applies the operator at exactly that level to all
 * bits in the input.
 * @param function The original function that is later approximated. Only the support size is used,
 * the function to approximate is later passed to each OperatorFunction individually
 * @param operators The set approximation operators to use
 * @return The list of approximation operator functions
 */
std::vector<OperatorFunction> generate_multi_bdd_operators(const std::vector<BDD>& function,
                                                           std::vector<Operator> operators);

/**
 * @brief generate_random_operators Generates a set of approximation operator functions of size
 * count. This is very similar to generate_single_bdd_operators, except that the variable level, bit
 * and operator are randomly selected upon calling the lambda instead of being hardcoded. The same
 * function is therefore repeated in the result. count (and therefore the size of the result) is
 * only used to influence the bucket based minimization algorithm.
 * @param function The original function that is later approximated. Only the support size and the
 * number of bits are used, the function to approximate is later passed to each OperatorFunction
 * individually
 * @param operators The set approximation operators to use
 * @param count The number of copies of the function to return in the result
 * @return The list of created approximation operator functions
 */
std::vector<OperatorFunction> generate_random_operators(const std::vector<BDD>& function,
                                                        std::vector<Operator> operators,
                                                        std::size_t count);

enum class ErrorMetric
{
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

typedef std::function<double(Cudd&, const std::vector<BDD>&, const std::vector<BDD>&)>
    MetricFunction;

//! Returns a human readable string version of the enum value passed as argument
std::string metric_to_string(ErrorMetric metric);

//! Returns a lambda computing the error metric given by the argument
MetricFunction metric_function(ErrorMetric metric);

//! Holds the configuration for one dimension of the buckets created
struct MetricDimension
{
    //! The number of buckets to create for this metric in the given dimension
    std::size_t grid_size;
    //! The function that computes the error metric representing this dimension
    MetricFunction metric;
    //! The maximum value of the error metric that is represented by a bucket
    double bound;
};

//! A struct holding the information about a bucket. This is also returned as the result of the
//! minimization procedure
struct Bucket
{
    //! The function currently occupying this bucket. It represents the currently best result found
    //! for this bucket
    std::vector<BDD> function;
    //! The number of nodes of the function
    std::size_t bdd_size;
    //! The value of every metric computed for the function
    std::vector<double> metric_values;
    //! For each operator given to the optimization procedure, this stores wether it can still be
    //! applied to the function
    std::vector<bool> possible_operators;
};

/**
 * @brief bucket_greedy_minimize Minimized BDD forests with a greedy bucket based algorithm.
 * A set of buckets is created, quantizing the space of the given error metrics up the maximum error
 * bound. Each bucket can only be inhabited by one BDD, the one with the lowest node cound found in
 * this error metric range. When a better result for a bucket is found, the given approximation
 * operator functions are applied to it to create more (possibly better) approximations.
 * @param mgr The cudd node manager to create nodes in
 * @param function The function to approximate
 * @param metrics The metrics used for the minimization procedure. It is recommended to keep the
 * number of metrics fairly low to avoid an explosion in the number of buckets created by the
 * procedure
 * @param operators The approximation operators to be used. Can for example be created by
 * generate_single_bdd_operators and similar helper functions
 * @param populate_all_buckets When a better approximation for a bucket is found, this flag
 * determines if the result is written into the exact bucket only (false) or all buckets with higher
 * error metrics and larger node counts.
 * @return A list of buckets created by the procedure. They represent a pareto front of the
 * minimization task
 */
std::vector<Bucket> bucket_greedy_minimize(Cudd& mgr, const std::vector<BDD>& function,
                                           const std::vector<MetricDimension>& metrics,
                                           const std::vector<OperatorFunction>& operators,
                                           const bool populate_all_buckets);

} // namespace abo::minimization

#endif // BUCKET_MINIMIZATION_H
