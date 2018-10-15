//
// Created by keszocze on 10.10.18.
//

#include "error_rate.hpp"


namespace abo::error_metrics {


    // TODO irgendwie die Anzahl an Variablen automatisch auslesen
    boost::multiprecision::uint256_t
    error_rate(const Cudd &mgr, const size_t n_vars, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {


        BDD miter_bdd = mgr.bddZero();


        for (std::size_t i = 0; i < f.size(); i++) {
            miter_bdd = miter_bdd | (f[i] ^ f_hat[i]);
        }


        CUDD_VALUE_TYPE value;
        DdGen *cube_generator;
        int *cube;
        DdManager *dd = mgr.getManager();

        const boost::multiprecision::uint256_t one = 1;

        boost::multiprecision::uint256_t n_solutions = 0;



        Cudd_ForeachCube(dd, miter_bdd.getNode(), cube_generator, cube, value) {

            unsigned int n_dontcares = 0u;
            for (size_t q = 0; q < n_vars; q++) {
//                    std::cout << cube[q];
                if (cube[q] == 2) {
//                        std::cout << "increasing n_dontcares to " << (n_dontcares + 1) << "\n";
                    ++n_dontcares;
                }

            }
            //std::cout << "increasing n_solutions by " << (one << n_dontcares) << " (" << one << " << " << n_dontcares << ")\n";
            //std::cout << "\n";
            n_solutions += one << n_dontcares;
        }

        return n_solutions;
    }


    boost::multiprecision::uint256_t error_rate(const Cudd & mgr,const size_t n_vars, const BDD& f, const BDD& f_hat) {
        std::vector<BDD> fv{f};
        std::vector<BDD> fv_hat{f_hat};
        return error_rate(mgr,n_vars,fv,fv_hat);
    }

}