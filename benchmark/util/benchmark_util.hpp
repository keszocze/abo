#ifndef BENCHMARK_UTIL_H
#define BENCHMARK_UTIL_H

#include <cudd/cplusplus/cuddObj.hh>
#include <string>
#include <vector>

namespace abo::benchmark {

enum class ErrorMetric
{
    WORST_CASE,
    WORST_CASE_RELATIVE_APPROX,
    APPROXIMATE_WORST_CASE_5,
    AVERAGE_CASE,
    AVERAGE_RELATIVE_APPROX,
    AVERAGE_RELATIVE_ADD,
    MEAN_SQUARED,
    ERROR_RATE,
    AVERAGE_BIT_FLIP,
    WORST_CASE_BIT_FLIP,
};

/**
 * Returns a human readable string containing the name of the error metric that is given as the enum
 */
std::string error_metric_name(ErrorMetric metric);

/**
 * @brief Computes the error metric for the given functions
 * @param mgr The BDD object manager
 * @param original The original function. As most metrics are symmetric, it can be swapped with the
 * approximated function for them. The only exceptions are the average case relative and worst case
 * relative error
 * @param approx The approximated function. Must be an unsigned integer.
 * @param metric The metric to evaluate
 * @return The metric value. For the approximate metrics, it is the average between the lower and
 * upper bound on the error
 */
double compute_error_metric(const Cudd& mgr, const std::vector<BDD>& original,
                            const std::vector<BDD>& approx, ErrorMetric metric);

enum class ISCAS85File
{
    C17 = 0,
    C432 = 1,
    C499 = 2,
    C880 = 3,
    C1355 = 4,
    C1908 = 5,
    C2670 = 6,
    C3540 = 7,
    C5315 = 8,
    C6288 = 9,
    C7552 = 10
};

/**
 * Returns the filename of the iscas 85 file identified the the enum input
 */
std::string iscas_85_filename_by_id(ISCAS85File file);

/**
 * @brief Loads an iscas 85 benchmark and returns it as a BDD vector
 * @param mgr The BDD object manager
 * @param file The file to load
 * @return The iscas 85 benchmark
 */
std::vector<BDD> load_iscas_85_file(Cudd& mgr, ISCAS85File file);

enum class ApproximateAdder
{
    ACA1,
    ACA2,
    GDA,
    GEAR
};

/**
 * @brief Returns a shorthand form of the name of the approximate adder and its exact configuration
 * @param adder The adder to use
 * @param bits The number of bits the adder should have
 * @param par1 The first parameter of the approximate adder. This has different meanings for
 * different adders
 * @param par2 (optional) The second parameter of the approximate adder. This has different meanings
 * for different adders
 * @return A string containing the name of the configuration
 */
std::string approximate_adder_name(ApproximateAdder adder, std::size_t bits, std::size_t par1,
                                   std::size_t par2 = 0);

/**
 * @brief Creates the approximate adder specified by the parameters
 * @param mgr The BDD object manager
 * @param adder The approximate adder type to create
 * @param bits The number of bits the adder should have
 * @param par1 The first parameter of the approximate adder. This has different meanings for
 * different adders
 * @param par2 par2 (optional) The second parameter of the approximate adder. This has different
 * meanings for different adders
 * @return The approximate adder
 */
std::vector<BDD> get_approximate_adder(Cudd& mgr, ApproximateAdder adder, std::size_t bits,
                                       std::size_t par1, std::size_t par2 = 0);

} // namespace abo::benchmark

#endif // BENCHMARK_UTIL_H
