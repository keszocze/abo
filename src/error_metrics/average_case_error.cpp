#include "average_case_error.hpp"

#include <cudd_helpers.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace abo::error_metrics {

    boost::multiprecision::cpp_dec_float_100 average_value(const std::vector<BDD> &f) {

        boost::multiprecision::uint256_t sum = 0;
        const boost::multiprecision::uint256_t one = 1;

        int max_support_size = 0;
        for (const auto &bdd : f) {
            max_support_size = std::max(max_support_size, bdd.SupportSize());
        }

        for (unsigned int i = 0;i<f.size();i++) {
            // TODO: CountMinterms returns a double, might loose precision
            double minterms = f[i].CountMinterm(max_support_size);
            sum += (one << i) * boost::multiprecision::uint256_t(minterms);
        }

        return boost::multiprecision::cpp_dec_float_100(sum) /
                boost::multiprecision::cpp_dec_float_100(one << max_support_size);
    }

    boost::multiprecision::cpp_dec_float_100 average_case_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        std::vector<BDD> f_= f;
        std::vector<BDD> f_hat_ = f_hat;

        // add sign bits so that
        f_.push_back(mgr.bddZero());
        f_hat_.push_back(mgr.bddZero());

        std::vector<BDD> difference = abo::util::bdd_subtract(mgr, f_, f_hat_);
        std::vector<BDD> absolute_difference = abo::util::abs(mgr,difference);

        return average_value(absolute_difference);
    }

}
