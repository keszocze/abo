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
#include <algorithm>
#include <assert.h>

namespace abo::util {

    unsigned int terminal_level(const std::vector<std::vector<BDD>>& bdds) {
        unsigned int max_index = 0;
        for (const auto &f : bdds) {
            for (const BDD &b : f) {
                std::vector<unsigned int> support = b.SupportIndices();
                auto max_support_index = std::max_element(support.begin(), support.end());
                max_index = std::max(max_index, (max_support_index == support.end() ? 0 : *max_support_index) + 1);
            }
        }
        return max_index;
    }

    long eval_adder(const std::vector<BDD> &adder, long input1, long input2, int bits) {
        std::vector<int> bdd_inputs;
        for (int i = 0;i<bits;i++) {
            bdd_inputs.push_back((input1 & (1 << i)) > 0 ? 1 : 0);
            bdd_inputs.push_back((input2 & (1 << i)) > 0 ? 1 : 0);
        }

        long result = 0;
        for (unsigned int i = 0;i<adder.size();i++) {
            if (adder[i].Eval(bdd_inputs.data()).IsOne()) {
                result |= 1 << i;
            }
        }

        return result;
    }

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

    BDD round_down(const Cudd &mgr, const BDD &bdd, unsigned int level_start, unsigned int level_end) {
        std::map<DdNode*, DdNode*> round_map;
        DdNode *node = round_rec(mgr.getManager(), bdd.getNode(), level_start, level_end, count_minterms(bdd), round_map, false, true);
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

    static std::map<DdNode*, unsigned long> count_paths(DdNode* node, unsigned int terminal_level) {
        if (Cudd_IsConstant(node)) {
            return {{node, 1}};
        }

        // do a sort of breadth first search, visiting all variable levels from top to bottom
        std::vector<std::stack<DdNode*>> levels;
        levels.push_back(std::stack<DdNode*>({Cudd_Regular(node)}));

        std::map<DdNode*, unsigned long> node_to_count;
        std::set<DdNode*> visited;
        node_to_count[node] = 1;
        visited.insert(node);
        for (unsigned int level = 0;level<levels.size();level++) {
            // a reference can not be used here since the vector may resize and move the data elsewhere
            std::stack<DdNode*> nodes = levels[level];
            while (nodes.size() > 0) {
                DdNode *current = nodes.top();
                nodes.pop();


                if (!Cudd_IsConstant(current)) {
                    DdNode *then_node = Cudd_Regular(Cudd_T(current));
                    DdNode *else_node = Cudd_Regular(Cudd_E(current));

                    unsigned long current_count = node_to_count[current];

                    unsigned long then_level = Cudd_IsConstant(then_node) ? terminal_level : Cudd_NodeReadIndex(then_node);
                    // the map will automatically use 0 if the node is not yet present
                    node_to_count[then_node] += current_count << (then_level - level - 1);
                    if (visited.find(then_node) == visited.end()) {
                        levels.resize(std::max(levels.size(), then_level+1));
                        levels[then_level].push(then_node);
                        visited.insert(then_node);
                    }

                    unsigned long else_level = Cudd_IsConstant(else_node) ? terminal_level : Cudd_NodeReadIndex(else_node);
                    node_to_count[else_node] += current_count << (else_level - level - 1);
                    if (visited.find(else_node) == visited.end()) {
                        levels.resize(std::max(levels.size(), else_level+1));
                        levels[else_level].push(else_node);
                        visited.insert(else_node);
                    }
                }
            }
        }

        return node_to_count;
    }

    std::vector<std::pair<double, unsigned long>> add_terminal_values(const ADD &add) {
        std::set<DdNode*> visited;
        std::stack<DdNode*> toVisit;
        toVisit.push(add.getNode());
        visited.insert(add.getNode());

        // determine the level that terminal nodes should be interpreted as
        std::vector<unsigned int> support = add.SupportIndices();
        auto max_support_index = std::max_element(support.begin(), support.end());
        unsigned int terminal_level = (max_support_index == support.end() ? 0 : *max_support_index) + 1;

        auto path_count = count_paths(add.getNode(), terminal_level);

        std::vector<std::pair<double, unsigned long>> result;
        while (toVisit.size() > 0) {
            DdNode *node = toVisit.top();
            toVisit.pop();

            if (Cudd_IsConstant(node)) {
                result.push_back({Cudd_V(node), path_count[node]});
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

    ADD xor_difference_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        std::vector<BDD> diff;
        for (unsigned int i = 0;i<f.size();i++) {
            diff.push_back(f[i] ^ f_hat[i]);
        }

        return bdd_forest_to_add(mgr, diff);
    }

    static DdNode * add_absolute_difference_apply(DdManager * dd, DdNode ** f, DdNode ** g) {
        // basically copied from cuddAddApply.c (the operator is modified of course)
        DdNode *F = *f;
        DdNode *G = *g;
        if (Cudd_IsConstant(F) && Cudd_IsConstant(G)) {
            CUDD_VALUE_TYPE value = std::abs(Cudd_V(F) - Cudd_V(G));
            DdNode * res = Cudd_addConst(dd, value);
            return res;
        }
        if (F > G) { /* swap f and g */
            *f = G;
            *g = F;
        }
        return nullptr;
    }

    ADD absolute_difference_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        ADD a = bdd_forest_to_add(mgr, f);
        ADD b = bdd_forest_to_add(mgr, f_hat);
        DdNode *result_node = Cudd_addApply(mgr.getManager(), add_absolute_difference_apply, a.getNode(), b.getNode());
        return ADD(mgr, result_node);
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

    int variable_index(const Cudd& mgr, const BDD& v) {
        //return Cudd_ReadPerm(v.getNode());
    }

    std::vector<BDD> copy_to_mgr(const Cudd &original_mgr, const std::vector<BDD> &f, const Cudd &mgr_to) {

    }

    std::pair<BDD, Cudd> characteristic_function(const Cudd &mgr, const std::vector<BDD> &f, int max_level) {
        /*auto function_support = mgr.SupportIndices(f);

        std::map<unsigned int, unsigned int> index_map;
        std::sort(function_support.begin(), function_support.end());
        for (unsigned int i = 0;i<function_support.size();i++) {
            index_map[function_support[i]] = i;
        }

        Cudd characteristic_manager(function_support.size() + f.size());*/

        std::vector<BDD> output_variables;
        for (unsigned int i = 0;i<f.size();i++) {
            output_variables.push_back(mgr.bddNewVarAtLevel(max_level + i + 1));
        }

        // TODO: copy bdd to new manager
        BDD result = mgr.bddOne();

        for (unsigned int i = 0; i < f.size(); i++) {
            result &= (!output_variables[i] ^ f[i]);
        }

        return std::make_pair(result, mgr);
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

        //std::cout << "diff.size() " << diff.size() << "\n";

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

    std::vector<BDD> generate_random_function_layered(const Cudd &mgr, int num_variables, int layers, int layer_size, int number_of_bdds) {
        std::vector<BDD> result;
        for (int i = 0;i<number_of_bdds;i++) {
            std::vector<BDD> layer;
            for (int b = 0;b<num_variables;b++) {
                layer.push_back(mgr.bddVar(b));
            }
            for (int b = 0;b<layers;b++) {
                std::vector<BDD> next_layer;
                while (next_layer.size() < layer_size) {
                    BDD a = layer[rand() % layer.size()];
                    if (rand() % 2 == 0) a = !a;
                    BDD b = layer[rand() % layer.size()];
                    if (rand() % 2 == 0) b = !b;
                    if (rand() % 5 == 0) {
                        next_layer.push_back(a & b);
                    } else {
                        next_layer.push_back(a ^ b);
                    }
                    if (next_layer.back().nodeCount() < std::min(a.nodeCount(), b.nodeCount())) {
                        next_layer.pop_back();
                    }
                }
                layer = next_layer;
            }
            BDD biggest = layer[0];
            for (const BDD & b : layer) {
                if (b.nodeCount() > biggest.nodeCount()) {
                    biggest = b;
                }
            }
            result.push_back(biggest);
            std::cout <<"Size: "<<biggest.nodeCount()<<std::endl;
        }

        return result;
    }


    std::vector<BDD> generate_random_function(const Cudd &mgr, int num_variables, int complexity, int number_of_bdds) {
        std::vector<BDD> result;
        for (int i = 0;i<number_of_bdds;i++) {
            std::vector<BDD> parts;
            BDD biggest = mgr.bddVar(0);
            for (int b = 0;b<num_variables;b++) {
                parts.push_back(mgr.bddVar(b));
            }
            for (int b = 0;b<complexity;b++) {
                int modification = rand() % 5;
                if (modification == 0) { // not
                    parts.push_back(!parts[rand() % parts.size()]);
                } else {
                    int a_pos = parts.size() - 1 - (rand() % std::min(100UL, parts.size()));
                    int b_pos = parts.size() - 1 - (rand() % std::min(100UL, parts.size()));
                    if (modification <= 2) {
                        parts.push_back(parts[a_pos] & parts[b_pos]);
                    } else {
                        parts.push_back(parts[a_pos] | parts[b_pos]);
                    }
                }
                if (parts.back().nodeCount() >= biggest.nodeCount()) {
                    biggest = parts.back();
                } else {
                    parts.pop_back();
                }
            }
            result.push_back(biggest);
            std::cout <<"Size: "<<biggest.nodeCount()<<std::endl;
        }

        return result;
    }


}
