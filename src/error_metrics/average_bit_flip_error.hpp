#pragma once

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>

namespace abo::error_metrics {

    /**
     * @brief Compute the average bit flip error between the functions f and f_hat
     * It is computed symbolically using BDD forests.
     * The time complexity of this function is linear in the number of BDD nodes in the inputs.
     * @param f The original function
     * @param f_hat The approximated function. must have the same number of bits as f
     * @return the average bit flip error ranging between 0 and f.size()
     */
    double average_bit_flip_error(const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    /**
     * @brief Compute the average bit flip error between the functions f and f_hat
     * It is computed symbolically using ADDs.
     * This function may take exponential time in the number of BDD nodes in the input.
     * @param mgr The Cudd object manager
     * @param f The original function
     * @param f_hat The approximated function. must have the same number of bits as f
     * @return the average bit flip error ranging between 0 and f.size()
     */
    double average_bit_flip_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

}
