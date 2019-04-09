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

}
