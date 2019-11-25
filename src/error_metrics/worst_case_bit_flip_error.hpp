#ifndef WORST_CASE_BIT_FLIP_ERROR_H
#define WORST_CASE_BIT_FLIP_ERROR_H

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>

namespace abo::error_metrics {

    /**
     * @brief Computes the maximum number of bits that differ in the outputs of f and f_hat for any input
     * The computation is performed symbolically using BDDs
     * @param mgr The BDD object manager
     * @param f The original function
     * @param f_hat The approximated function. Must have the same number of bits as f
     * @return the worst case bit flip error
     */
    unsigned int worst_case_bit_flip_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    /**
     * @brief Computes the maximum number of bits that differ in the outputs of f and f_hat for any input
     * The computation is performed symbolically using ADDs
     * @param mgr The BDD object manager
     * @param f The original function
     * @param f_hat The approximated function. Must have the same number of bits as f
     * @return the worst case bit flip error
     */
    unsigned int worst_case_bit_flip_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

}

#endif // WORST_CASE_BIT_FLIP_ERROR_H
