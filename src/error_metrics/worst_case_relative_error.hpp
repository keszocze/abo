#ifndef WORST_CASE_RELATIVE_ERROR_H
#define WORST_CASE_RELATIVE_ERROR_H

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>
#include <boost/multiprecision/cpp_dec_float.hpp>

namespace abo::error_metrics {

    double worst_case_relative_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    std::pair<boost::multiprecision::cpp_dec_float_100, boost::multiprecision::cpp_dec_float_100>
            maximum_relative_value_bounds(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g);

    std::pair<boost::multiprecision::cpp_dec_float_100, boost::multiprecision::cpp_dec_float_100>
            worst_case_relative_error_bounds(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    double worst_case_relative_error_search(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat,
                                            unsigned int num_extra_bits = 16, double precision = 0.0001);

    boost::multiprecision::cpp_dec_float_100 worst_case_relative_error_symbolic_division(const Cudd &mgr, const std::vector<BDD> &f,
                                                                                         const std::vector<BDD> &f_hat, unsigned int num_extra_bits = 16);

}

#endif // WORST_CASE_RELATIVE_ERROR_H
