//
// Created by keszocze on 10.10.18.
//

#ifndef ABO_ERROR_RATE_HPP
#define ABO_ERROR_RATE_HPP

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>

#include <boost/multiprecision/cpp_int.hpp>

namespace abo::error_metrics {

    /**
     * @brief Computes the error rate, i.e. the number of inputs for which f_hat differs from f
     * The result is scaled to lie between 0 and 1
     * The error rate is computed symbolically using BDDs
     * @param mgr The BDD object manager
     * @param f The original function
     * @param f_hat The approximated function. Must have the same number of bits as f
     * @return The computed error rate in the interval [0, 1]
     */
    double error_rate(const Cudd & mgr, const  std::vector<BDD>& f, const std::vector<BDD>& f_hat);
    double error_rate(const Cudd & mgr, const BDD& f, const BDD& f_hat);

    /**
     * @brief Computes the error rate, i.e. the number of inputs for which f_hat differs from f
     * The result is scaled to lie between 0 and 1
     * The error rate is computed symbolically using ADDs
     * The computation time will be significantly longer than the BDD variant
     * @param mgr The BDD object manager
     * @param f The original function
     * @param f_hat The approximated function. Must have the same number of bits as f
     * @return The computed error rate in the interval [0, 1]
     */
    double error_rate_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    /**
     * @brief Approximates the error rate, i.e. the number of inputs for which f_hat differs from f
     * The result is scaled to lie between 0 and 1
     * The result is computed using sampling, no proper guarantee on the introduced error can be given
     * @param mgr The BDD object manager
     * @param f The original function
     * @param f_hat The approximated function. Must have the same number of bits as f
     * @param samples The number of samples to use
     * @return The approximated error rate in the interval [0, 1]
     */
    double error_rate_sampling(const Cudd &mgr, const std::vector<BDD>& f, const std::vector<BDD>& f_hat, long samples = 10000);

    /**
     * @brief Approximates the error rate, i.e. the number of inputs for which f_hat differs from f
     * The result is scaled to lie between 0 and 1
     * The result is computed using an improved sampling method that offers faster convergence than
     * the regular error rate, but still no proper guarantee on the introduced error can be given
     * @param mgr The BDD object manager
     * @param f The original function
     * @param f_hat The approximated function. Must have the same number of bits as f
     * @param samples The number of samples to use
     * @return The approximated error rate in the interval [0, 1]
     */
    double error_rate_efficient_sampling(const Cudd &mgr, const std::vector<BDD>& f, const std::vector<BDD>& f_hat, long samples = 10000);
}

#endif //ABO_ERROR_RATE_HPP
