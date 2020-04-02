//
// Created by keszocze on 15.10.18.
//

#include "worst_case_error.hpp"

#include <cudd_helpers.hpp>

using abo::util::NumberRepresentation;

namespace abo::error_metrics {

boost::multiprecision::uint256_t get_max_value(const Cudd& mgr, const std::vector<BDD>& fun)
{

    /*
     * fun[i] is the i'th bit of the represented natural numnber. f[0] is the least
     * significant bit
     */

    BDD sigma = mgr.bddOne();

    boost::multiprecision::uint256_t error = 0U;
    boost::multiprecision::uint256_t one = 1U;

    /*
     * We use iterators instead of explicit indexing using ints as that would (well, let's be
     * honest: it is not going to happen) theoretically reduce the size of supported functions.
     */

    size_t exponent = fun.size() - 1;

    for (auto iter = fun.rbegin(); iter != fun.rend(); ++iter, --exponent)
    {
        BDD mask = sigma & *iter;

        if (!mask.IsZero())
        {
            error += (one << exponent);
            sigma = mask;
        }
    }

    return error;
}

boost::multiprecision::uint256_t worst_case_error(const Cudd& mgr, const std::vector<BDD>& f,
                                                  const std::vector<BDD>& f_hat,
                                                  const NumberRepresentation num_rep)
{

    std::vector<BDD> absolute_difference =
        abo::util::bdd_absolute_difference(mgr, f, f_hat, num_rep);
    return get_max_value(mgr, absolute_difference);
}

double worst_case_error_percent(const Cudd& mgr, const std::vector<BDD>& f,
                                const std::vector<BDD>& f_hat, const NumberRepresentation num_rep)
{
    return double(worst_case_error(mgr, f, f_hat, num_rep)) / (std::pow(2, f.size()) - 1);
}

boost::multiprecision::uint256_t worst_case_error_add(const Cudd& mgr, const std::vector<BDD>& f,
                                                      const std::vector<BDD>& f_hat,
                                                      const NumberRepresentation num_rep)
{

    ADD diff = abo::util::absolute_difference_add(mgr, f, f_hat, num_rep);
    std::vector<std::pair<double, unsigned long>> terminal_values =
        abo::util::add_terminal_values(diff);

    boost::multiprecision::uint256_t max_value = 0;
    for (auto p : terminal_values)
    {
        max_value = std::max(boost::multiprecision::uint256_t(p.first), max_value);
    }
    return max_value;
}

boost::multiprecision::uint256_t approximate_worst_case_error(const Cudd& mgr,
                                                              const std::vector<BDD>& f,
                                                              const std::vector<BDD>& f_hat, int n,
                                                              const NumberRepresentation num_rep)
{
    assert(f.size() == f_hat.size() && n >= 1);

    std::vector<BDD> f_;
    std::vector<BDD> f_hat_;

    int bit_start = -1;
    int bit_end = -1;
    int offset = num_rep == NumberRepresentation::TwosComplement ? 1 : 0;
    for (int i = static_cast<int>(f.size()) - 1 - offset; i >= 0; i--)
    {
        if (bit_start == -1 && f[i] != f_hat[i])
        {
            bit_start = i;
        }
        if (bit_start >= 0 && bit_start - i >= n)
        {
            bit_end = i;
            break;
        }
        f_.push_back(f[i]);
        f_hat_.push_back(f_hat[i]);
    }
    if (bit_start == -1)
    {
        return 0;
    }
    std::reverse(f_.begin(), f_.end());
    std::reverse(f_hat_.begin(), f_hat_.end());

    if (num_rep == NumberRepresentation::TwosComplement)
    {
        f_.push_back(f.back());
        f_hat_.push_back(f_hat.back());
    }

    std::vector<BDD> absolute_difference =
        abo::util::bdd_absolute_difference(mgr, f_, f_hat_, num_rep);

    unsigned long shift = bit_end + 1;
    boost::multiprecision::uint256_t one = 1;
    // add one to give an upper bound on the error
    return (get_max_value(mgr, absolute_difference) + 1) * (one << shift) - 1;
}

} // namespace abo::error_metrics
