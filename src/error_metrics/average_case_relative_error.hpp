#ifndef AVERAGE_CASE_RELATIVE_ERROR_HPP
#define AVERAGE_CASE_RELATIVE_ERROR_HPP

#include <vector>
#include <cudd/cplusplus/cuddObj.hh>
#include <boost/multiprecision/cpp_dec_float.hpp>

namespace abo::error_metrics {
    // return bounds on the average respective error: {minimum error, maximum error}
    std::pair<boost::multiprecision::cpp_dec_float_100, boost::multiprecision::cpp_dec_float_100>
        avererage_relative_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    boost::multiprecision::cpp_dec_float_100 average_relative_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat);

    boost::multiprecision::cpp_dec_float_100 average_relative_error_sampling(const std::vector<BDD>& f, const std::vector<BDD>& f_hat, long samples);
}

#endif // AVERAGE_CASE_RELATIVE_ERROR_HPP
