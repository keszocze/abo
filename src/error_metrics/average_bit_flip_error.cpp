#include "average_bit_flip_error.hpp"
#include <cassert>
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

}
