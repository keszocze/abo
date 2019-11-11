#ifndef AVERAGE_BIT_FLIP_ERROR_HPP
#define AVERAGE_BIT_FLIP_ERROR_HPP

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>

namespace abo::error_metrics {

    double average_bit_flip_error(const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    double average_bit_flip_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

}

#endif // AVERAGE_BIT_FLIP_ERROR_HPP
