//
// Created by keszocze on 15.10.18.
//

#include "worst_case_error.hpp"


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
}