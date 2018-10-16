//
// Created by keszocze on 27.09.18.
//

#include "cudd_helpers.hpp"
#include "string_helpers.hpp"

#include <iostream>
#include <cudd/cudd/cudd.h>

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

    // func  https://stackoverflow.com/questions/47704600/cudd-access-bdd-childs

}