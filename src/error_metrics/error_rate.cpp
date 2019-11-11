//
// Created by keszocze on 10.10.18.
//

#include "error_rate.hpp"
#include "cudd_helpers.hpp"


namespace abo::error_metrics {


    double
    error_rate(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {


        BDD miter_bdd = mgr.bddZero();



        // maybe: use some kind of zip function as soon as C++ actually supports that in a non-ugly fassion
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
        for (unsigned int i = 0;i<f.size();i++) {
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

    double error_rate_efficient_sampling(const Cudd &mgr, const std::vector<BDD>& f, const std::vector<BDD>& f_hat, long samples) {
            assert(f.size() == f_hat.size());
            std::vector<BDD> difference;

            difference.reserve(f.size());
            for (unsigned int i = 0;i<f.size();i++) {
                difference.push_back(f[i] ^ f_hat[i]);
            }

            unsigned int terminal_level = mgr.ReadSize();

            double maximum_rate = 0;
            std::vector<std::pair<int, double>> activations;
            activations.reserve(difference.size());
            for (unsigned int i = 0;i<difference.size();i++) {
                double percentage = Cudd_CountMinterm(mgr.getManager(), difference[i].getNode(), terminal_level - 1) / std::pow(2.0, terminal_level - 1);
                activations.push_back({i, percentage});
                maximum_rate += percentage;
            }
            std::sort(activations.begin(), activations.end(), [](const std::pair<int, double> &a, const std::pair<int, double> &b) { return a.second > b.second; });

            double actual_rate = activations[0].second;
            for (unsigned int i = 1;i<difference.size();i++) {
                long index_samples = samples * activations[i].second / (maximum_rate - activations[0].second);
                if (index_samples == 0) {
                    continue;
                }

                std::map<DdNode*, double> minterm_count = abo::util::count_minterms(difference[activations[i].first]);
                long good_samples = 0;
                for (int b = 0;b<index_samples;b++) {
                    std::vector<int> input = abo::util::random_satisfying_input(difference[activations[i].first], minterm_count, terminal_level);
                    bool found = false;
                    for (unsigned int j = 0;j<i;j++) {
                        if (difference[activations[j].first].Eval(input.data()).IsOne()) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        good_samples++;
                    }
                }
                actual_rate += activations[i].second * double(good_samples) / double(index_samples);
            }

            return std::min(1.0, actual_rate);
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
