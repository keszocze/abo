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

        std::vector<BDD> difference = abo::util::bdd_subtract(mgr, f_, f_hat_);
        std::vector<BDD> absolute_difference = abo::util::abs(mgr,difference);

        return average_value(absolute_difference);
    }

    boost::multiprecision::cpp_dec_float_100 mean_squared_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        std::vector<BDD> f_= f;
        std::vector<BDD> f_hat_ = f_hat;

        // add sign bits so that
        f_.push_back(mgr.bddZero());
        f_hat_.push_back(mgr.bddZero());

        std::vector<BDD> difference = abo::util::bdd_subtract(mgr, f_, f_hat_);
        std::vector<BDD> absolute_difference = abo::util::abs(mgr,difference);

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

    std::pair<boost::multiprecision::cpp_dec_float_100, boost::multiprecision::cpp_dec_float_100>
            average_relative_value(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g) {

        BDD zero_so_far = mgr.bddOne();
        boost::multiprecision::cpp_dec_float_100 min_average_result = 0, max_average_result = 0;
        for (int i = g.size()-1;i>=0;i--) {
            std::vector<BDD> partial_result;
            partial_result.reserve(f.size());
            BDD modifier = zero_so_far & g[i];
            for (const BDD &b : f) {
                partial_result.push_back(b & modifier);
            }
            auto average = average_value(partial_result);
            min_average_result += average  / (std::pow(2.0, i + 1) - 1);
            max_average_result += average  / std::pow(2.0, i);

            zero_so_far &= !g[i];
        }

        // handle f(x) = 0
        std::vector<BDD> partial_result;
        for (const BDD &b : f) {
            partial_result.push_back(b & zero_so_far);
        }
        auto average = average_value(partial_result);
        min_average_result += average;
        max_average_result += average;

        return {min_average_result, max_average_result};
    }

    std::pair<boost::multiprecision::cpp_dec_float_100, boost::multiprecision::cpp_dec_float_100>
            avererage_relative_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        std::vector<BDD> f_= f;
        std::vector<BDD> f_hat_ = f_hat;

        f_.push_back(mgr.bddZero());
        f_hat_.push_back(mgr.bddZero());

        std::vector<BDD> difference = abo::util::bdd_subtract(mgr, f_, f_hat_);
        std::vector<BDD> absolute_difference = abo::util::abs(mgr,difference);

        return average_relative_value(mgr, absolute_difference, f_);
    }

    boost::multiprecision::cpp_dec_float_100 average_relative_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        ADD diff = abo::util::absolute_difference_add(mgr, f, f_hat);
        ADD respective_diff = diff.Divide(abo::util::bdd_forest_to_add(mgr, f).Maximum(mgr.addOne()));
        std::vector<std::pair<double, unsigned long>> terminal_values = abo::util::add_terminal_values(respective_diff);

        boost::multiprecision::cpp_dec_float_100 sum = 0;
        boost::multiprecision::uint256_t path_sum = 0;
        for (auto p : terminal_values) {
            boost::multiprecision::cpp_dec_float_100 value(p.first);
            sum += value * p.second;
            path_sum += p.second;
        }
        return boost::multiprecision::cpp_dec_float_100(sum) /
                boost::multiprecision::cpp_dec_float_100(path_sum);
    }

    boost::multiprecision::cpp_dec_float_100 average_relative_error_sampling(const std::vector<BDD>& f, const std::vector<BDD>& f_hat, long samples) {
        assert(f.size() == f_hat.size());
        std::vector<BDD> difference;

        difference.reserve(f.size());
        for (unsigned int i = 0;i<f.size();i++) {
            difference.push_back(f[i] ^ f_hat[i]);
        }

        unsigned int terminal_level = abo::util::terminal_level({f, f_hat});

        boost::multiprecision::cpp_dec_float_100 error = 0;
        std::vector<int> inputs(terminal_level, 0);
        for (int i = 0;i<samples;i++) {
            std::generate(inputs.begin(), inputs.end(), []() { return rand() % 2; });
            boost::multiprecision::cpp_dec_float_100 original_value = 0, diff_value = 0;
            for (auto it = difference.rbegin();it != difference.rend();++it) {
                diff_value *= 2;
                if (it->Eval(inputs.data()).IsOne()) {
                    diff_value += 1;
                }
            }
            for (auto it = f.rbegin();it != f.rend();++it) {
                original_value *= 2;
                if (it->Eval(inputs.data()).IsOne()) {
                    original_value += 1;
                }
            }
            error += diff_value / max(boost::multiprecision::cpp_dec_float_100(1.0), original_value);
        }
        return error / samples;
    }

}
