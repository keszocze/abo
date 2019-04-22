//
// Created by keszocze on 10.10.18.
//

#include "error_rate.hpp"
#include "cudd_helpers.hpp"


namespace abo::error_metrics {


    double
    error_rate(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {


        BDD miter_bdd = mgr.bddZero();



        // TODO use some kind of zip function as soon as C++ actually supports that
        for (std::size_t i = 0; i < f.size(); i++) {
            miter_bdd = miter_bdd | (f[i] ^ f_hat[i]);
        }

        DdManager *dd = mgr.getManager();

        return Cudd_CountMinterm(dd, miter_bdd.getNode(), Cudd_ReadSize(dd));
    }

    double error_rate_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        ADD diff = abo::util::xor_difference_add(mgr, f, f_hat);
        std::vector<std::pair<unsigned long, unsigned long>> terminal_values = abo::util::add_terminal_values(diff);
        unsigned long non_zero_path_count = 0;
        unsigned long total_path_count = 0;
        for (auto [value, path_count] : terminal_values) {
            if (value != 0) {
                non_zero_path_count += path_count;
            }
            total_path_count += path_count;
        }
        return non_zero_path_count / double(total_path_count);
    }

}
