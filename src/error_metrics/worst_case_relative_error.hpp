#pragma once

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <cudd/cplusplus/cuddObj.hh>
#include <vector>

#include "number_representation.hpp"

namespace abo::error_metrics {

/**
 * @brief Computes the maximum relative difference between f and f_hat for any input
 * It is defined as the maximum of |f(x) - f_hat(x)| / max(1, |f(x)|) over all inputs x
 * As it is not symmetric, it is not a metric in the mathematical sense
 * The computation is performed with ADDs and might be quite slow
 * @param mgr The BDD object manager
 * @param f The original function
 * @param f_hat The approximated function. Must have the same number of bits as f
 * @param num_rep The number representation for f and f_hat
 * @return the maximum relative difference of the inputs
 */
double wcre_add(const Cudd& mgr, const std::vector<BDD>& f,
              const std::vector<BDD>& f_hat,
              const abo::util::NumberRepresentation num_rep = abo::util::NumberRepresentation::BaseTwo);

/**
 * @brief Computes bounds on the maximum relative value of f in relation to g
 * It is defined as the maximum of |f(x)| / max(1, |g(x)|) over all inputs x
 * This function returns a range in which the actual relative value is guaranteed to lie
 * The computed bounds are always within a factor of 2, meaning that the maximum bound
 * returned by this function is at most twice the minimum bound
 * @param mgr The BDD object manager
 * @param f The function to compute the maximum relative value of (must be an unsigned integer)
 * @param g The function to use as a relation. Must have the same number of bits as f (must be an
 * unsigned integer)
 * @return {min, max}, the lower and upper bound on the maximum relative value
 */
std::pair<boost::multiprecision::cpp_dec_float_100, boost::multiprecision::cpp_dec_float_100>
maximum_relative_value_bounds(const Cudd& mgr, const std::vector<BDD>& f,
                              const std::vector<BDD>& g);

/**
 * @brief Computes bounds on the maximum relative difference between f and f_hat
 * It is defined as the maximum of |f(x) - f_hat(x)| / max(1, |f(x)|) over all inputs x
 * As it is not symmetric, it is not a metric in the mathematical sense
 * This function returns a range in which the actual maximum relative error is guaranteed to lie
 * The computed bounds are always within a factor of 2, meaning that the maximum error
 * returned by this function is at most twice the minimum error
 * @param mgr The BDD object manager
 * @param f The original function
 * @param f_hat The approximated function. Must have the same number of bits as f
 * @param num_rep The number representation for f and f_hat
 * @return {min, max}, the lower and upper bound on the maximum relative error
 */
std::pair<boost::multiprecision::cpp_dec_float_100, boost::multiprecision::cpp_dec_float_100>
    wcre_bounds(
        const Cudd& mgr, const std::vector<BDD>& f, const std::vector<BDD>& f_hat,
        const abo::util::NumberRepresentation num_rep = abo::util::NumberRepresentation::BaseTwo);

/**
 * @brief Computes the maximum relative difference between f and f_hat for any input
 * It is defined as the maximum of |f(x) - f_hat(x)| / max(1, |f(x)|) over all inputs x
 * As it is not symmetric, it is not a metric in the mathematical sense
 * The computation is performed with BDDs using a binary search to find the maximum value
 * If the correct value is found, the search is aborted. Otherwise, it is run until the desired
 * precision is reached
 * @param mgr The BDD object manager
 * @param f The original function
 * @param f_hat The approximated function. Must have the same number of bits as f
 * @param num_extra_bits The number of additional bits used during the search to represent values
 * smaller than one
 * @param precision The desired precision of the result if the correct value is not found during the
 * binary search Do not set it lower than 2^-num_extra_bits
 * @param num_rep The number representation for f and f_hat
 * @return the maximum relative difference of the inputs
 */
double wcre_search(
    const Cudd& mgr, const std::vector<BDD>& f, const std::vector<BDD>& f_hat,
    unsigned int num_extra_bits = 16, double precision = 0.0001,
    const abo::util::NumberRepresentation num_rep = abo::util::NumberRepresentation::BaseTwo);

/**
 * @brief Computes the maximum relative difference between f and f_hat for any input
 * It is defined as the maximum of |f(x) - f_hat(x)| / max(1, |f(x)|) over all inputs x
 * As it is not symmetric, it is not a metric in the mathematical sense
 * The computation is performed with BDDs using a symbolic division and might be quite slow
 * @param mgr The BDD object manager
 * @param f The original function
 * @param f_hat The approximated function. Must have the same number of bits as f
 * @param num_extra_bits The number of additional fixed precision bits to use during the division
 * As the result of each division is not an integer, the result is described as a fixed point number
 * with exactly num_extra_bits bits with a lower significance than one. Roughly correlates the the
 * precision of the result
 * @param num_rep The number representation for f and f_hat
 * @return the maximum relative difference of the inputs
 */
boost::multiprecision::cpp_dec_float_100 wcre_symbolic_division(
    const Cudd& mgr, const std::vector<BDD>& f, const std::vector<BDD>& f_hat,
    unsigned int num_extra_bits = 16,
    const abo::util::NumberRepresentation num_rep = abo::util::NumberRepresentation::BaseTwo);

} // namespace abo::error_metrics
