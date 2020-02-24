#include "approximation_operators.hpp"
#include "cudd_helpers.hpp"

#include <map>

#include <cudd/cudd/cudd.h>
#include <cudd_helpers.hpp>

namespace abo::operators {

    static DdNode *remove_children_rec(
            DdManager *dd, DdNode *node,
            unsigned int level_start,
            unsigned int level_end,
            const std::map<DdNode *, double> &minterm_count,
            std::map<DdNode *, DdNode *> &round_map,
            bool remove_heavy, bool subset);

    static DdNode *round_rec(
            DdManager *dd,
            DdNode *node,
            unsigned int level_start,
            const std::map<DdNode *, double> &minterm_count,
            std::map<DdNode *, DdNode *> &round_map);


    static DdNode *round_best_rec(
            DdManager *dd,
            DdNode *node,
            unsigned int level_start,
            unsigned int level_end,
            const std::map<DdNode *, double> &minterm_count,
            std::map<DdNode *, DdNode *> &round_map);

    static BDD round_any(
            const Cudd &mgr,
            const BDD &bdd,
            const unsigned int level_start,
            const unsigned int level_end,
            bool remove_heavy,
            bool subset);

    BDD subset_light_child(const Cudd &mgr, const BDD &bdd, const unsigned int level_start,
                           const unsigned int level_end) {
        return round_any(mgr, bdd, level_start, level_end, false, true);
    }

    BDD superset_heavy_child(const Cudd &mgr, const BDD &bdd, const unsigned int level_start,
                             const unsigned int level_end) {
        return round_any(mgr, bdd, level_start, level_end, true, false);
    }

    BDD superset_light_child(const Cudd &mgr, const BDD &bdd, const unsigned int level_start,
                             const unsigned int level_end) {
        return round_any(mgr, bdd, level_start, level_end, false, false);
    }

    BDD subset_heavy_child(const Cudd &mgr, const BDD &bdd, const unsigned int level_start,
                           const unsigned int level_end) {
        return round_any(mgr, bdd, level_start, level_end, true, true);
    }

    BDD round(const Cudd &mgr, const BDD &bdd, const unsigned int level) {
        std::map<DdNode *, DdNode *> round_map;
        DdNode *node = round_rec(mgr.getManager(), bdd.getNode(), level,
                                 abo::util::count_minterms(bdd), round_map);
        return BDD(mgr, node);
    }


    BDD round_best(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end) {
        std::map<DdNode *, DdNode *> round_map;
        DdNode *node = round_best_rec(mgr.getManager(), bdd.getNode(), level_start, level_end,
                                      abo::util::count_minterms(bdd), round_map);
        return BDD(mgr, node);
    }


    BDD round_up(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end) {
        std::map<DdNode *, DdNode *> round_map;
        DdNode *node = remove_children_rec(mgr.getManager(), bdd.getNode(), level_start, level_end,
                                           abo::util::count_minterms(bdd), round_map, false, false);
        return BDD(mgr, node);
    }

    BDD round_down(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end) {
        std::map<DdNode *, DdNode *> round_map;
        DdNode *node = remove_children_rec(mgr.getManager(), bdd.getNode(), level_start, level_end,
                                           abo::util::count_minterms(bdd), round_map, false, true);
        return BDD(mgr, node);
    }

