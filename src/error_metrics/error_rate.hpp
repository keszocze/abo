//
// Created by keszocze on 10.10.18.
//

#ifndef ABO_ERROR_RATE_HPP
#define ABO_ERROR_RATE_HPP

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>

#include <boost/multiprecision/cpp_int.hpp>

namespace abo::error_metrics {
    double error_rate(const Cudd & mgr, const  std::vector<BDD>& f, const std::vector<BDD>& f_hat);

    double error_rate_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    double error_rate(const Cudd & mgr, const BDD& f, const BDD& f_hat);

}

#endif //ABO_ERROR_RATE_HPP
