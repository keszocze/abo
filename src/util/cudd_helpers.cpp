//
// Created by keszocze on 27.09.18.
//

#include "cudd_helpers.hpp"
#include "string_helpers.hpp"

namespace abo::util {


    void dump_dot(
            Cudd &mgr,
            const std::vector<BDD>& bdd,
            const std::vector <std::string> &innames,
            const std::vector <std::string> &outnames) {

        char **ins = vec_to_arr(innames);
        char **outs = vec_to_arr(outnames);

        mgr.DumpDot(bdd, ins, outs);

        del_arr(ins, innames.size());
        del_arr(outs, outnames.size());
    }
}