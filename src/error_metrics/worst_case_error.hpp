//
// Created by keszocze on 15.10.18.
//

#ifndef ABO_WORST_CASE_ERROR_HPP
#define ABO_WORST_CASE_ERROR_HPP

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>
#include <boost/multiprecision/cpp_int.hpp>

namespace abo::error_metrics {
    enum class NumberRepresentation: bool { BaseTwo, TwosComplement};

    /**
     * @brief Computes the maximal value of a function represented by a vector of BDDs
     *
     * The function is assumed to return an natural number
     *
     * @param mgr
     * @param fun The function given by a vector of BDDs
     */
    boost::multiprecision::uint256_t get_max_value(const Cudd &mgr, const std::vector<BDD> &fun);

    boost::multiprecision::uint256_t
    worst_case_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat,
                         const NumberRepresentation num_rep= NumberRepresentation::BaseTwo);

    boost::multiprecision::uint256_t
        worst_case_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    /**
     * @brief approximate_worst_case_error
     *  Calculates the worst case error approximately, to a given relative error.
     *  The time and memory this function uses scales exponentially with the desired precision (n) (in the worst case).
     *  With m being the input size (f, f_hat), in the worst case, it lies in O(m ^ n).
     * @param mgr
     * @param f
     * @param f_hat
     * @param n The precision to calculate in number of bits, for more details see the return value. It must be greater than zero.
     * @return The approximated worst case error. It is an upper bound and therefore guaranteed to be larger
     *  than the actual error. Let wc be the correct worst case error and x be the result of this function.
     *  Then it holds that wc <= x <= wc * (1 + 1 / (2 ^ (n + 1) - 1)))
     */
    boost::multiprecision::uint256_t approximate_worst_case_error(const Cudd &mgr, const std::vector<BDD> &f,
                                                                  const std::vector<BDD> &f_hat, int n);

}

#endif //ABO_WORST_CASE_ERROR_HPP

