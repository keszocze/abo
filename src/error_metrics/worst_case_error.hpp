//
// Created by keszocze on 15.10.18.
//

#ifndef ABO_WORST_CASE_ERROR_HPP
#define ABO_WORST_CASE_ERROR_HPP

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>
#include <boost/multiprecision/cpp_int.hpp>

namespace abo::error_metrics {

    /**
     * @brief Computes the maximal value of a function represented by a vector of BDDs
     *
     * The function is assumed to return an natural number
     *
     * @param mgr
     * @param fun The function given by a vector of BDDs
     */
    boost::multiprecision::uint256_t get_max_value(const Cudd &mgr, const std::vector<BDD> &fun);
}

#endif //ABO_WORST_CASE_ERROR_HPP