    static DdNode *remove_children_rec(
            DdManager *const dd,
            DdNode *const node,
            const unsigned int level_start,
            const unsigned int level_end,
            const std::map<DdNode *, double> &minterm_count,
            std::map<DdNode *, DdNode *> &round_map,
            const bool remove_heavy,
            const bool subset) {

        if (Cudd_IsConstant(node)) {
            return node;
        }

        const auto it = round_map.find(node);
        if (it != round_map.end()) {
            return it->second;
        }

        DdNode *const N = Cudd_Regular(node);
        DdNode *Nv = Cudd_T(N);
        DdNode *Nnv = Cudd_E(N);

        /* complement if necessary */
        Nv = Cudd_NotCond(Nv, Cudd_IsComplement(node));
        Nnv = Cudd_NotCond(Nnv, Cudd_IsComplement(node));

        const unsigned int varId = Cudd_NodeReadIndex(N);

        DdNode *then_branch;
        DdNode *else_branch;

        if (varId < level_start) {
            then_branch = remove_children_rec(dd, Nv, level_start, level_end, minterm_count, round_map, remove_heavy,
                                              subset);
            else_branch = remove_children_rec(dd, Nnv, level_start, level_end, minterm_count, round_map, remove_heavy,
                                              subset);
        } else if (varId >= level_start && varId <= level_end) {
            auto then_minterm_count = minterm_count.find(Nv);
            auto else_minterm_count = minterm_count.find(Nnv);
            if (then_minterm_count == minterm_count.end() || else_minterm_count == minterm_count.end()) {
                throw std::logic_error("remove_children_rec: node should be in map");
            }

            bool then_is_heavy = then_minterm_count->second > else_minterm_count->second;

            if (remove_heavy == then_is_heavy) {
                then_branch = subset ? Cudd_Not(Cudd_ReadOne(dd)) : Cudd_ReadOne(dd);
                else_branch = remove_children_rec(dd, Nnv, level_start, level_end, minterm_count, round_map,
                                                  remove_heavy, subset);
            } else {
                then_branch = remove_children_rec(dd, Nv, level_start, level_end, minterm_count, round_map,
                                                  remove_heavy, subset);
                else_branch = subset ? Cudd_Not(Cudd_ReadOne(dd)) : Cudd_ReadOne(dd);
            }

        } else {
            return node;
        }
        Cudd_Ref(then_branch);
        Cudd_Ref(else_branch);

        DdNode *const topv = Cudd_ReadVars(dd, varId);
        Cudd_Ref(topv);
        DdNode *const neW = Cudd_bddIte(dd, topv, then_branch, else_branch);
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


    static DdNode *
    round_rec(
            DdManager *const dd,
            DdNode *const node,
            const unsigned int level_start,
            const std::map<DdNode *, double> &minterm_count,
            std::map<DdNode *, DdNode *> &round_map) {

        if (Cudd_IsConstant(node)) {
            return node;
        }

        const auto it = round_map.find(node);
        if (it != round_map.end()) {
            return it->second;
        }

        DdNode *const N = Cudd_Regular(node);
        DdNode *Nv = Cudd_T(N);
        DdNode *Nnv = Cudd_E(N);

        /* complement if necessary */
        Nv = Cudd_NotCond(Nv, Cudd_IsComplement(node));
        Nnv = Cudd_NotCond(Nnv, Cudd_IsComplement(node));

        unsigned int varId = Cudd_NodeReadIndex(N);


        if (varId < level_start) {

            DdNode *then_branch;
            DdNode *else_branch;

            then_branch = round_rec(dd, Nv, level_start, minterm_count, round_map);
            else_branch = round_rec(dd, Nnv, level_start, minterm_count, round_map);

            Cudd_Ref(then_branch);
            Cudd_Ref(else_branch);

            DdNode *topv = Cudd_ReadVars(dd, varId);
            Cudd_Ref(topv);
            DdNode *neW = Cudd_bddIte(dd, topv, then_branch, else_branch);
            if (neW != nullptr) {
                // TODO: is this necessary?
                Cudd_Ref(neW);
            }
            Cudd_RecursiveDeref(dd, topv);
            Cudd_RecursiveDeref(dd, then_branch);
            Cudd_RecursiveDeref(dd, else_branch);

            round_map[node] = neW;

            return neW;
        } else {
            auto count = minterm_count.find(node);
            DdNode *replace = count->second > 0.5 ? Cudd_ReadOne(dd) : Cudd_Not(Cudd_ReadOne(dd));
            round_map[node] = replace;
            return replace;
        }
    }


    static DdNode *round_best_rec(
            DdManager *const dd,
            DdNode *const node,
            const unsigned int level_start,
            const unsigned int level_end,
            const std::map<DdNode *, double> &minterm_count,
            std::map<DdNode *, DdNode *> &round_map) {

        if (Cudd_IsConstant(node)) {
            return node;
        }

        const auto it = round_map.find(node);
        if (it != round_map.end()) {
            return it->second;
        }

        DdNode *const N = Cudd_Regular(node);
        DdNode *Nv = Cudd_T(N);
        DdNode *Nnv = Cudd_E(N);

        /* complement if necessary */
        Nv = Cudd_NotCond(Nv, Cudd_IsComplement(node));
        Nnv = Cudd_NotCond(Nnv, Cudd_IsComplement(node));

        const unsigned int varId = Cudd_NodeReadIndex(N);

        DdNode *then_branch;
        DdNode *else_branch;
        if (varId < level_start) {
            // not at the right level yet, simply follow the BDD down
            then_branch = round_best_rec(dd, Nv, level_start, level_end, minterm_count, round_map);
            else_branch = round_best_rec(dd, Nnv, level_start, level_end, minterm_count, round_map);
        }

            // reached range of variable levels  to perform the rounding on
        else if (varId >= level_start && varId <= level_end) {
            const auto then_minterm_count = minterm_count.find(Nv);
            const auto else_minterm_count = minterm_count.find(Nnv);
            if (then_minterm_count == minterm_count.end() || else_minterm_count == minterm_count.end()) {
                throw std::logic_error("round_best_rec: node should be in map");
            }

            // TODO Fälle erläutern

            if (then_minterm_count->second < else_minterm_count->second &&
                then_minterm_count->second < 1 - else_minterm_count->second) {
                then_branch = Cudd_Not(Cudd_ReadOne(dd));
                else_branch = round_best_rec(dd, Nnv, level_start, level_end, minterm_count, round_map);

            } else if (else_minterm_count->second < then_minterm_count->second &&
                       else_minterm_count->second < 1 - then_minterm_count->second) {
                then_branch = round_best_rec(dd, Nv, level_start, level_end, minterm_count, round_map);
                else_branch = Cudd_Not(Cudd_ReadOne(dd));

            } else if (then_minterm_count->second > else_minterm_count->second &&
                       then_minterm_count->second > 1 - else_minterm_count->second) {
                then_branch = Cudd_ReadOne(dd);
                else_branch = round_best_rec(dd, Nnv, level_start, level_end, minterm_count, round_map);

            } else if (else_minterm_count->second > then_minterm_count->second &&
                       else_minterm_count->second > 1 - then_minterm_count->second) {
                then_branch = round_best_rec(dd, Nv, level_start, level_end, minterm_count, round_map);
                else_branch = Cudd_ReadOne(dd);

            } else {
                then_branch = then_minterm_count->second > 0.5 ? Cudd_ReadOne(dd) : Cudd_Not(Cudd_ReadOne(dd));
                else_branch = else_minterm_count->second > 0.5 ? Cudd_ReadOne(dd) : Cudd_Not(Cudd_ReadOne(dd));
            }

        } else {
            return node;
        }
        Cudd_Ref(then_branch);
        Cudd_Ref(else_branch);

        DdNode *const topv = Cudd_ReadVars(dd, varId);
        Cudd_Ref(topv);
        DdNode *neW = Cudd_bddIte(dd, topv, then_branch, else_branch);
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

    static BDD round_any(const Cudd &mgr, const BDD &bdd, const unsigned int level_start, const unsigned int level_end,
                             bool remove_heavy, bool subset) {
        std::map<DdNode *, DdNode *> round_map;
        DdNode *node = remove_children_rec(mgr.getManager(), bdd.getNode(), level_start, level_end,
                                           abo::util::count_minterms(bdd), round_map, remove_heavy, subset);
        return BDD(mgr, node);
    }


}
