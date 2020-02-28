//
// Created by keszocze on 27.09.18.
//

#include "cudd_helpers.hpp"
#include "string_helpers.hpp"

#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <cudd/cudd/cudd.h>
#include <tuple>
#include <map>
#include <set>
#include <stack>
#include <algorithm>
#include <assert.h>
#include <cmath>

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

    std::map<DdNode*, double> count_minterms(const BDD &bdd) {
        std::map<DdNode*, double> result;

        count_minterms_rec(bdd.getNode(), result);
        return result;
    }

    static double count_solutions_rec(DdNode* node, std::map<DdNode*, double> &solutions_map, int terminal_level) {
        auto it = solutions_map.find(node);
        if (it != solutions_map.end()) {
            return it->second;
        }

        if (Cudd_IsConstant(node)) {
            double value = Cudd_V(node);
            if (Cudd_IsComplement(node)) {
                value = value == 0.0 ? 1.0 : 0.0;
            }
            solutions_map[node] = value;
            return value;
        }

        DdNode *N = Cudd_Regular(node);

        DdNode *Nv = Cudd_T(N);
        DdNode *Nnv = Cudd_E(N);

        Nv = Cudd_NotCond(Nv, Cudd_IsComplement(node));
        Nnv = Cudd_NotCond(Nnv, Cudd_IsComplement(node));

        double high_result = count_solutions_rec(Nv, solutions_map, terminal_level);
        double low_result = count_solutions_rec(Nnv, solutions_map, terminal_level);

        unsigned long high_level = Cudd_IsConstant(Nv) ? terminal_level : Cudd_NodeReadIndex(Nv);
        unsigned long low_level = Cudd_IsConstant(Nnv) ? terminal_level : Cudd_NodeReadIndex(Nnv);
        unsigned long own_level = Cudd_NodeReadIndex(node);

        double solutions = high_result * std::pow(2.0, high_level - own_level - 1) +
                low_result * std::pow(2.0, low_level - own_level - 1);

        solutions_map[node] = solutions;
        return solutions;
    }

    std::map<DdNode*, double> count_solutions(const BDD &bdd) {
        std::map<DdNode*, double> result;

        count_solutions_rec(bdd.getNode(), result, terminal_level({{bdd}}));
        return result;
    }

    std::vector<int> random_satisfying_input(const BDD &bdd, const std::map<DdNode*, double> &minterm_count, int max_level) {
            std::vector<int> result;

            DdNode *node = bdd.getNode();

            for(int level = 0;level < max_level;level++) {
                long node_level = Cudd_IsConstant(node) ? max_level : Cudd_NodeReadIndex(node);
                if (level < node_level) {
                    result.push_back(rand() % 2);
                } else {
                    assert(!Cudd_IsConstant(node));

                    DdNode *N = Cudd_Regular(node);
                    DdNode *then_node = Cudd_T(N);
                    DdNode *else_node = Cudd_E(N);

                    then_node = Cudd_NotCond(then_node, Cudd_IsComplement(node));
                    else_node = Cudd_NotCond(else_node, Cudd_IsComplement(node));

                    double then_weight = minterm_count.at(then_node);
                    double else_weight = minterm_count.at(else_node);

                    double r = rand() / double(RAND_MAX);
                    if (r <= then_weight / (then_weight + else_weight)) {
                        result.push_back(1);
                        node = then_node;
                    } else {
                        result.push_back(0);
                        node = else_node;
                    }
                }
            }

            return result;
        }

    unsigned int const_ADD_value(const ADD &add) {
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
        unsigned int term_level = (max_support_index == support.end() ? 0 : *max_support_index) + 1;

        auto path_count = count_paths(add.getNode(), term_level);

        std::vector<std::pair<double, unsigned long>> result;
        while (toVisit.size() > 0) {
            DdNode *node = toVisit.top();
            toVisit.pop();

            if (Cudd_IsConstant(node)) {
                result.push_back({Cudd_V(node), path_count[node]});
            } else {
                DdNode *thenNode = Cudd_T(node);
                if (visited.find(thenNode) == visited.end()) {
                    // cppcheck-suppress stlFindInsert
                    visited.insert(thenNode);
                    toVisit.push(thenNode);
                }
                DdNode *elseNode = Cudd_E(node);
                if (visited.find(elseNode) == visited.end()) {
                    // cppcheck-suppress stlFindInsert
                    visited.insert(elseNode);
                    toVisit.push(elseNode);
                }
            }
        }
        return result;
    }

    ADD bdd_forest_to_add(const Cudd &mgr, const std::vector<BDD> &bdds, const NumberRepresentation num_rep) {

        std::vector<BDD> bdds_ = bdds;
        if (num_rep == NumberRepresentation::BaseTwo) {
            bdds_.push_back(mgr.bddZero());
        }

        ADD result = mgr.addZero();
        ADD two = mgr.addOne() + mgr.addOne();
        ADD signBitPower = mgr.addOne();

        for (auto it = bdds_.rbegin() + 1;it != bdds_.rend();it++) {
            result *= two;
            signBitPower *= two;
            result += it->Add();
        }

        result -= bdds.back().Add() * signBitPower;

        return result;
    }

    ADD xor_difference_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat) {
        std::vector<BDD> diff;
        for (unsigned int i = 0;i<f.size();i++) {
            diff.push_back(f[i] ^ f_hat[i]);
        }

        return bdd_forest_to_add(mgr, diff, NumberRepresentation::BaseTwo);
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

    ADD absolute_difference_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &f_hat, const NumberRepresentation num_rep) {
        ADD a = bdd_forest_to_add(mgr, f, num_rep);
        ADD b = bdd_forest_to_add(mgr, f_hat, num_rep);
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

    /**
     * @
     */
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

        return diff;
    }

    std::vector<BDD> bdd_absolute_difference(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g,
                                             const NumberRepresentation num_rep) {

        std::vector<BDD> f_= f;
        std::vector<BDD> g_ = g;

        if (num_rep == NumberRepresentation::BaseTwo) {
            // we need to add sign bits to the functions as they don't have one right now
            // these are necessary to be able to add the negative number instead of actually subtracting
            f_.push_back(mgr.bddZero());
            g_.push_back(mgr.bddZero());
        }

        bool smaller = true;
        for (std::size_t i = 0;i<f_.size();i++) {
            if (f_[i] > g_[i]) {
                smaller = false;
                break;
            }
        }

        // use correct order for the difference calculation to minimize computation time
        const std::vector<BDD> &f__ = smaller ? g_ : f_;
        const std::vector<BDD> &g__ = smaller ? f_ : g_;

        std::vector<BDD> difference = abo::util::bdd_subtract(mgr, f__, g__);
        return abo::util::abs(mgr,difference);
    }


    std::vector<BDD> abs(const Cudd &mgr, const std::vector<BDD> &f) {

        // create mask consisting of the sign bit only
        BDD sign_bit = f.back();
        std::vector<BDD> mask(f.size(), sign_bit);

        std::vector<BDD> tmp;

        for (size_t i=0; i < f.size(); ++i) {
            tmp.push_back(f[i] ^ sign_bit);
        }

        std::vector<BDD> abs = abo::util::bdd_subtract(mgr,tmp,mask);

        return abs;
    }

    std::vector<BDD> bdd_shift(const Cudd &mgr, const std::vector<BDD> &f, int bits_to_shift) {
        std::vector<BDD> result;
        result.reserve(f.size());

        for (int i = 0;i<int(f.size());i++) {
            if (i < bits_to_shift || i >= int(f.size()) + bits_to_shift) {
                result.push_back(mgr.bddZero());
            } else {
                result.push_back(f[i - bits_to_shift]);
            }
        }

        return result;
    }

    std::vector<BDD> bdd_add(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g) {
        std::vector<BDD> sum;
        sum.reserve(f.size());

        BDD carry = mgr.bddZero();

        for (unsigned int i = 0; i < f.size(); ++i) {
            auto tmp  = full_adder(f[i], g[i], carry);
            sum.push_back(tmp.first);
            carry = tmp.second;

        }

        return sum;
    }

    std::vector<BDD> bdd_multiply_constant(const Cudd &mgr, const std::vector<BDD> &f, double factor, const unsigned int num_extra_bits) {

        std::vector<BDD> result(f.size() + size_t(std::ceil(std::log2(factor))) + 2, mgr.bddZero());

        std::vector<BDD> fc = f;
        while (result.size() > fc.size()) {
            fc.push_back(mgr.bddZero());
        }

        assert (factor <= static_cast<double>(std::numeric_limits<boost::multiprecision::uint256_t>::max()));

        boost::multiprecision::uint256_t great_factor = static_cast<boost::multiprecision::uint256_t>(factor);
        boost::multiprecision::uint256_t one = 1;
        for (int i = 0;i<256;i++) {
            if (great_factor & (one << i)) {
                result = bdd_add(mgr, result, bdd_shift(mgr, fc, i));
            }
        }
        unsigned long lesser_factor = static_cast<unsigned long>(std::fmod(factor, 1.0) * (1UL << num_extra_bits));
        for (unsigned int i = 0;i<num_extra_bits;i++) {
            if (lesser_factor & (1UL << i)) {
                result = bdd_add(mgr, result, bdd_shift(mgr, fc, -static_cast<int>(num_extra_bits - i)));
            }
        }

        return result;
    }

    void equalize_vector_size(const Cudd &mgr, std::vector<BDD> &f1, std::vector<BDD> &f2) {
        while (f1.size() < f2.size()) {
            f1.push_back(mgr.bddZero());
        }
        while (f2.size() < f1.size()) {
            f2.push_back(mgr.bddZero());
        }
    }

    std::pair<bool, bool> exists_greater_equals(const Cudd &mgr, const std::vector<BDD> &f1, const std::vector<BDD> &f2) {
        std::vector<BDD> f1_ = f1;
        std::vector<BDD> f2_ = f2;

        equalize_vector_size(mgr, f1_, f2_);

        BDD zero_condition = mgr.bddOne();
        BDD equal_condition = mgr.bddOne();
        for (int i = int(f1_.size())-1;i>=0;i--) {
            zero_condition &= !f2_[i];
            if (!((f1_[i] & zero_condition).IsZero())) {
                return {true, false};
            }
            if (!((f1_[i] & !f2_[i] & equal_condition).IsZero())) {
                return {true, false};
            }
            equal_condition &= (f1_[i] & f2_[i]) | ((!f1_[i]) & (!f2_[i]));
        }
        if (!equal_condition.IsZero()) {
            return {true, true};
        }
        return {false, false};
    }

    std::vector<BDD> bdd_max_one(const Cudd &mgr, const std::vector<BDD> &f) {
        std::vector<BDD> result = f;

        BDD zero_condition = mgr.bddOne();
        for (int i = int(result.size())-1;i>0;i--) {
            zero_condition &= !result[i];
        }
        result[0] |= zero_condition;

        return result;
    }

    BDD greater_equals(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g) {
        std::vector<BDD> f_ = f;
        std::vector<BDD> g_ = g;

        equalize_vector_size(mgr, f_, g_);

        BDD zero_condition = mgr.bddOne();
        BDD equal_condition = mgr.bddOne();

        BDD result = mgr.bddZero();
        for (int i = int(f_.size()) - 1; i >= 0; i--) {
            zero_condition &= !g_[i];
            result |= f_[i] & zero_condition;
            result |= f_[i] & !g_[i] & equal_condition;
            equal_condition &= (f_[i] & g_[i]) | ((!f_[i]) & (!g_[i]));
        }
        result |= equal_condition;
        return result;
    }

    std::vector<BDD> bdd_divide(const Cudd &mgr, const std::vector<BDD> &f, const std::vector<BDD> &g, unsigned int extra_bits) {

        std::vector<BDD> values(extra_bits, mgr.bddZero());
        std::vector<BDD> temp = f;
        temp.insert(temp.begin(), values.begin(), values.end());

        std::vector<BDD> g_ = g;
        for (unsigned int i = 0;i<f.size()+1;i++) {
            g_.push_back(mgr.bddZero());
        }
        g_.insert(g_.begin(), values.begin(), values.end());

        std::vector<BDD> result;
        for (int i = int(f.size())+1;i >= -int(extra_bits);i--) {
            auto shifted = bdd_shift(mgr, g_, i);
            BDD subtract_condition = greater_equals(mgr, temp, shifted);
            std::vector<BDD> to_subtract = shifted;
            for (BDD &b : to_subtract) {
                b &= subtract_condition;
            }
            equalize_vector_size(mgr, to_subtract, temp);
            temp = bdd_subtract(mgr, temp, to_subtract);
            result.push_back(subtract_condition);
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

}
