#include "average_case_relative_error.hpp"
#include "average_case_error.hpp"
#include "cudd_helpers.hpp"
#include <boost/multiprecision/cpp_int.hpp>

using abo::util::NumberRepresentation;
using boost::multiprecision::cpp_dec_float_100;
using boost::multiprecision::uint256_t;

namespace abo::error_metrics {

std::pair<cpp_dec_float_100, cpp_dec_float_100>
    average_relative_value(const Cudd& mgr,
                           const std::vector<BDD>& f,
                           const std::vector<BDD>& g)
{

    BDD zero_so_far = mgr.bddOne();

    cpp_dec_float_100 min_average_result = 0;
    cpp_dec_float_100 max_average_result = 0;
    std::vector<BDD> max_one = abo::util::bdd_max_one(mgr, g);
    for (int i = int(g.size()) - 1; i >= 0; i--)
    {
        std::vector<BDD> partial_result;
        partial_result.reserve(max_one.size());
        BDD modifier = zero_so_far & max_one[i];
        for (const BDD& b : f)
        {
            partial_result.push_back(b & modifier);
        }
        auto average = average_value(partial_result);
        min_average_result += average / (std::pow(2.0, i + 1) - 1);
        max_average_result += average / std::pow(2.0, i);

        zero_so_far &= !max_one[i];
    }

    return {min_average_result, max_average_result};
}

std::pair<cpp_dec_float_100, cpp_dec_float_100>
    acre_bounds(const Cudd& mgr, const std::vector<BDD>& f,
              const std::vector<BDD>& f_hat,
              const util::NumberRepresentation num_rep)
{

    std::vector<BDD> absolute_difference =
        abo::util::bdd_absolute_difference(mgr, f, f_hat, num_rep);
    std::vector<BDD> f_absolute = abo::util::bdd_abs(mgr, f, num_rep);
    return average_relative_value(mgr, absolute_difference, f_absolute);
}

cpp_dec_float_100
    acre_add(const Cudd& mgr,
            const std::vector<BDD>& f,
            const std::vector<BDD>& f_hat,
            const NumberRepresentation num_rep)
{

    ADD diff = abo::util::absolute_difference_add(mgr, f, f_hat, num_rep);
    std::vector<BDD> f_absolute = abo::util::bdd_abs(mgr, f, num_rep);
    ADD respective_diff = diff.Divide(
        abo::util::bdd_forest_to_add(mgr, f_absolute).Maximum(mgr.addOne()));
    std::vector<std::pair<double, unsigned long>> terminal_values =
        abo::util::add_terminal_values(respective_diff);

    cpp_dec_float_100 sum = 0;
    uint256_t path_sum = 0;
    for (auto p : terminal_values)
    {
        cpp_dec_float_100 value(p.first);
        sum += value * p.second;
        path_sum += p.second;
    }
    return cpp_dec_float_100(sum) /
           cpp_dec_float_100(path_sum);
}

cpp_dec_float_100
    acre_symbolic_division(const Cudd& mgr,
                            const std::vector<BDD>& f,
                            const std::vector<BDD>& f_hat,
                            unsigned int num_extra_bits,
                            const NumberRepresentation num_rep)
{

    std::vector<BDD> absolute_difference =
        abo::util::bdd_absolute_difference(mgr, f, f_hat, num_rep);

    std::vector<BDD> f_absolute = abo::util::bdd_abs(mgr, f, num_rep);
    std::vector<BDD> no_zero = abo::util::bdd_max_one(mgr, f_absolute);
    std::vector<BDD> divided =
        abo::util::bdd_divide(mgr, absolute_difference, no_zero, num_extra_bits);

    return average_value(divided) / std::pow(2.0, num_extra_bits);
}

} // namespace abo::error_metrics
