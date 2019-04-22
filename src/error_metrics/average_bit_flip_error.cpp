#include "average_bit_flip_error.hpp"
#include "cudd_helpers.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace abo::error_metrics {

    double average_bit_flip_error(const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        assert(f.size() == f_hat.size());

        int max_support_size = 0;
        for (const auto &bdd : f) {
            max_support_size = std::max(max_support_size, bdd.SupportSize());
        }
        for (const auto &bdd : f_hat) {
            max_support_size = std::max(max_support_size, bdd.SupportSize());
        }

        double result = 0;
        for (unsigned int i = 0;i<f.size();i++) {
            result += (f[i] ^ f_hat[i]).CountMinterm(max_support_size);
        }

        // use double exponentiation to allow for more than 64 input variables
        return result / std::pow(2.0, max_support_size);
    }

    double average_bit_flip_error_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        ADD diff = abo::util::xor_difference_add(mgr, f, f_hat);
        std::vector<std::pair<unsigned long, unsigned long>> terminal_values = abo::util::add_terminal_values(diff);
        unsigned long bit_count_sum = 0;
        unsigned long total_path_count = 0;
        for (auto [value, path_count] : terminal_values) {
            // counts the number of set bits in value
            bit_count_sum += static_cast<unsigned long>(__builtin_popcountll(value)) * path_count;
            total_path_count += path_count;
        }
        return bit_count_sum / double(total_path_count);
    }

}
