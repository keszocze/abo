//
// Created by keszocze on 15.10.18.
//

#include "worst_case_error.hpp"

#include <cudd_helpers.hpp>

namespace abo::error_metrics {


    boost::multiprecision::uint256_t get_max_value(const Cudd &mgr, const std::vector<BDD> &fun) {

        /*
         * fun[i] is the i'th bit of the represented natural numnber. f[0] is the least
         * significant bit
         */


        BDD sigma = mgr.bddOne();

        boost::multiprecision::uint256_t error = 0u;

        /*
         * We use iterators instead of explicit indexing using ints as that would (well, let's be honest: it is not
         * going to happen) theoretically reduce the size of supported functions.
         */

        size_t exponent = fun.size()-1;

        for (auto iter = fun.rbegin(); iter != fun.rend(); ++iter, --exponent) {
            BDD mask = sigma & *iter;

            if (!mask.IsZero()) {
                error += (1 << exponent);
                sigma = mask;
            }
        }

        return error;
    }

    boost::multiprecision::uint256_t
    worst_case_error(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat,
                         const NumberRepresentation num_rep) {

        std::vector<BDD> f_= f;
        std::vector<BDD> f_hat_ = f_hat;

        if (num_rep == NumberRepresentation::BaseTwo) {
            // we need to add sign bits to the functions as they don't have one right now
            // these are necessary to be able to add the negative number instead of actually subtracting
            f_.push_back(mgr.bddZero());
            f_hat_.push_back(mgr.bddZero());
        }

        std::vector<BDD> difference = abo::util::bdd_subtract(mgr, f_, f_hat_);

        abo::util::dump_dot(mgr, f);
        abo::util::dump_dot(mgr,f_hat);
        abo::util::dump_dot(mgr,difference);
        std::vector<BDD> absolute_difference = abo::util::abs(mgr,difference);
        abo::util::dump_dot(mgr,absolute_difference);


        return get_max_value(mgr,absolute_difference);
    }
}