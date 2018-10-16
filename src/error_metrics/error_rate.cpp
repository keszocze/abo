//
// Created by keszocze on 10.10.18.
//

#include "error_rate.hpp"


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


    double error_rate(const Cudd &mgr, const BDD &f, const BDD &f_hat) {
        std::vector<BDD> fv{f};
        std::vector<BDD> fv_hat{f_hat};
        return error_rate(mgr, fv, fv_hat);
    }

}