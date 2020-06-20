#include "worst_case_relative_error.hpp"
#include "cudd_helpers.hpp"
#include "worst_case_error.hpp"

using abo::util::NumberRepresentation;
using boost::multiprecision::cpp_dec_float_100;

namespace abo::error_metrics {

double wcre_add(const Cudd& mgr, const std::vector<BDD>& f,
                 const std::vector<BDD>& f_hat,
                 const util::NumberRepresentation num_rep)
{

    ADD diff = abo::util::absolute_difference_add(mgr, f, f_hat, num_rep);
    std::vector<BDD> f_abs = abo::util::bdd_abs(mgr, f, num_rep);
    ADD respective_diff = diff.Divide(
        abo::util::bdd_forest_to_add(mgr, f_abs).Maximum(mgr.addOne()));
    std::vector<std::pair<double, unsigned long>> terminal_values =
        abo::util::add_terminal_values(respective_diff);

    double largest = 0;
    for (auto p : terminal_values)
    {
        largest = std::max(largest, p.first);
    }
    return largest;
}

static std::pair<bool, bool> has_greater_equal_both(const Cudd& mgr, const std::vector<BDD> & f,
                                                    const std::vector<BDD>& g,
                                                    const boost::multiprecision::uint256_t& counter,
                                                    const boost::multiprecision::uint256_t& denominator)
{
    std::vector<BDD> bdd_denom = abo::util::bdd_multiply_constant(mgr, f, denominator);
    std::vector<BDD> bdd_counter = abo::util::bdd_multiply_constant(mgr, g, counter);
    return abo::util::exists_greater_equals(mgr, bdd_denom, bdd_counter);
}

std::pair<long, long> wcre_randomized_search(const Cudd& mgr, const std::vector<BDD>& f,
                                             const std::vector<BDD>& f_hat, unsigned int samples,
                                             const NumberRepresentation num_rep)
{
    std::vector<BDD> f_abs = abo::util::bdd_abs(mgr, f, num_rep);
    std::vector<BDD> f_ = abo::util::bdd_max_one(mgr, f_abs);

    unsigned int terminal_level = mgr.ReadSize();

    std::vector<BDD> absolute_difference =
            abo::util::bdd_absolute_difference(mgr, f, f_hat, num_rep);

    std::vector<unsigned int> support_indices1 = mgr.SupportIndices(f);
    std::vector<unsigned int> support_indices2 = mgr.SupportIndices(f_hat);
    std::set<unsigned int> unique_support(support_indices1.begin(), support_indices1.end());
    unique_support.insert(support_indices2.begin(), support_indices2.end());
    std::vector<BDD> support;
    support.reserve(unique_support.size());
    for (unsigned int s : unique_support) {
        support.push_back(mgr.bddVar(s));
    }

    boost::multiprecision::uint256_t counter = 1;
    boost::multiprecision::uint256_t denominator = 1;

    // search for the last power of two reachable as a fraction
    auto ge_one = has_greater_equal_both(mgr, absolute_difference, f_, 1, 1);
    if (ge_one.second) {
        return {1, 1};
    }
    if (ge_one.first) {
        while (true) {
            counter *= 2;
            auto ge = has_greater_equal_both(mgr, absolute_difference, f_, counter, denominator);
            if (ge.second) {
                return {long(counter), long(denominator)};
            }
            if (!ge.first) {
                break;
            }
        }
        counter /= 2;
    } else {
        while (true) {
            denominator *= 2;
            auto ge = has_greater_equal_both(mgr, absolute_difference, f_, counter, denominator);
            if (ge.second) {
                return {long(counter), long(denominator)};
            }
            if (ge.first) {
                break;
            }
        }
    }

    BDD last_greater = mgr.bddOne();
    {
        std::vector<BDD> bdd_denom = abo::util::bdd_multiply_constant(mgr, absolute_difference, denominator);
        std::vector<BDD> bdd_counter = abo::util::bdd_multiply_constant(mgr, f_, counter);
        last_greater = abo::util::greater_than(mgr, bdd_denom, bdd_counter);
    }
    while (true) {
        // reduce fraction to multiply as little as possible
        boost::multiprecision::uint256_t common = std::__gcd(counter, denominator);
        counter /= common;
        denominator /= common;

        // only compute the multiplication for the values which could result in higher errors
        auto absGr = absolute_difference;
        for (BDD &b : absGr) {
            b = b & last_greater;
        }

        auto fGr = f_;
        for (BDD &b : fGr) {
            b = b & last_greater;
        }

        std::vector<BDD> bdd_denom = abo::util::bdd_multiply_constant(mgr, absGr, denominator);
        std::vector<BDD> bdd_counter = abo::util::bdd_multiply_constant(mgr, fGr, counter);

        BDD greater = abo::util::greater_than(mgr, bdd_denom, bdd_counter);
        if (greater.IsZero()) {
            break;
        }

        // search for the next wcr value
        std::map<DdNode*, double> minterm_count = abo::util::count_minterms(greater);
        for (std::size_t i = 0;i<samples;i++) {
            std::vector<int> next_input = abo::util::random_satisfying_input(greater, minterm_count, terminal_level);
            long test_counter = abo::util::eval(absolute_difference, next_input);
            long test_denominator = abo::util::eval(f_, next_input);
            if (test_counter * denominator > counter * test_denominator) {
                counter = test_counter;
                denominator = test_denominator;
            }
        }
    }
    return {long(counter), long(denominator)};
}

double wcre_search(const Cudd& mgr, const std::vector<BDD>& f,
                    const std::vector<BDD>& f_hat, unsigned int num_extra_bits,
                    double precision, const NumberRepresentation num_rep)
{

    std::vector<BDD> f_abs = abo::util::bdd_abs(mgr, f, num_rep);
    std::vector<BDD> f_ = abo::util::bdd_max_one(mgr, f_abs);

    std::vector<BDD> absolute_difference =
        abo::util::bdd_absolute_difference(mgr, f, f_hat, num_rep);

    std::vector<BDD> result(num_extra_bits, mgr.bddZero());
    absolute_difference.insert(absolute_difference.begin(), result.begin(),
                               result.begin() + num_extra_bits);
    f_.insert(f_.begin(), result.begin(), result.begin() + num_extra_bits);

    double min;
    double max;
    for (float factor = 1.0f;;factor *= 2.0f)
    {
        std::vector<BDD> multiplied = abo::util::bdd_multiply_constant(mgr, f_, factor);
        auto ge = abo::util::exists_greater_equals(mgr, absolute_difference, multiplied);
        if (ge.second)
        { // the correct value was already found
            return factor;
        }
        if (!ge.first)
        {
            max = factor;
            min = factor == 1.0 ? 0 : (factor / 2.0);
            break;
        }
    }
    while (max - min > precision)
    {
        double middle = (min + max) / 2.0;
        std::vector<BDD> multiplied = abo::util::bdd_multiply_constant(mgr, f_, middle);
        auto ge = abo::util::exists_greater_equals(mgr, absolute_difference, multiplied);
        if (ge.second)
        { // the correct value was already found
            return middle;
        }
        if (ge.first)
        {
            min = middle;
        }
        else
        {
            max = middle;
        }
    }
    return (min + max) / 2.0;
}

cpp_dec_float_100 wcre_symbolic_division(
    const Cudd& mgr, const std::vector<BDD>& f, const std::vector<BDD>& f_hat,
    unsigned int num_extra_bits, const NumberRepresentation num_rep)
{

    std::vector<BDD> absolute_difference =
        abo::util::bdd_absolute_difference(mgr, f, f_hat, num_rep);

    std::vector<BDD> f_abs = abo::util::bdd_abs(mgr, f, num_rep);
    std::vector<BDD> no_zero = abo::util::bdd_max_one(mgr, f_abs);
    std::vector<BDD> divided =
        abo::util::bdd_divide(mgr, absolute_difference, no_zero, num_extra_bits);
    auto max = get_max_value(mgr, divided);
    cpp_dec_float_100 value(max);
    value /= std::pow(2, num_extra_bits);
    return value;
}

std::pair<cpp_dec_float_100, cpp_dec_float_100>
    maximum_relative_value_bounds(const Cudd& mgr,
                                  const std::vector<BDD>& f,
                                  const std::vector<BDD>& g)
{

    BDD zero_so_far = mgr.bddOne();
    cpp_dec_float_100 min_error = 0;
    cpp_dec_float_100 max_error = 0;
    std::vector<BDD> max_one = abo::util::bdd_max_one(mgr, g);
    for (int i = int(g.size()) - 1; i >= 0; i--)
    {
        std::vector<BDD> partial_result;
        partial_result.reserve(f.size());
        BDD modifier = zero_so_far & max_one[i];

        for (const BDD& b : f)
        {
            partial_result.push_back(b & modifier);
        }
        cpp_dec_float_100 max_val =
            cpp_dec_float_100(get_max_value(mgr, partial_result));

        min_error = max(min_error, max_val / (std::pow(2.0, i + 1) - 1));
        max_error = max(max_error, max_val / std::pow(2.0, i));

        zero_so_far &= !max_one[i];
    }

    return {min_error, max_error};
}

std::pair<cpp_dec_float_100, cpp_dec_float_100>
    wcre_bounds(const Cudd& mgr,
                const std::vector<BDD>& f,
                const std::vector<BDD>& f_hat,
                const util::NumberRepresentation num_rep)
{

    std::vector<BDD> absolute_difference =
        abo::util::bdd_absolute_difference(mgr, f, f_hat, num_rep);

    std::vector<BDD> f_abs = abo::util::bdd_abs(mgr, f, num_rep);
    std::vector<BDD> abs_max_one = abo::util::bdd_max_one(mgr, f_abs);
    return maximum_relative_value_bounds(mgr, absolute_difference, abs_max_one);
}

} // namespace abo::error_metrics
