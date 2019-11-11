#ifndef WORST_CASE_BIT_FLIP_ERROR_H
#define WORST_CASE_BIT_FLIP_ERROR_H

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>

namespace abo::error_metrics {

    unsigned int worst_case_bit_flip_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    unsigned int worst_case_bit_flip_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

}

#endif // WORST_CASE_BIT_FLIP_ERROR_H
