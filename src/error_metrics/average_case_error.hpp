#pragma once

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include "number_representation.hpp"

namespace abo::error_metrics {

    using abo::util::NumberRepresentation;

    /**
     * @brief Computes the average value of the function f over all possible inputs
     * @param f The function to compute the average of
     * Is interpreted to be unsigned
     * @return the average value of f as a high precision float
     */
    boost::multiprecision::cpp_dec_float_100 average_value(const std::vector<BDD> &f);

    /**
     * @brief Computes the average squared value of the function f over all possible inputs
     * @param f The function to compute the average squared value of
     * Is interpreted to be unsigned
     * @return the mean squared value of f as a high precision float
     */
    boost::multiprecision::cpp_dec_float_100 mean_squared_value(const std::vector<BDD> &f);

    /**
     * @brief Computes the average absolute difference between the functions f and f_hat
     * The computation is performed symbolically with BDD forests and may take exponential time
     * @param mgr Cudd manager object
     * @param f The original function
     * @param f_hat The approximated function
     * @param num_rep The number representation for f and f_hat
     * @return The average absolute difference between f and f_hat
     */
    boost::multiprecision::cpp_dec_float_100 average_case_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat,
                                                                const NumberRepresentation num_rep = NumberRepresentation::BaseTwo);

    /**
     * @brief Computes the average squared absolute difference between the functions f and f_hat
     * The computation is performed symbolically with BDD forests and may take exponential time
     * @param mgr Cudd manager object
     * @param f The original function
     * @param f_hat The approximated function
     * @param num_rep The number representation for f and f_hat
     * @return The average squared absolute difference between f and f_hat
     */
    boost::multiprecision::cpp_dec_float_100 mean_squared_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat,
                                                                const NumberRepresentation num_rep = NumberRepresentation::BaseTwo);

    /**
     * @brief Computes the average absolute difference between the functions f and f_hat
     * The computation is performed symbolically with ADDs and may take exponential time
     * This function is generally slower than the equivalent average_case_error() function
     * and should only be used as a reference
     * @param mgr Cudd manager object
     * @param f The original function
     * @param f_hat The approximated function
     * @param num_rep The number representation for f and f_hat
     * @return The average absolute difference between f and f_hat
     */
    boost::multiprecision::cpp_dec_float_100 average_case_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat,
                                                                    const NumberRepresentation num_rep = NumberRepresentation::BaseTwo);

    /**
     * @brief Computes the average squared absolute difference between the functions f and f_hat
     * The computation is performed symbolically with ADDs and may take exponential time
     * This function is generally slower than the equivalent mean_squared_error() function
     * and should only be used as a reference
     * @param mgr Cudd manager object
     * @param f The original function
     * @param f_hat The approximated function
     * @param num_rep The number representation for f and f_hat
     * @return The average squared absolute difference between f and f_hat
     */
    boost::multiprecision::cpp_dec_float_100 mean_squared_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat,
                                                                    const NumberRepresentation num_rep = NumberRepresentation::BaseTwo);

}
