//
// Created by keszocze on 10.10.18.
//

#ifndef ABO_ERROR_RATE_HPP
#define ABO_ERROR_RATE_HPP

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>

#include <boost/multiprecision/cpp_int.hpp>

namespace abo::error_metrics {
    boost::multiprecision::uint256_t error_rate(const Cudd & mgr,const size_t n_vars, const  std::vector<BDD>& f, const std::vector<BDD>& f_hat);


    boost::multiprecision::uint256_t error_rate(const Cudd & mgr,const size_t n_vars, const BDD& f, const BDD& f_hat);
}

#endif //ABO_ERROR_RATE_HPP
