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

        unsigned int num_variables = abo::util::terminal_level({f, f_hat}) - 1;
        return Cudd_CountMinterm(dd, miter_bdd.getNode(), num_variables) / std::pow(2.0, num_variables);
    }

    double error_rate_sampling(const std::vector<BDD>& f, const std::vector<BDD>& f_hat, long samples) {
        assert(f.size() == f_hat.size());
        std::vector<BDD> difference;

        difference.reserve(f.size());
        for (int i = 0;i<f.size();i++) {
            difference.push_back(f[i] ^ f_hat[i]);
        }

        unsigned int terminal_level = abo::util::terminal_level({f, f_hat});

        long error_samples = 0;
        std::vector<int> inputs(terminal_level, 0);
        for (int i = 0;i<samples;i++) {
            std::generate(inputs.begin(), inputs.end(), []() { return rand() % 2; });
            for (const BDD &b : difference) {
                if (b.Eval(inputs.data()).IsOne()) {
                    error_samples++;
                    break;
                }
            }
        }
        return double(error_samples) / samples;
    }

    double error_rate_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        ADD diff = abo::util::xor_difference_add(mgr, f, f_hat);
        std::vector<std::pair<double, unsigned long>> terminal_values = abo::util::add_terminal_values(diff);
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

    double error_rate(const Cudd &mgr, const BDD &f, const BDD &f_hat) {
        std::vector<BDD> fv{f};
        std::vector<BDD> fv_hat{f_hat};
        return error_rate(mgr, fv, fv_hat);
    }

}
