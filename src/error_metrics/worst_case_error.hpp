//
// Created by keszocze on 15.10.18.
//

#pragma once

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cudd/cplusplus/cuddObj.hh>
#include <vector>

#include "number_representation.hpp"

namespace abo::error_metrics {

/**
 * @brief Computes the maximal value of a function represented by a vector of BDDs
 *
 * The function is assumed to return an natural number
 *
 * @param mgr
 * @param fun The function given by a vector of BDDs
 */
boost::multiprecision::uint256_t get_max_value(const Cudd& mgr,
                                               const std::vector<BDD>& fun);

/**
 * @brief Computes the maximum absolute difference between the f and f_hat for any input
 * The computation is performed symbolically using BDDs
 * @param mgr The BDD object manager
 * @param f The original function
 * @param f_hat The approximated function. Must have the same number of bits as f
 * @param num_rep The number representation for f and f_hat
 * @return The maximum absolute difference
 */
boost::multiprecision::uint256_t
worst_case_error(const Cudd& mgr,
                 const std::vector<BDD>& f,
                 const std::vector<BDD>& f_hat,
                 const abo::util::NumberRepresentation num_rep
                    = abo::util::NumberRepresentation::BaseTwo);

/**
 * @brief Computes the maximum absolute difference between the f and f_hat for any input
 * divided by 2^n - 1 to normalize it to the range [0, 1] regardless of the function size (with n =
 * f.size()) This is mainly intended to be a helper function to make the use of the worst case error
 * easier
 * @param mgr The BDD object manager
 * @param f The original function
 * @param f_hat The approximated function. Must have the same number of bits as f
 * @param num_rep The number representation for f and f_hat
 * @return The maximum absolute difference normalized to [0, 1]
 */
double worst_case_error_percent(const Cudd& mgr, const std::vector<BDD>& f,
                                const std::vector<BDD>& f_hat,
                                const abo::util::NumberRepresentation num_rep
                                    = abo::util::NumberRepresentation::BaseTwo);

/**
 * @brief Computes the maximum absolute difference between the f and f_hat for any input
 * The computation is performed using BDDs and will be typically slow compared to the BDD based
 * variant
 * @param mgr The BDD object manager
 * @param f The original function
 * @param f_hat The approximated function. Must have the same number of bits as f
 * @param num_rep The number representation for f and f_hat
 * @return The maximum absolute difference
 */
boost::multiprecision::uint256_t
worst_case_error_add(const Cudd& mgr,
                     const std::vector<BDD>& f,
                     const std::vector<BDD>& f_hat,
                     const abo::util::NumberRepresentation num_rep
                        = abo::util::NumberRepresentation::BaseTwo);

/**
 * @brief approximate_worst_case_error
 *  Calculates the worst case error approximately, to a given relative error.
 *  The time and memory this function uses scales exponentially with the desired precision (n) (in
 * the worst case). With m being the input size (f, f_hat), in the worst case, it lies in O(m ^ n).
 * @param mgr
 * @param f
 * @param f_hat
 * @param n The precision to calculate in number of bits, for more details see the return value. It
 * must be greater than zero
 * @param num_rep The number representation for f and f_hat
 * @return The approximated worst case error. It is an upper bound and therefore guaranteed to be
 * larger than the actual error. Let wc be the correct worst case error and x be the result of this
 * function. Then it holds that wc <= x <= wc * (1 + 1 / (2 ^ (n + 1) - 1)))
 */
boost::multiprecision::uint256_t
approximate_worst_case_error(const Cudd& mgr, const std::vector<BDD>& f,
                             const std::vector<BDD>& f_hat, int n,
                             const abo::util::NumberRepresentation num_rep
                                = abo::util::NumberRepresentation::BaseTwo);

} // namespace abo::error_metrics
