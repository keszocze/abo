#include "average_case_relative_error.hpp"
#include "average_case_error.hpp"
#include "cudd_helpers.hpp"
#include <boost/multiprecision/cpp_int.hpp>

namespace abo::error_metrics {
    std::pair<boost::multiprecision::cpp_dec_float_100, boost::multiprecision::cpp_dec_float_100>
            average_relative_value(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g) {

        BDD zero_so_far = mgr.bddOne();

        boost::multiprecision::cpp_dec_float_100 min_average_result = 0;
        boost::multiprecision::cpp_dec_float_100 max_average_result = 0;
        std::vector<BDD> max_one = abo::util::bdd_max_one(mgr, f);
        for (int i = int(g.size())-1;i>=0;i--) {
            std::vector<BDD> partial_result;
            partial_result.reserve(max_one.size());
            BDD modifier = zero_so_far & g[i];
            for (const BDD &b : max_one) {
                partial_result.push_back(b & modifier);
            }
            auto average = average_value(partial_result);
            min_average_result += average  / (std::pow(2.0, i + 1) - 1);
            max_average_result += average  / std::pow(2.0, i);

            zero_so_far &= !g[i];
        }

        return {min_average_result, max_average_result};
    }

    std::pair<boost::multiprecision::cpp_dec_float_100, boost::multiprecision::cpp_dec_float_100>
            average_relative_error_bounds(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        std::vector<BDD> f_= f;
        std::vector<BDD> f_hat_ = f_hat;

        f_.push_back(mgr.bddZero());
        f_hat_.push_back(mgr.bddZero());

        std::vector<BDD> absolute_difference = abo::util::bdd_absolute_difference(mgr, f_, f_hat_);

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
            boost::multiprecision::cpp_dec_float_100 original_value = 0;
            boost::multiprecision::cpp_dec_float_100 diff_value = 0;
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

    boost::multiprecision::cpp_dec_float_100 average_relative_error_symbolic_division(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat, unsigned int num_extra_bits) {
        std::vector<BDD> f_= f;
        std::vector<BDD> f_hat_ = f_hat;

        f_.push_back(mgr.bddZero());
        f_hat_.push_back(mgr.bddZero());

        std::vector<BDD> absolute_difference = abo::util::bdd_absolute_difference(mgr, f_, f_hat_);

        std::vector<BDD> no_zero = abo::util::bdd_max_one(mgr, f_);
        std::vector<BDD> divided = abo::util::bdd_divide(mgr, absolute_difference, no_zero, num_extra_bits);

        return average_value(divided) / std::pow(2.0, num_extra_bits);
    }

}
