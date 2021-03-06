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
