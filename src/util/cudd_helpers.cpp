//
// Created by keszocze on 27.09.18.
//

#include "cudd_helpers.hpp"
#include "string_helpers.hpp"

#include <iostream>
#include <cudd/cudd/cudd.h>
#include <tuple>
#include <map>
#include <set>
#include <stack>
#include <assert.h>

namespace abo::util {

    static double count_minterms_rec(DdNode* node, std::map<DdNode*, double> &minterms_map) {
        auto it = minterms_map.find(node);
        if (it != minterms_map.end()) {
            return it->second;
        }

        if (Cudd_IsConstant(node)) {
            double value = Cudd_V(node);
            if (Cudd_IsComplement(node)) {
                value = value == 0.0 ? 1.0 : 0.0;
            }
            minterms_map[node] = value;
            return value;
        }

        DdNode *N = Cudd_Regular(node);

        DdNode *Nv = Cudd_T(N);
        DdNode *Nnv = Cudd_E(N);

        Nv = Cudd_NotCond(Nv, Cudd_IsComplement(node));
        Nnv = Cudd_NotCond(Nnv, Cudd_IsComplement(node));

        double highResult = count_minterms_rec(Nv, minterms_map);
        double lowResult = count_minterms_rec(Nnv, minterms_map);
        double minTerms = highResult / 2 + lowResult / 2;

        minterms_map[node] = minTerms;
        return minTerms;
    }

    static std::map<DdNode*, double> count_minterms(const BDD &bdd) {
        std::map<DdNode*, double> result;

        count_minterms_rec(bdd.getNode(), result);
        return result;
    }

    static DdNode *round_rec(DdManager * dd, DdNode *node, unsigned int level_start, unsigned int level_end,
                             const std::map<DdNode*, double> minterm_count, std::map<DdNode*, DdNode*> round_map,
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

        return neW;
    }

    BDD round_down(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end) {
        std::map<DdNode*, DdNode*> round_map;
        DdNode *node = round_rec(mgr.getManager(), bdd.getNode(), level_start, level_end, count_minterms(bdd), round_map, false, true);
        count_minterms(BDD(mgr, node));
        return BDD(mgr, node);
    }

    BDD round_up(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end) {
        std::map<DdNode*, DdNode*> round_map;
        DdNode *node = round_rec(mgr.getManager(), bdd.getNode(), level_start, level_end, count_minterms(bdd), round_map, true, false);
        return BDD(mgr, node);
    }

    BDD round(const Cudd &mgr, const BDD &bdd, unsigned int level) {
        std::map<DdNode*, DdNode*> round_map;
        // the end level does not matter since the recursion will not reach it
        DdNode *node = round_rec(mgr.getManager(), bdd.getNode(), level, level + 1, count_minterms(bdd), round_map, true, true);
        return BDD(mgr, node);
    }


    unsigned int const_add_value(const ADD &add) {
        DdNode *node = add.getNode();
        if (Cudd_IsConstant(node)) {
            return static_cast<unsigned int>(Cudd_V(node));
        }
        return 0;
    }

    std::vector<int> add_terminal_values(const ADD &add) {
        std::set<DdNode*> visited;
        std::stack<DdNode*> toVisit;
        toVisit.push(add.getNode());
        visited.insert(add.getNode());

        std::vector<int> result;
        while (toVisit.size() > 0) {
            DdNode *node = toVisit.top();
            toVisit.pop();

            if (Cudd_IsConstant(node)) {
                result.push_back(static_cast<int>(Cudd_V(node)));
            } else {
                DdNode *thenNode = Cudd_T(node);
                if (visited.find(thenNode) == visited.end()) {
                    visited.insert(thenNode);
                    toVisit.push(thenNode);
                }
                DdNode *elseNode = Cudd_E(node);
                if (visited.find(elseNode) == visited.end()) {
                    visited.insert(elseNode);
                    toVisit.push(elseNode);
                }
            }
        }
        return result;
    }

    ADD bdd_forest_to_add(const Cudd &mgr, const std::vector<BDD> &bdds) {
        ADD result = mgr.addZero();
        ADD two = mgr.addOne() + mgr.addOne();
        for (auto it = bdds.rbegin();it != bdds.rend();it++) {
            result *= two;
            result += it->Add();
        }
        return result;
    }


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
            throw std::invalid_argument("high(mgr,v): Cannot retrieve child of a terminal node");
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
            throw std::invalid_argument("low(mgr,v): Cannot retrieve child of a terminal node");
        }

        if (Cudd_IsComplement(n)) {
            return !BDD(mgr, Cudd_Regular(Cudd_E(n)));
        } else {
            return BDD(mgr, Cudd_E(n));
        }
    }

    std::pair<BDD, BDD> full_adder(const BDD &f, const BDD &g, const BDD &carry_in) {
        BDD carry_out = (f * g) | (f * carry_in) | (g * carry_in);
        BDD sum = f ^ g ^carry_in;
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