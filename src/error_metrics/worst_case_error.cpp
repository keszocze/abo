//
// Created by keszocze on 15.10.18.
//

#include "worst_case_error.hpp"

#include <cudd_helpers.hpp>

namespace abo::error_metrics {


    boost::multiprecision::uint256_t get_max_value(const Cudd &mgr, const std::vector<BDD> &fun) {

        /*
         * fun[i] is the i'th bit of the represented natural numnber. f[0] is the least
         * significant bit
         */


        BDD sigma = mgr.bddOne();

        boost::multiprecision::uint256_t error = 0U;
        boost::multiprecision::uint256_t one = 1U;

        /*
         * We use iterators instead of explicit indexing using ints as that would (well, let's be honest: it is not
         * going to happen) theoretically reduce the size of supported functions.
         */

        size_t exponent = fun.size()-1;

        for (auto iter = fun.rbegin(); iter != fun.rend(); ++iter, --exponent) {
            BDD mask = sigma & *iter;

            if (!mask.IsZero()) {
                error += (one << exponent);
                sigma = mask;
            }
        }

        return error;
    }

    boost::multiprecision::uint256_t
    worst_case_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat,
                         const NumberRepresentation num_rep) {

        std::vector<BDD> f_= f;
        std::vector<BDD> f_hat_ = f_hat;

        if (num_rep == NumberRepresentation::BaseTwo) {
            // we need to add sign bits to the functions as they don't have one right now
            // these are necessary to be able to add the negative number instead of actually subtracting
            f_.push_back(mgr.bddZero());
            f_hat_.push_back(mgr.bddZero());
        }

        std::vector<BDD> difference = abo::util::bdd_subtract(mgr, f_, f_hat_);


        std::vector<BDD> absolute_difference = abo::util::abs(mgr,difference);



        return get_max_value(mgr,absolute_difference);
    }

    boost::multiprecision::uint256_t
    worst_case_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        ADD diff = abo::util::absolute_difference_add(mgr, f, f_hat);
        std::vector<std::pair<double, unsigned long>> terminal_values = abo::util::add_terminal_values(diff);

        boost::multiprecision::uint256_t max_value = 0;
        for (auto p : terminal_values) {
            max_value = std::max(boost::multiprecision::uint256_t(p.first), max_value);
        }
        return max_value;
    }

    boost::multiprecision::uint256_t approximate_worst_case_error(const Cudd &mgr, const std::vector<BDD> &f,
                                                                  const std::vector<BDD> &f_hat, int n) {
        assert(f.size() == f_hat.size() && n >= 1);

        std::vector<BDD> f_;
        std::vector<BDD> f_hat_;

        f_.push_back(mgr.bddZero());
        f_hat_.push_back(mgr.bddZero());

        int bit_start = -1;
        int bit_end = -1;
        for (int i = f.size()-1;i >= 0;i--) {
            if (bit_start == -1 && f[i] != f_hat[i]) {
                bit_start = i;
            }
            if (bit_start >= 0 && bit_start - i >= n) {
                bit_end = i;
                break;
            }
            f_.push_back(f[i]);
            f_hat_.push_back(f_hat[i]);
        }
        if (bit_start == -1) {
            return 0;
        }
        std::reverse(f_.begin(), f_.end());
        std::reverse(f_hat_.begin(), f_hat_.end());

        std::vector<BDD> difference = abo::util::bdd_subtract(mgr, f_, f_hat_);
        std::vector<BDD> absolute_difference = abo::util::abs(mgr, difference);

        unsigned long shift = bit_end + 1;
        boost::multiprecision::uint256_t one = 1;
        // add one to give an upper bound on the error
        return (get_max_value(mgr, absolute_difference) + 1) * (one << shift) - 1;
    }

    double worst_case_relative_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        ADD diff = abo::util::absolute_difference_add(mgr, f, f_hat);
        ADD respective_diff = diff.Divide(abo::util::bdd_forest_to_add(mgr, f).Maximum(mgr.addOne()));
        std::vector<std::pair<double, unsigned long>> terminal_values = abo::util::add_terminal_values(respective_diff);

        double largest = 0;
        for (auto p : terminal_values) {
            largest = std::max(largest, p.first);
        }
        return largest;
    }

    double worst_case_relative_error_search(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat, unsigned int num_extra_bits, double precision) {
        std::vector<BDD> f_ = f;
        std::vector<BDD> f_hat_ = f_hat;

        f_.push_back(mgr.bddZero());
        f_hat_.push_back(mgr.bddZero());

        f_ = abo::util::bdd_max_one(mgr, f_);

        std::vector<BDD> difference = abo::util::bdd_subtract(mgr, f_, f_hat_);
        std::vector<BDD> absolute_difference = abo::util::abs(mgr, difference);

        std::vector<BDD> result(num_extra_bits, mgr.bddZero());
        absolute_difference.insert(absolute_difference.begin(), result.begin(), result.begin() + num_extra_bits);
        f_.insert(f_.begin(), result.begin(), result.begin() + num_extra_bits);

        double min, max;
        double factor = 1;
        for (int i = 0;;i++) {
            std::vector<BDD> multiplied = abo::util::bdd_multiply_constant(mgr, f_, factor);
            if (!abo::util::exists_greater_equals(mgr, absolute_difference, multiplied)) {
                max = factor;
                min = factor == 1.0 ? 0 : factor / 2.0;
                break;
            }
            factor *= 2;
        }
        for (int i = 0;max - min > precision;i++) {
            double middle = (min + max) / 2.0;
            std::vector<BDD> multiplied = abo::util::bdd_multiply_constant(mgr, f_, middle);
            if (abo::util::exists_greater_equals(mgr, absolute_difference, multiplied)) {
                min = middle;
            } else {
                max = middle;
            }
        }
        return (min + max) / 2.0;
    }

    boost::multiprecision::cpp_dec_float_100 worst_case_relative_error_symbolic_division(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat, unsigned int num_extra_bits) {

        std::vector<BDD> f_= f;
        std::vector<BDD> f_hat_ = f_hat;

        // add sign bits so that
        f_.push_back(mgr.bddZero());
        f_hat_.push_back(mgr.bddZero());

        std::vector<BDD> difference = abo::util::bdd_subtract(mgr, f_, f_hat_);
        std::vector<BDD> absolute_difference = abo::util::abs(mgr,difference);

        std::vector<BDD> no_zero = abo::util::bdd_max_one(mgr, f_);
        std::vector<BDD> divided = abo::util::bdd_divide(mgr, absolute_difference, no_zero, num_extra_bits);
        auto max = get_max_value(mgr, divided);
        boost::multiprecision::cpp_dec_float_100 value(max);
        value /= std::pow(2, num_extra_bits);
        return value;
    }

    std::pair<boost::multiprecision::cpp_dec_float_100, boost::multiprecision::cpp_dec_float_100>
            maximum_relative_value(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g) {

        BDD zero_so_far = mgr.bddOne();
        boost::multiprecision::cpp_dec_float_100 min_error = std::numeric_limits<double>::infinity();
        boost::multiprecision::cpp_dec_float_100 max_error = 0;
        for (int i = g.size()-1;i>=0;i--) {
            std::vector<BDD> partial_result;
            partial_result.reserve(f.size());
            BDD modifier = zero_so_far & g[i];
            for (const BDD &b : f) {
                partial_result.push_back(b & modifier);
            }
            std::vector<int> inputValue;
            boost::multiprecision::cpp_dec_float_100 max_val = boost::multiprecision::cpp_dec_float_100(get_max_value(mgr, partial_result));

            min_error = min(min_error, max_val / (std::pow(2.0, i + 1) - 1));
            max_error = max(max_error, max_val  / std::pow(2.0, i));

            zero_so_far &= !g[i];
        }

        // handle f(x) = 0
        std::vector<BDD> partial_result;
        for (const BDD &b : f) {
            partial_result.push_back(b & zero_so_far);
        }
        std::vector<int> inputValue;
        boost::multiprecision::cpp_dec_float_100 max_val = boost::multiprecision::cpp_dec_float_100(get_max_value(mgr, partial_result));
        min_error = min(min_error, max_val); // divided by one
        max_error = max(max_error, max_val);

        return {min_error, max_error};
    }

    std::pair<boost::multiprecision::cpp_dec_float_100, boost::multiprecision::cpp_dec_float_100>
            worst_case_relative_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        std::vector<BDD> f_= f;
        std::vector<BDD> f_hat_ = f_hat;

        f_.push_back(mgr.bddZero());
        f_hat_.push_back(mgr.bddZero());

        std::vector<BDD> difference = abo::util::bdd_subtract(mgr, f_, f_hat_);
        std::vector<BDD> absolute_difference = abo::util::abs(mgr,difference);

        return maximum_relative_value(mgr, absolute_difference, f_);
    }
}
