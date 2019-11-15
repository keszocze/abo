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

    boost::multiprecision::cpp_dec_float_100 mean_squared_value(const std::vector<BDD> &f) {

        boost::multiprecision::uint256_t sum = 0;
        const boost::multiprecision::uint256_t one = 1;

        int max_support_size = 0;
        for (const auto &bdd : f) {
            max_support_size = std::max(max_support_size, bdd.SupportSize());
        }

        for (unsigned int i = 0;i<f.size();i++) {
            for (unsigned int b = i;b<f.size();b++) {
                // cudd handles the case i = b efficiently
                BDD both = f[i] & f[b];
                double minterms = both.CountMinterm(max_support_size);
                // most terms (except for i == b), appear twice when factoring the square
                boost::multiprecision::uint256_t doubling_factor = i == b ? 1 : 2;
                sum += (one << i) * (one << b) * boost::multiprecision::uint256_t(minterms) * doubling_factor;
            }
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

        std::vector<BDD> absolute_difference = abo::util::bdd_absolute_difference(mgr, f_, f_hat_);

        return average_value(absolute_difference);
    }

    boost::multiprecision::cpp_dec_float_100 mean_squared_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        std::vector<BDD> f_= f;
        std::vector<BDD> f_hat_ = f_hat;

        // add sign bits so that
        f_.push_back(mgr.bddZero());
        f_hat_.push_back(mgr.bddZero());

        std::vector<BDD> absolute_difference = abo::util::bdd_absolute_difference(mgr, f_, f_hat_);

        return mean_squared_value(absolute_difference);
    }


    boost::multiprecision::cpp_dec_float_100 average_case_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        ADD diff = abo::util::absolute_difference_add(mgr, f, f_hat);
        std::vector<std::pair<double, unsigned long>> terminal_values = abo::util::add_terminal_values(diff);

        boost::multiprecision::uint256_t sum = 0;
        boost::multiprecision::uint256_t path_sum = 0;
        for (auto p : terminal_values) {
            sum += boost::multiprecision::uint256_t(p.first) * p.second;
            path_sum += p.second;
        }
        return boost::multiprecision::cpp_dec_float_100(sum) /
                boost::multiprecision::cpp_dec_float_100(path_sum);
    }

    boost::multiprecision::cpp_dec_float_100 mean_squared_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        ADD diff = abo::util::absolute_difference_add(mgr, f, f_hat);
        std::vector<std::pair<double, unsigned long>> terminal_values = abo::util::add_terminal_values(diff);

        boost::multiprecision::uint256_t sum = 0;
        boost::multiprecision::uint256_t path_sum = 0;
        for (auto p : terminal_values) {
            boost::multiprecision::uint256_t value(p.first);
            sum += value * value * p.second;
            path_sum += p.second;
        }
        return boost::multiprecision::cpp_dec_float_100(sum) /
                boost::multiprecision::cpp_dec_float_100(path_sum);
    }

}
