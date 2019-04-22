#include "worst_case_bit_flip_error.hpp"
#include "cudd_helpers.hpp"
#include <cassert>

namespace abo::error_metrics {

    unsigned int worst_case_bit_flip_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        assert(f.size() == f_hat.size());

        ADD bit_error_sum = mgr.addZero();
        for (unsigned int i = 0;i<f.size();i++) {
            bit_error_sum += (f[i] ^ f_hat[i]).Add();
        }

        return abo::util::const_add_value(bit_error_sum.FindMax());
    }

    unsigned int worst_case_bit_flip_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        ADD diff = abo::util::xor_difference_add(mgr, f, f_hat);
        std::vector<std::pair<unsigned long, unsigned long>> terminal_values = abo::util::add_terminal_values(diff);
        unsigned int max_flip_error = 0;
        for (auto [value, _] : terminal_values) {
            unsigned int bit_flips = static_cast<unsigned int>(__builtin_popcountll(value));
            max_flip_error = std::max(bit_flips, max_flip_error);
        }
        return max_flip_error;
    }

}
