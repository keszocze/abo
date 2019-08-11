#include "approximation_operators.hpp"
#include "cudd_helpers.hpp"

#include <map>

#include <cudd/cudd/cudd.h>
#include <cudd_helpers.hpp>

namespace abo::operators {

    static DdNode *round_rec(DdManager * dd, DdNode *node, unsigned int level_start, unsigned int level_end,
                             const std::map<DdNode*, double> &minterm_count, std::map<DdNode*, DdNode*> &round_map,
                             bool remove_heavy, bool remove_light) {

        if (Cudd_IsConstant(node)) {
            return node;
        }

        auto it = round_map.find(node);
        if (it != round_map.end()) {
            return it->second;
        }

        DdNode *N = Cudd_Regular(node);
        DdNode *Nv = Cudd_T(N);
        DdNode *Nnv = Cudd_E(N);

        /* complement if necessary */
        Nv = Cudd_NotCond(Nv, Cudd_IsComplement(node));
        Nnv = Cudd_NotCond(Nnv, Cudd_IsComplement(node));

        unsigned int varId = Cudd_NodeReadIndex(N);

        DdNode *then_branch, *else_branch;
        if (varId < level_start) {
            then_branch = round_rec(dd, Nv, level_start, level_end, minterm_count, round_map, remove_heavy, remove_light);
            else_branch = round_rec(dd, Nnv, level_start, level_end, minterm_count, round_map, remove_heavy, remove_light);
        } else if (varId >= level_start && varId <= level_end) {
            auto then_minterm_count = minterm_count.find(Nv);
            auto else_minterm_count = minterm_count.find(Nnv);
            if (then_minterm_count == minterm_count.end() || else_minterm_count == minterm_count.end()) {
                throw std::logic_error("round_rec: node should be in map");
            }

            if (then_minterm_count->second > else_minterm_count->second && remove_heavy) {
                then_branch = Cudd_ReadOne(dd);
            } else if (then_minterm_count->second <= else_minterm_count->second && remove_light) {
                then_branch = Cudd_Not(Cudd_ReadOne(dd));
            } else {
                then_branch = round_rec(dd, Nv, level_start, level_end, minterm_count, round_map, remove_heavy, remove_light);
            }

            if (else_minterm_count->second > then_minterm_count->second && remove_heavy) {
                else_branch = Cudd_ReadOne(dd);
            } else if (else_minterm_count->second <= then_minterm_count->second && remove_light) {
                else_branch = Cudd_Not(Cudd_ReadOne(dd));
            } else {
                else_branch = round_rec(dd, Nnv, level_start, level_end, minterm_count, round_map, remove_heavy, remove_light);
            }
        } else {
            return node;
        }
        Cudd_Ref(then_branch);
        Cudd_Ref(else_branch);

        DdNode *topv = Cudd_ReadVars(dd, varId);
        Cudd_Ref(topv);
        DdNode *neW =  Cudd_bddIte(dd, topv, then_branch, else_branch);
        if (neW != nullptr) {
            // TODO: is this necessary?
            Cudd_Ref(neW);
        }
        Cudd_RecursiveDeref(dd, topv);
        Cudd_RecursiveDeref(dd, then_branch);
        Cudd_RecursiveDeref(dd, else_branch);

        round_map[node] = neW;

        return neW;
    }

    BDD subset_light_child(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end) {
        std::map<DdNode*, DdNode*> round_map;
        DdNode *node = round_rec(mgr.getManager(), bdd.getNode(), level_start, level_end, abo::util::count_minterms(bdd), round_map, false, true);
        return BDD(mgr, node);
    }

    BDD superset_heavy_child(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end) {
        std::map<DdNode*, DdNode*> round_map;
        DdNode *node = round_rec(mgr.getManager(), bdd.getNode(), level_start, level_end, abo::util::count_minterms(bdd), round_map, true, false);
        return BDD(mgr, node);
    }

    BDD round(const Cudd &mgr, const BDD &bdd, unsigned int level) {
        std::map<DdNode*, DdNode*> round_map;
        // the end level does not matter since the recursion will not reach it
        DdNode *node = round_rec(mgr.getManager(), bdd.getNode(), level, level + 1, abo::util::count_minterms(bdd), round_map, true, true);
        return BDD(mgr, node);
    }

}
