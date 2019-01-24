//
// Created by keszocze on 27.09.18.
//

#include "cudd_helpers.hpp"
#include "string_helpers.hpp"

#include <iostream>
#include <cudd/cudd/cudd.h>
#include <tuple>
#include <assert.h>

namespace abo::util {


    void dump_dot(
            const Cudd &mgr,
            const std::vector<BDD> &bdd,
            const std::vector<std::string> &innames,
            const std::vector<std::string> &outnames) {


        if (bdd.empty()) {
            return;
        }

        char **ins = innames.empty() ? nullptr : vec_to_arr(innames);
        char **outs = outnames.empty() ? nullptr : vec_to_arr(outnames);

        mgr.DumpDot(bdd, ins, outs);

        del_arr(ins, innames.size());
        del_arr(outs, outnames.size());
    }

    void dump_dot(
            const Cudd &mgr,
            const BDD &bdd,
            const std::vector<std::string> &inames,
            const std::string &funname) {
        std::vector<BDD> bddv{bdd};
        std::vector<std::string> funnames{funname};
        dump_dot(mgr, bddv, inames, funnames);
    }


    /*
     * How to get the children of nodes has been taken from StackOverflow:
     * https://stackoverflow.com/questions/47704600/cudd-access-bdd-childs
     */

    BDD high(const Cudd &mgr, const BDD &v) {
        DdNode *n = v.getNode();

        if (Cudd_IsConstant(n)) {
            throw "high(mgr,n): Cannot retrieve child of a terminal node";
        }


        if (Cudd_IsComplement(n)) {
            return !BDD(mgr, Cudd_Regular(Cudd_T(n)));
        } else {
            return BDD(mgr, Cudd_T(n));
        }
    }


    BDD low(const Cudd &mgr, const BDD &v) {
        DdNode *n = v.getNode();

        if (Cudd_IsConstant(n)) {
            throw "low(mgr,n): Cannot retrieve child of a terminal node";
        }

        if (Cudd_IsComplement(n)) {
            return !BDD(mgr, Cudd_Regular(Cudd_E(n)));
        } else {
            return BDD(mgr, Cudd_E(n));
        }
    }

    std::pair<BDD, BDD> full_adder(const BDD &f, const BDD &g, const BDD &carry_in) {
        BDD carry_out = (f * g) | (f * carry_in) | (g * carry_in);
        BDD sum = g ^g ^carry_in;
        return {sum, carry_out};
    }

    std::vector<BDD>
    bdd_subtract(const Cudd &mgr, const std::vector<BDD> &minuend, const std::vector<BDD> &subtrahend) {
        std::vector<BDD> diff;
        diff.reserve(minuend.size());


        // using one as carry in serves as an implicit method to add one to the subtrahend, which is necessary to
        // change its sign
        BDD carry = mgr.bddOne();

        for (size_t i = 0; i < minuend.size(); ++i) {
            // the subtrahend's bits are negated to change the sign of the subtrahend
            auto tmp  = full_adder(minuend[i], !subtrahend[i], carry);
            diff.push_back(tmp.first);
            carry = tmp.second;

        }

        std::cout << "diff.size() " << diff.size() << "\n";

        return diff;
    }


    std::vector<BDD> abs(const Cudd &mgr, const std::vector<BDD> &f) {

        // mask consisting of the sign bit only
        BDD sign_bit = f.back();
        std::vector<BDD> mask(f.size(), sign_bit);

        std::vector<BDD> tmp;

        for (size_t i=0; i < f.size(); ++i) {
            tmp.push_back(f[i] ^ sign_bit);
        }

        std::vector<BDD> abs = abo::util::bdd_subtract(mgr,tmp,mask);

        return abs;
    }



}